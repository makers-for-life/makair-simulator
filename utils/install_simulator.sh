#!/usr/bin/env bash

git clone https://github.com/makers-for-life/makair-simulator --recurse-submodules
git --git-dir makair-simulator/.git --work-tree=makair-simulator checkout test-with-ui
git --git-dir makair-simulator/.git --work-tree=makair-simulator pull
git --git-dir makair-simulator/makair-firmware/.git --work-tree=makair-simulator/makair-firmware/ checkout dev
git --git-dir makair-simulator/makair-firmware/.git --work-tree=makair-simulator/makair-firmware/ pull
make -C makair-simulator/build

git clone https://github.com/makers-for-life/makair-control-ui
git --git-dir makair-control-ui/.git --work-tree=makair-control-ui/ pull
cargo build --manifest-path=makair-control-ui/Cargo.toml
mkdir serialport
socat -d -d pty,link=serialport/tty0,raw,echo=0 pty,link=serialport/tty1,raw,echo=0 & pid="$!"
wait pid
sleep 1
./makair-simulator/build/simulator -p serialport/tty0 &
./makair-control-ui/target/debug/makair-control -p serialport/tty1
