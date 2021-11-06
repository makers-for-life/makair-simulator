extern crate libc;
use std::thread;
use std::os::raw;

extern {
    fn serialBufferSize() -> raw::c_int;
}

extern {
    fn getTXSerialBufferPointer() -> *const raw::c_uchar;
}

extern {
    fn getTXSerialBufferIndexPointer() -> *const raw::c_int;
}

extern {
    fn run_simulator() -> raw::c_int;
}

fn main() {


    thread::spawn(|| {
        let size_of_buffer = unsafe{serialBufferSize()};
        println!("Size of TX buffer = {}",size_of_buffer);
        println!("Value of TX buffer index = {}",unsafe{*getTXSerialBufferIndexPointer()});
        for i in 0..size_of_buffer {
            println!("TX buffer[{}] = {}",i, unsafe{ *getTXSerialBufferPointer().offset(i as isize) });
        }
    });

    thread::spawn(|| {
        unsafe { run_simulator()}
    });
    
    
}