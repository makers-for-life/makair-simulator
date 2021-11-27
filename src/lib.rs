use log::{error, info, warn};
use makair_telemetry::control::ControlMessage;
use makair_telemetry::{gather_telemetry_from_bytes, TelemetryChannelType};
use std::default::Default;
use std::fmt::Display;
use std::ops::RangeInclusive;
use std::sync::mpsc::{channel, Receiver, Sender};
use std::time::Duration;

extern "C" {
    /// Run the MakAir Simulator
    ///
    /// This must be run in a thread because it never returns.
    fn run_simulator() -> i32;

    /// Size of the Serial buffer exposed by the simulator
    fn serialBufferSize() -> i32;

    /// Pointer to the beginning of the Serial buffer exposed by the simulator
    fn getTXSerialBufferPointer() -> *const u8;

    /// Pointer to the last position in the Serial buffer where the simulator has written
    fn getTXSerialBufferIndexPointer() -> *const i32;

    /// Pointer to the beginning of the Serial buffer exposed by the simulator for receiving data
    fn getRXSerialBufferPointer() -> *mut u8;

    /// Pointer to the last position in the Serial buffer where the rust has written
    fn getRXSerialBufferIndexPointer() -> *mut i32;

    /// Start the simulation
    fn setStateOn();

    /// Pause the simulation
    fn setStateOff();

    /// Set resistance (cmh2O/L/s) of patient model
    fn setResistance(resistance: i32) -> bool;

    /// Set compliance (mL/cmH2O) of patient model
    fn setCompliance(compliance: i32) -> bool;

    /// Set spontaneous breath rate (cycle/min) of patient model
    fn setSpontaneousBreathRate(spontaneousBreathRate: i32) -> bool;

    /// Set spontaneous effort intensity (cmH2O) of patient model
    fn setSpontaneousBreathEffort(spontaneousBreathEffort: i32) -> bool;

    /// Set spontaneous breath duration (ms) of patient model
    fn setSpontaneousBreathDuration(spontaneousBreathDuration: i32) -> bool;

    /// Set acceleration factor of model speed (1.0 means realistic speed)
    fn setAccelerationFactor(accelerationFactor: f32) -> bool;

    /// Get resistance (cmh2O/L/s) of patient model
    fn getResistance() -> i32;

    /// Get compliance (mL/cmH2O) of patient model
    fn getCompliance() -> i32;

    /// Get spontaneous breath rate (cycle/min) of patient model
    fn getSpontaneousBreathRate() -> i32;

    /// Get spontaneous effort intensity (cmH2O) of patient model
    fn getSpontaneousBreathEffort() -> i32;

    /// Get spontaneous breath duration (ms) of patient model
    fn getSpontaneousBreathDuration() -> i32;

    /// Get acceleration factor of model speed
    fn getAccelerationFactor() -> f32;
}

/// The simulator instance
///
/// _Do not create more than one simulator!_
pub struct MakAirSimulator {
    initialized: bool,
    running: bool,
    tx_messages_sender: Sender<TelemetryChannelType>,
    rx_messages_sender: Sender<ControlMessage>,
    rx_messages_receiver: Option<Receiver<ControlMessage>>,
    simulator_setting_sender: Sender<SimulatorSetting>,
    simulator_setting_receiver: Option<Receiver<SimulatorSetting>>,
}

impl MakAirSimulator {
    /// Create a new simulator
    ///
    /// _Do not create more than one simulator!_
    pub fn new(tx_messages_sender: Sender<TelemetryChannelType>) -> Self {
        // Create a channel to send control messages
        let (rx_messages_sender, rx_messages_receiver) = channel::<ControlMessage>();

        // Create a channel to set simulator settings
        let (simulator_setting_sender, simulator_setting_receiver) = channel::<SimulatorSetting>();

        Self {
            initialized: false,
            running: false,
            tx_messages_sender,
            rx_messages_sender,
            rx_messages_receiver: Some(rx_messages_receiver),
            simulator_setting_sender,
            simulator_setting_receiver: Some(simulator_setting_receiver),
        }
    }

