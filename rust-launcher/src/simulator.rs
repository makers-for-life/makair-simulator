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

            std::thread::spawn(|| {
                unsafe { 
                    setStateOn();
                    run_simulator() };
                error!("run_simulator stopped working");
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
