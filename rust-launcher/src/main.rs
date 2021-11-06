use std::os::raw;
use std::thread;
use std::time;

extern "C" {
    fn serialBufferSize() -> raw::c_int;
    fn getTXSerialBufferPointer() -> *const raw::c_uchar;
    fn getTXSerialBufferIndexPointer() -> *const raw::c_int;
    fn run_simulator() -> raw::c_int;
}

fn main() {
    thread::spawn(|| {
        let size_of_buffer = unsafe { serialBufferSize() };
        println!("Size of TX buffer = {}", size_of_buffer);
        println!("Value of TX buffer index = {}", unsafe {
            *getTXSerialBufferIndexPointer()
        });

        // Display content of the buffer
        loop {
            print!("Content of TX bufffer: ");
            for i in 0..size_of_buffer {
                print!("{} ", unsafe {
                    *getTXSerialBufferPointer().offset(i as isize)
                });
            }
            println!();
            thread::sleep(time::Duration::from_millis(2000));
        }
    });

    thread::spawn(|| unsafe { run_simulator() });
}