    /// Spawn several threads in order to run the simulator's loop and communication with the firmware
    pub fn initialize(&mut self, start_breathing_immediately: bool) -> bool {
        if self.initialized {
            info!("Simulator is already initialized");
            false
        } else {
            info!("Starting simulator");

            // Telemetry
            let (tx_bytes_sender, tx_bytes_receiver) = channel::<Vec<u8>>();
            let tx_messages_sender = self.tx_messages_sender.clone();

            // Control
            let (rx_bytes_sender, rx_bytes_receiver) = channel::<Vec<u8>>();
            let rx_messages_receiver = self.rx_messages_receiver.take().unwrap();

            // Get telemetry bytes from the simulator and send them into a channel
            std::thread::spawn(move || {
                let mut tx_read_position = -1;

                loop {
                    let tx_last_write_position =
                        unsafe { std::ptr::read(getTXSerialBufferIndexPointer()) };

                    if tx_last_write_position != tx_read_position && tx_last_write_position >= 0 {
                        if tx_last_write_position > tx_read_position {
                            tx_bytes_sender
                                .send(Self::extract_data(
                                    tx_read_position + 1,
                                    tx_last_write_position,
                                ))
                                .unwrap();
                        } else {
                            let tx_buffer_size = unsafe { serialBufferSize() };
                            if tx_read_position < tx_buffer_size - 1 {
                                tx_bytes_sender
                                    .send(Self::extract_data(
                                        tx_read_position + 1,
                                        tx_buffer_size - 1,
                                    ))
                                    .unwrap();
                            }
                            tx_bytes_sender
                                .send(Self::extract_data(0, tx_last_write_position))
                                .unwrap();
                        }
                        tx_read_position = tx_last_write_position;
                    }

                    std::thread::sleep(std::time::Duration::from_millis(1));
                }
            });

            // Parse telemetry messages from bytes
            std::thread::spawn(move || {
                gather_telemetry_from_bytes(
                    tx_bytes_receiver,
                    tx_messages_sender,
                    Some(rx_messages_receiver),
                    Some(rx_bytes_sender),
                    Some(Duration::from_millis(1)),
                );
                error!("gather_telemetry_from_bytes stopped working");
            });

            // Get control messages bytes and feed them to the simulator
            std::thread::spawn(move || {
                while let Ok(bytes) = rx_bytes_receiver.recv() {
                    let n = bytes.len() as i32;
                    let rx_buffer_size = unsafe { serialBufferSize() };
                    for i in 0..n {
                        unsafe {
                            let rx_last_write_position =
                                std::ptr::read(getRXSerialBufferIndexPointer());
                            let offset_position = if rx_last_write_position + i >= rx_buffer_size {
                                0
                            } else {
                                rx_last_write_position + 1
                            };
                            std::ptr::write(
                                getRXSerialBufferPointer().offset(offset_position as isize),
                                bytes[i as usize],
                            );
                            std::ptr::write(getRXSerialBufferIndexPointer(), offset_position);
                        };
                    }
                }
            });

            let simulator_setting_receiver = self.simulator_setting_receiver.take().unwrap();
            std::thread::spawn(move || {
                while let Ok(setting) = simulator_setting_receiver.recv() {
                    Self::dangerous_set(setting);
                }
            });

            // Run the simulation loop
            std::thread::spawn(move || {
                unsafe {
                    if start_breathing_immediately {
                        // Unpause the simulation
                        setStateOn();
                    }
                    // Start the infinite loop of the simulator
                    run_simulator()
                };
                error!("run_simulator stopped working");
            });

            self.initialized = true;
            self.running = true;
            true
        }
    }

    fn extract_data(start_offset: i32, end_offset: i32) -> Vec<u8> {
        let capacity = end_offset as usize - start_offset as usize;
        let mut tmp = Vec::with_capacity(capacity);

        for i in start_offset..=end_offset {
            unsafe {
                tmp.push(std::ptr::read(
                    getTXSerialBufferPointer().offset(i as isize),
                ));
            }
        }

        tmp
    }

    /// Send a control message to the simulator
    ///
    /// **This will panic if something fails.**
    pub fn send_control_message(&self, message: ControlMessage) {
        self.rx_messages_sender.send(message).unwrap();
    }

    /// Get a sender that can be used to send control message to the simulator
    pub fn control_message_sender(&self) -> Sender<ControlMessage> {
        self.rx_messages_sender.clone()
    }

    /// Get a sender that can be used to set simulator settings
    pub fn simulator_setting_sender(&self) -> Sender<SimulatorSetting> {
        self.simulator_setting_sender.clone()
    }

