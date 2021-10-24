extern crate libc;

extern {
    fn run_simulator() -> libc::c_int;
}

fn main() {
    unsafe { run_simulator()};
}