use log::error;
use makair_telemetry::control::{ControlMessage, ControlSetting};
use makair_telemetry::structures::TelemetryMessage;
use makair_telemetry::TelemetryChannelType;
use std::sync::mpsc::channel;

use makair_simulator::MakAirSimulator;

fn main() {
    // Init logging system
    env_logger::Builder::from_env(env_logger::Env::default().default_filter_or("debug")).init();

    // If a thread panics, the whole program should go down
    let orig_hook = std::panic::take_hook();
    std::panic::set_hook(Box::new(move |panic_info| {
        orig_hook(panic_info);
        error!("A thread panicked, shutting down the process");
        std::process::exit(1);
    }));

    // Create a channel to receive telemetry messages
    let (tx_messages_sender, tx_messages_receiver) = channel::<TelemetryChannelType>();

    // Initialize simulator
    let mut simulator = MakAirSimulator::new(tx_messages_sender);

    // Start simulator (this will spawn a few threads)
    simulator.initialize(true);

    // Listen for telemetry mesages and print them
    let display_telemetry_messages = std::thread::spawn(move || {
        while let Ok(message) = tx_messages_receiver.recv() {
            match message {
                Ok(TelemetryMessage::DataSnapshot(_)) => (),
                Ok(m) => println!("{:?}", &m),
                _ => println!("{:?}", &message),
            };
        }
    });

    // The following thread simulates a use case of the lifespan of the simulator
    std::thread::spawn(move || {
        std::thread::sleep(std::time::Duration::from_secs(5));
        simulator.pause();
        println!("pause");

        std::thread::sleep(std::time::Duration::from_secs(5));
        simulator.set_resistance(25);
        simulator.set_compliance(10);
        simulator.set_spontaneous_breath_rate(10);
        simulator.set_spontaneous_breath_effort(2);
        simulator.set_spontaneous_breath_duration(200);
        simulator.set_acceleration_factor(10.0);
        println!("Resistance: {}", simulator.resistance());
        println!("Compliance: {}", simulator.compliance());
        println!(
            "SpontaneousBreathRate: {}",
            simulator.spontaneous_breath_rate()
        );
        println!(
            "SpontaneousBreathEffort: {}",
            simulator.spontaneous_breath_effort()
        );
        println!(
            "SpontaneousBreathDuration: {}",
            simulator.spontaneous_breath_duration()
        );
        println!("AccelerationFactor: {}", simulator.acceleration_factor());

        std::thread::sleep(std::time::Duration::from_secs(5));
        simulator.resume();
        println!("resume");

        std::thread::sleep(std::time::Duration::from_secs(5));
        simulator.send_control_message(ControlMessage {
            setting: ControlSetting::PEEP,
            value: 0,
        });
    });


    std::thread::sleep(std::time::Duration::from_secs(20));

    #[cfg(not(target_os = "emscripten"))]
    display_telemetry_messages.join().unwrap();
}