    /// Check if the simulator is initialized and running
    pub fn running(&self) -> bool {
        self.initialized && self.running
    }

    /// Ask the simulator to pause execution
    pub fn pause(&mut self) {
        if self.running {
            unsafe { setStateOff() };
            self.running = false;
        }
    }

    /// Ask the simulator to resume execution
    pub fn resume(&mut self) {
        if !self.running {
            unsafe { setStateOn() };
            self.running = true;
        }
    }

    /// Get acceleration factor of model speed
    pub fn acceleration_factor(&self) -> f32 {
        unsafe { getAccelerationFactor() }
    }

    /// Set acceleration factor of model speed (1.0 means realistic speed)
    pub fn set_acceleration_factor(&mut self, value: f32) {
        unsafe { setAccelerationFactor(value) };
    }

    /// Get resistance (cmh2O/L/s) of patient model
    pub fn resistance(&self) -> i32 {
        unsafe { getResistance() }
    }

    /// Set resistance (cmh2O/L/s) of patient model
    pub fn set_resistance(&mut self, value: i32) {
        unsafe { setResistance(value) };
    }

    /// Get compliance (mL/cmH2O) of patient model
    pub fn compliance(&self) -> i32 {
        unsafe { getCompliance() }
    }

    /// Set compliance (mL/cmH2O) of patient model
    pub fn set_compliance(&mut self, value: i32) {
        unsafe { setCompliance(value) };
    }

    /// Get spontaneous breath rate (cycle/min) of patient model
    pub fn spontaneous_breath_rate(&self) -> i32 {
        unsafe { getSpontaneousBreathRate() }
    }

    /// Set spontaneous breath rate (cycle/min) of patient model
    pub fn set_spontaneous_breath_rate(&mut self, value: i32) {
        unsafe { setSpontaneousBreathRate(value) };
    }

    /// Get spontaneous effort intensity (cmH2O) of patient model
    pub fn spontaneous_breath_effort(&self) -> i32 {
        unsafe { getSpontaneousBreathEffort() }
    }

    /// Set spontaneous effort intensity (cmH2O) of patient model
    pub fn set_spontaneous_breath_effort(&mut self, value: i32) {
        unsafe { setSpontaneousBreathEffort(value) };
    }

    /// Get spontaneous breath duration (ms) of patient model
    pub fn spontaneous_breath_duration(&self) -> i32 {
        unsafe { getSpontaneousBreathDuration() }
    }

    /// Set spontaneous breath duration (ms) of patient model
    pub fn set_spontaneous_breath_duration(&mut self, value: i32) {
        unsafe { setSpontaneousBreathDuration(value) };
    }

    /// Set a setting
    fn dangerous_set(setting: SimulatorSetting) {
        if setting.is_valid() {
            match setting.kind {
                SimulatorSettingKind::AccelerationPercent => {
                    unsafe { setAccelerationFactor((setting.value as f32) / 100.0) };
                }
                SimulatorSettingKind::Resistance => {
                    unsafe { setResistance(setting.value) };
                }
                SimulatorSettingKind::Compliance => {
                    unsafe { setCompliance(setting.value) };
                }
                SimulatorSettingKind::SpontaneousBreathRate => {
                    unsafe { setSpontaneousBreathRate(setting.value) };
                }
                SimulatorSettingKind::SpontaneousBreathEffort => {
                    unsafe { setSpontaneousBreathEffort(setting.value) };
                }
                SimulatorSettingKind::SpontaneousBreathDuration => {
                    unsafe { setSpontaneousBreathDuration(setting.value) };
                }
            }
        } else {
            warn!(
                "Tried to set a simulator setting that is invalid: {}",
                &setting
            );
        }
    }

    /// Set a setting
    pub fn set(&mut self, setting: SimulatorSetting) {
        Self::dangerous_set(setting)
    }

    /// Get a setting
    pub fn get(&self, kind: SimulatorSettingKind) -> i32 {
        match kind {
            SimulatorSettingKind::AccelerationPercent => {
                (self.acceleration_factor() * 100.0).round() as i32
            }
            SimulatorSettingKind::Resistance => self.resistance(),
            SimulatorSettingKind::Compliance => self.compliance(),
            SimulatorSettingKind::SpontaneousBreathRate => self.spontaneous_breath_rate(),
            SimulatorSettingKind::SpontaneousBreathEffort => self.spontaneous_breath_effort(),
            SimulatorSettingKind::SpontaneousBreathDuration => self.spontaneous_breath_duration(),
        }
    }
}

