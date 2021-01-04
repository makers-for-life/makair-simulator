# makair-simulator
🥽 [Software] Simulator of the MakAir ventilator. Integrates the makair-firmware, and simulates hardware behavior.


## Versions

| Version | Last Changelog | Ready? |
| ------- | -------------- | ------ |
| V1.0.x | Functionnal simumlator usable with UI | ❌

## How To Build the simulator?

In order to setup your environment and build the code, please follow the following commands :

1. You need a toolchain able to compile c++. On ubuntu : `sudo apt install build-essential `
2. Clone this repo on your machine : `git clone https://github.com/makers-for-life/makair-simulator`
3. Go in the build directory `cd makair-simulator/build`
4. Compile the project `make`
5. You get an executable called `simulator` in the build directory

## How To Run?

In order to run the simulator that you just built using the instructions above, please follow the following commands:
1. Install socat. On ubuntu : `sudo apt install socat `
2. Create a directory to place the serial port file, for this example a directory called dev `mkdir serialport `
3. Create fakes serial ports with socat : `socat -d -d pty,link=serialport/tty0,raw,echo=0 pty,link=serialport/tty1,raw,echo=0 `
4. Run the simulator, specifying the serial port (`-p`), the lungs resistance (`-r`), and the compliance (`-c`) : `./simulator -p serialport/tty0 -r 10 -c 70`
5. You can now run the control-ui executable on port `serialport/tty1`. For example: `makair-control -p serialport/tty1`

## Easy way to Run the Simulator and the UI
You might want to run the simulator in background, with a control UI
1.Do


