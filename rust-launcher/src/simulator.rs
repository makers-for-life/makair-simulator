use log::{error, info};
use makair_telemetry::{gather_telemetry_from_bytes, TelemetryChannelType};
use std::sync::mpsc::{channel, Sender};
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

    // Start the simulation
    fn setStateOn();

    // Pause the simulation
    fn setStateOff();

    // Set resistance (cmh2O/L/s) of patient model
    fn setResistance(resistance : i32) -> bool;

    // Set compliance (mL/cmH2O) of patient model
    fn setCompliance(compliance : i32) -> bool;

    // Set spontaneous breath rate (cycle/min) of patient model
    fn setSpontaneousBreathRate(spontaneousBreathRate : i32) -> bool;

    // Set spontaneous effort intensity (cmH2O) of patient model
    fn setSpontaneousBreathEffort(spontaneousBreathEffort : i32) -> bool;

    // Set spontaneous breath duration (ms) of patient model
    fn setSpontaneousBreathDuration(spontaneousBreathDuration : i32) -> bool;

    // Set acceleration factor of model speed
    fn setAccelerationFactor(accelerationFactor : f32) -> bool;

    // Get resistance (cmh2O/L/s) of patient model
    fn getResistance() -> i32;

    // Get compliance (mL/cmH2O) of patient model
    fn getCompliance() -> i32;

    // Get spontaneous breath rate (cycle/min) of patient model
    fn getSpontaneousBreathRate() -> i32;

    // Get spontaneous effort intensity (cmH2O) of patient model
    fn getSpontaneousBreathEffort() -> i32;

    // Get spontaneous breath duration (ms) of patient model
    fn getSpontaneousBreathDuration() -> i32;

    // Get acceleration factor of model speed
    fn getAccelerationFactor() -> f32;
}

pub struct MakAirSimulator {
    running: bool,
    tx_messages_sender: Sender<TelemetryChannelType>,
}

impl MakAirSimulator {
    /// Create a new simulator
    ///
    /// _Do not create more than one simulator!_
    pub fn new(tx_messages_sender: Sender<TelemetryChannelType>) -> Self {
        Self {
            running: false,
            tx_messages_sender,
        }
    }

    /// Start the simulator in a new thread
    pub fn run(&mut self) -> bool {
        if self.running {
            info!("Simulator is already running");
            false
        } else {
            info!("Starting simulator");

            let (bytes_sender, bytes_receiver) = channel::<Vec<u8>>();
            let messages_sender = self.tx_messages_sender.clone();

            std::thread::spawn(move || {
                let mut tx_read_position = -1;

                loop {
                    let tx_last_write_position =
                        unsafe { std::ptr::read(getTXSerialBufferIndexPointer()) };

                    if tx_last_write_position != tx_read_position && tx_last_write_position >= 0 {
                        if tx_last_write_position > tx_read_position {
                            bytes_sender
                                .send(Self::extract_data(
                                    tx_read_position + 1,
                                    tx_last_write_position,
                                ))
                                .unwrap();
                        } else {
                            let tx_buffer_size = unsafe { serialBufferSize() };
                            if tx_read_position < tx_buffer_size-1 {
                                bytes_sender
                                .send(Self::extract_data(tx_read_position + 1, tx_buffer_size-1))
                                .unwrap();
                            }
                            bytes_sender
                                .send(Self::extract_data(0, tx_last_write_position))
                                .unwrap();
                            
                        }
                        tx_read_position = tx_last_write_position;
                    }

                    std::thread::sleep(std::time::Duration::from_millis(1));
                }
           });

            std::thread::spawn(move || {
                gather_telemetry_from_bytes(
                    bytes_receiver,
                    messages_sender,
                    None,
                    None,
                    Some(Duration::from_millis(1)),
                );
                error!("gather_telemetry_from_bytes stopped working");
            });


            std::thread::spawn(move || {
                unsafe { 
                    // Unpause the simulation 
                    setStateOn(); 
                    // Start the infinite loop of the simulator
                    run_simulator() 
                };
                error!("run_simulator stopped working");
            });

            // The following thread simulate a usecase of the slifespan of the simulator
            std::thread::spawn( move || {
                unsafe { 
                    std::thread::sleep(std::time::Duration::from_millis(5000));
                    // pause the simulation 
                    setStateOff(); 
                    println!("setStateOff");
                    std::thread::sleep(std::time::Duration::from_millis(5000));
                    setResistance(25);
                    setCompliance(10);
                    setSpontaneousBreathRate(10);
                    setSpontaneousBreathEffort(2);
                    setSpontaneousBreathDuration(200);
                    setAccelerationFactor(10.0);
                    println!("Resistance : {}", getResistance());
                    println!("Compliance : {}", getCompliance());
                    println!("SpontaneousBreathRate : {}", getSpontaneousBreathRate());
                    println!("SpontaneousBreathEffort : {}", getSpontaneousBreathEffort());
                    println!("SpontaneousBreathDuration : {}", getSpontaneousBreathDuration());
                    println!("AccelerationFactor : {}", getAccelerationFactor());


                    std::thread::sleep(std::time::Duration::from_millis(5000));
                    setStateOn(); 
                    println!("setStateOn");
                }
                    
            });

            

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
}
