use log::{error, info, warn};
use makair_telemetry::control::ControlMessage;
use makair_telemetry::{gather_telemetry_from_bytes, TelemetryChannelType};
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

pub struct MakAirSimulator {
    initialized: bool,
    running: bool,
    tx_messages_sender: Sender<TelemetryChannelType>,
    rx_messages_sender: Sender<ControlMessage>,
    rx_messages_receiver: Option<Receiver<ControlMessage>>,
}

impl MakAirSimulator {
    /// Create a new simulator
    ///
    /// _Do not create more than one simulator!_
    pub fn new(tx_messages_sender: Sender<TelemetryChannelType>) -> Self {
        // Create a chennel to send control messages
        let (rx_messages_sender, rx_messages_receiver) = channel::<ControlMessage>();

        Self {
            initialized: false,
            running: false,
            tx_messages_sender,
            rx_messages_sender,
            rx_messages_receiver: Some(rx_messages_receiver),
        }
    }

    /// Spawn several threads in order to run the simulator's loop and communication with the firmware
    pub fn initialize(&mut self) -> bool {
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
                    let rx_last_write_position = unsafe { std::ptr::read(getRXSerialBufferIndexPointer()) };
                    let n = bytes.len() as i32;
                    for i in 0..n {
                        unsafe{ std::ptr::write(getRXSerialBufferPointer().offset(i as isize), bytes[i as usize]) };
                    }
                    unsafe{ std::ptr::write(getRXSerialBufferIndexPointer(), rx_last_write_position+n);}

                }
            });

            // Run the simulation loop
            std::thread::spawn(move || {
                unsafe {
                    // Unpause the simulation
                    setStateOn();
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
}