#[derive(Debug, Clone, PartialEq)]
pub struct SimulatorSettingStep {
    pub step: i32,
    pub step_if_under_one_hundred: Option<i32>,
}

impl Default for SimulatorSettingStep {
    fn default() -> Self {
        Self {
            step: 1,
            step_if_under_one_hundred: None,
        }
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
/// A simulator setting kind
pub enum SimulatorSettingKind {
    /// Acceleration factor of model speed (100 means realistic speed, can be higher than 100)
    AccelerationPercent,
    /// Resistance (cmh2O/L/s) of patient model
    Resistance,
    /// Compliance (mL/cmH2O) of patient model
    Compliance,
    /// Spontaneous breath rate (cycle/min) of patient model
    SpontaneousBreathRate,
    /// Spontaneous effort intensity (cmH2O) of patient model
    SpontaneousBreathEffort,
    /// Spontaneous breath duration (ms) of patient model
    SpontaneousBreathDuration,
}

impl SimulatorSettingKind {
    /// Allowed value bounds per setting
    pub fn bounds(&self) -> RangeInclusive<i32> {
        match self {
            Self::AccelerationPercent => RangeInclusive::new(10, 1000),
            Self::Resistance => RangeInclusive::new(1, 70),
            Self::Compliance => RangeInclusive::new(1, 150),
            Self::SpontaneousBreathRate => RangeInclusive::new(0, 35),
            Self::SpontaneousBreathEffort => RangeInclusive::new(0, 15),
            Self::SpontaneousBreathDuration => RangeInclusive::new(0, 1500),
        }
    }

    /// Steps to increment or decrement values
    pub fn step(&self) -> SimulatorSettingStep {
        match self {
            Self::AccelerationPercent => SimulatorSettingStep {
                step: 100,
                step_if_under_one_hundred: Some(10),
            },
            Self::Resistance => SimulatorSettingStep {
                step: 2,
                step_if_under_one_hundred: None,
            },
            Self::Compliance => SimulatorSettingStep {
                step: 2,
                step_if_under_one_hundred: None,
            },
            Self::SpontaneousBreathRate => Default::default(),
            Self::SpontaneousBreathEffort => Default::default(),
            Self::SpontaneousBreathDuration => SimulatorSettingStep {
                step: 50,
                step_if_under_one_hundred: None,
            },
        }
    }

    /// Default value
    pub fn default(&self) -> i32 {
        match self {
            Self::AccelerationPercent => 100,
            Self::Resistance => 1,
            Self::Compliance => 1,
            Self::SpontaneousBreathRate => 0,
            Self::SpontaneousBreathEffort => 0,
            Self::SpontaneousBreathDuration => 0,
        }
    }
}

#[derive(Debug, Clone, PartialEq)]
/// A simulator setting with its value
pub struct SimulatorSetting {
    pub kind: SimulatorSettingKind,
    pub value: i32,
}

impl Display for SimulatorSetting {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{:?}={}", self.kind, self.value)
    }
}

impl SimulatorSetting {
    /// Check if a setting is valid
    pub fn is_valid(&self) -> bool {
        self.kind.bounds().contains(&self.value)
    }

    /// Get the incremented (bounded) value
    pub fn increment(&self) -> Self {
        let SimulatorSettingStep {
            step,
            step_if_under_one_hundred,
        } = self.kind.step();

        let new_value = match step_if_under_one_hundred {
            Some(s) if self.value < 100 => self.value + s,
            _ => self.value + step,
        };
        let new_setting = SimulatorSetting {
            kind: self.kind,
            value: new_value,
        };

        if new_setting.is_valid() {
            new_setting
        } else {
            self.clone()
        }
    }

    /// Get the decremented (bounded) value
    pub fn decrement(&self) -> Self {
        let SimulatorSettingStep {
            step,
            step_if_under_one_hundred,
        } = self.kind.step();

        let new_value = match step_if_under_one_hundred {
            Some(s) if self.value <= 100 => self.value - s,
            _ => self.value - step,
        };
        let new_setting = SimulatorSetting {
            kind: self.kind,
            value: new_value,
        };

        if new_setting.is_valid() {
            new_setting
        } else {
            self.clone()
        }
    }
}
