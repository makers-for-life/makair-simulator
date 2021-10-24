extern crate libc;
use std::thread;
use std::time::Duration;

extern {
    fn serialBufferSize() -> libc::c_int;
}

extern {
    fn getTXSerialBufferPointer() -> *const libc::c_uchar;
}

extern {
    fn getTXSerialBufferIndexPointer() -> *const libc::c_int;
}

extern {
    fn run_simulator() -> libc::c_int;
}

fn main() {

    

    unsafe { run_simulator()};
    let size_of_buffer = unsafe{serialBufferSize()};
    println!("Size of TX buffer = {}",size_of_buffer);
    println!("Value of TX buffer index = {}",unsafe{*getTXSerialBufferIndexPointer()});
    for i in 0..size_of_buffer {
        println!("TX buffer[{}] = {}",i, unsafe{ *getTXSerialBufferPointer().offset(i as isize) });
    }
    
    
}