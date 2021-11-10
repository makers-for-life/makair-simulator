mod simulator;

use makair_telemetry::structures::TelemetryMessage;
use makair_telemetry::TelemetryChannelType;
use std::sync::mpsc::channel;

use simulator::MakAirSimulator;

fn main() {
    // Init logging system
    env_logger::Builder::from_env(env_logger::Env::default().default_filter_or("debug")).init();

    // Create a chennel to receive telemetry messages
    let (tx_messages_sender, tx_messages_receiver) = channel::<TelemetryChannelType>();

    // Initialize simulator
    let mut simulator = MakAirSimulator::new(tx_messages_sender);

    // Start simulator (this will spawn a few threads)
    simulator.run();

    // Listen for telemetry mesages and print them
    std::thread::spawn(move || loop {
        while let Ok(message) = tx_messages_receiver.try_recv() {
            match message {
                Ok(TelemetryMessage::DataSnapshot(_)) => (),
                _ => println!("{:?}", &message),
            };
        }
        std::thread::sleep(std::time::Duration::from_secs(1));
    });
}
