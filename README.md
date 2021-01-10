# makair-simulator
🥽 [Software] Simulator of the MakAir ventilator. Integrates the makair-firmware, and simulates hardware behavior.


## Versions

| Version | Last Changelog | Ready? |
| ------- | -------------- | ------ |
| V1.0.x | Functionnal simumlator usable with UI | ❌

## How To Build the simulator on Linux or Mac?

In order to setup your environment and build the code, please follow the following commands :

1. You need a toolchain able to compile c++. On ubuntu : `sudo apt install build-essential `
2. Clone this repo on your machine : `git clone https://github.com/makers-for-life/makair-simulator`
3. Go in the build directory `cd makair-simulator/build`
4. Compile the project `make`
5. You get an executable called `simulator` in the build directory

## How To Build the simulator on Windows?

In order to setup your environment and build the code, please follow the following step :

1. Install a c++ compiler. Tested on mingw64 http://mingw-w64.org/doku.php/download/mingw-builds
2. Add the /bin directory of the compiler to the PATH.
3. Install 'Make for Windows" http://gnuwin32.sourceforge.net/packages/make.htm
4. Add the /bin directory of gnuwin32 to the PATH.
5. Clone this repo on your machine : `git clone https://github.com/makers-for-life/makair-simulator`
6. Create a new git script, using the following command `

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
}`

__git_rm_symlinks
ETX
)"

git config --global alias.rm-symlink '!git rm-symlinks'  # for back-compat.


## How To Run?

In order to run the simulator that you just built using the instructions above, please follow the following commands:
1. Install socat. On ubuntu : `sudo apt install socat `
2. Create a directory to place the serial port file, for this example a directory called dev `mkdir serialport `
3. Create fakes serial ports with socat : `socat -d -d pty,link=serialport/tty0,raw,echo=0 pty,link=serialport/tty1,raw,echo=0 `
4. Run the simulator, specifying the serial port (`-p`), the lungs resistance (`-r`), and the compliance (`-c`) : `./simulator -p serialport/tty0 -r 10 -c 70`
5. You can now run the control-ui executable on port `serialport/tty1`. For example: `makair-control -p serialport/tty1`

## Easy way to Run the Simulator and the UI
You might want to run the simulator in background, with a control UI. You could just use the` install_simulator.sh `bash script located in the utils directory of this repo.
This script will clone, build and run this repo AND the [control-ui](https://github.com/makers-for-life/makair-control-ui) repo

