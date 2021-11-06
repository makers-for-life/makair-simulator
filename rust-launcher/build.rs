//extern crate cc;





fn main() {
   println!("cargo:rustc-link-arg=-omakair-simulator-test.html");
   println!("cargo:rustc-link-arg=-sASYNCIFY");
   cc::Build::new()
        .cpp(true) 
        .file("../srcs/vc_cmv_controller.cpp")
        .file("../srcs/gxx_personality_v0_stub.cpp")
        .file("../srcs/simulator.cpp")
        .file("../srcs/telemetry.cpp")
        .file("../srcs/buzzer_control.cpp")
        .file("../srcs/vc_ac_controller.cpp")
        .file("../srcs/activation.cpp")
        .file("../srcs/battery.cpp")
        .file("../srcs/Arduino.cpp")
        .file("../srcs/pressure_valve.cpp")
        .file("../srcs/alarm_controller.cpp")
        .file("../srcs/main_controller.cpp")
        .file("../srcs/buzzer.cpp")
        .file("../srcs/pc_vsai_controller.cpp")
        .file("../srcs/rpi_watchdog.cpp")
        .file("../srcs/pc_ac_controller.cpp")
        .file("../srcs/CRC32.cpp")
        .file("../srcs/blower.cpp")
        .file("../srcs/model.cpp")
        .file("../srcs/serial_control.cpp")
        .file("../srcs/pc_cmv_controller.cpp")
        .file("../srcs/serialib.cpp")
        .file("../srcs/simu_state_machine.cpp")
        .file("../srcs/patient_model.cpp")
        .file("../srcs/alarm.cpp")
        .file("../srcs/main.cpp")
        .file("../srcs/cpu_load.cpp")
        .compile("cppsimulator");

}
