# MakAir Simulator

## Versions

| Version | Last Changelog | Ready? |
| ------- | -------------- | ------ |
| V1.0.x | Functionnal simulator usable with UI | ❌

## What's In The Box?

This repository contains three parts:

1. A copy of the [MakAir firmware](https://github.com/makers-for-life/makair-firmware) (brought in the `makair-firmware/` directory using a Git submodule).
2. Simulation code: C++ files in the `includes/` and `srcs/` directories that are either symbolic links to the firmware files or "overrides" of the firmware's content in order to simulate hardware. _For example, `srcs/Arduino.cpp` implements Arduino's standard library methods used by the firmware which let use plug a simulation model without modifying the firmware._
3. A Rust library crate that includes the simulator (and firmware) code and exposes an API to run and manage a simulation (including means to get structured telemetry messages out of it, or send it control messages).

## How To Build And Run?

### Building For Linux

In order to setup your environment and build the code, please follow these steps:

1. Install [Rustup](https://rustup.rs/)
2. Ensure you are using the Rust stable toolchain: `rustup default stable`
3. Clone this repo and initialize submodules: `git clone https://github.com/makers-for-life/makair-simulator --recurse-submodules`
4. Build the project: `cargo build`

**You need to be on Linux (or WSL) because this repository contains symlinks that compiler will need to follow.**

#### How To Run The Example Program?

Following the previous steps, then run `cargo run --example main`.

### Building For WebAssembly (WASM)

⚠ _This is probably broken!_

1. Install Emscripten **version 2.0.13** (see https://emscripten.org/docs/getting_started/downloads.html) and activate it (`source ./emsdk_env.sh`)
2. Install Rust nightly: `rustup toolchain install nightly`
3. Run `cargo +nightly build --target wasm32-unknown-emscripten --example main`
4. Run a web server that supports cross-origin isolatation: `python3 httpserver.py`
5. Open http://localhost:8000/makair-simulator-test.html in a web browser

## RCC patient equation

![IMG_20211005_092929](https://user-images.githubusercontent.com/10956689/135980051-f7d4eaaa-6643-4dcf-b2d9-db3d4663749b.jpg)
![IMG_20211005_092937](https://user-images.githubusercontent.com/10956689/135980664-5cc8be00-bf33-4e1c-b68c-276e584252d4.jpg)

## OLD (to refactor/remove?)

### How To Build the simulator on Linux or Mac?

In order to setup your environment and build the code, please follow the following commands :

1. You need a toolchain able to compile c++. On ubuntu : `sudo apt install build-essential `
2. Clone this repo on your machine : `git clone https://github.com/makers-for-life/makair-simulator --recurse-submodules`
3. Go in the build directory `cd makair-simulator/build`
4. In the makair-firmware submodule, uncomment the line "#define SIMULATOR" in includes/config.h
5. Compile the project `make`
6. You get an executable called `simulator` in the build directory

### How To Build the simulator on Windows?

In order to setup your environment and build the code, please follow the following step :

1. Install a c++ compiler. Tested on mingw64 http://mingw-w64.org/doku.php/download/mingw-builds
2. Add the /bin directory of the compiler to the PATH.
3. Install 'Make for Windows" http://gnuwin32.sourceforge.net/packages/make.htm
4. Add the /bin directory of gnuwin32 to the PATH.
5. Clone this repo on your machine : `git clone https://github.com/makers-for-life/makair-simulator --recurse-submodules`
6. Create a new git script, using the command below. This will convert linux symlinks to windows symlinks
```
    git checkout -- "$symlink"
    src="${symdir}/$(cat "$symlink")"

    posix_to_dos_sed='s_^/\([A-Za-z]\)_\1:_;s_/_\\\\_g'
    doslnk=$(printf '%s\n' "$symlink" | sed "$posix_to_dos_sed")
    dossrc=$(printf '%s\n' "$src" | sed "$posix_to_dos_sed")

    if [ -f "$src" ]; then
      rm -f "$symlink"
      cmd //C mklink //H "$doslnk" "$dossrc"
    elif [ -d "$src" ]; then
      rm -f "$symlink"
      cmd //C mklink //J "$doslnk" "$dossrc"
    else
      printf 'error: git-rm-symlink: Not a valid source\n' >&2
      printf '%s =/=> %s  (%s =/=> %s)...\n' \
          "$symlink" "$src" "$doslnk" "$dossrc" >&2
      false
    fi || printf 'ESC[%d]: %d\n' "$ppid" "$?"

    git update-index --assume-unchanged "$symlink"
  done | awk '
    BEGIN { status_code = 0 }
    /^ESC\['"$ppid"'\]: / { status_code = $2 ; next }
    { print }
    END { exit status_code }
  '
}

__git_rm_symlinks
ETX
)"
git config --global alias.rm-symlink '!git rm-symlinks'  # for back-compat.
```
7. Execute the previous command inside the repo `git rm-symlinks`
8. In the `build` directory, replace the content of the Makefile by the content of the MakefileWindows
9. In the makair-firmware submodule, uncomment the line "#define SIMULATOR" in includes/config.h
10. In the `build` directory, run `make`

### How To Run on linux or Mac?

In order to run the simulator that you just built using the instructions above, please follow the following commands:
1. Install socat. On ubuntu : `sudo apt install socat `
2. Create a directory to place the serial port file, for this example a directory called dev `mkdir serialport `
3. Create fakes serial ports with socat : `socat -d -d pty,link=serialport/tty0,raw,echo=0 pty,link=serialport/tty1,raw,echo=0 `
4. Run the simulator, specifying the serial port (`-p`), the lungs resistance (`-r`), the compliance (`-c`),  the spontaneousBreathRate (`--spontaneousBreathRate`),  the spontaneousBreathEffort (`--spontaneousBreathEffort`),  the spontaneousBreathDuration (`--spontaneousBreathDuration`). Example : `./simulator -p serialport/tty0 -r 10 -c 70 --spontaneousBreathRate 20 --spontaneousBreathEffort 5 --spontaneousBreathDuration 200`
5. You can now run the control-ui executable on port `serialport/tty1`. For example: `makair-control -p serialport/tty1`

### How To Run on Windows ?

In order to run the simulator that you just built using the instructions above, please follow the following instructions:
1. Create a fake serial port, using com0com 2.2 : https://sourceforge.net/projects/com0com/files/com0com/2.2.2.0/ . Make sure to download the signed version, otherwise Windows will insult you.
2. Install com0com using `setup.exe` that you have juste downloaded
3. In the install directory of com0com, run `setupc.exe` . It will open a specific command prompt
4. In the command com0com prompt, create a pair of "fake" serial ports : `install PortName=CNCA0 PortName=CNCB0`
5. In the command com0com prompt, set the baudrate emulation to yes : `change CNCA0 EmuBR=yes`
6. Run the simulator, specifying the serial port (`-p`), the lungs resistance (`-r`), and the compliance (`-c`) : `simulator.exe -p \\.\CNCA0  10 -c 70`. As in this example, make sure to have `\\.\` before the name of the serial port.
7. You can now run the control-ui executable on port `CNCB0`. For example: `makair-control.exe -p CNCB0`. For some unknown reason, the `\\.\` prefix is not required for the port name here.

### Easy way to Run the Simulator and the UI (linux only)

You might want to run the simulator in background, with a control UI. You could just use the `install-libs.sh` and the ` install_simulator.sh `bash scripts located in the utils directory of this repo.

`install-libs.sh` script will install the requiered libs

`install_simulator.sh ` script will clone, build and run this repo AND the [control-ui](https://github.com/makers-for-life/makair-control-ui) repo
