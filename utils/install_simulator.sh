#!/usr/bin/env bash

git clone https://github.com/makers-for-life/makair-simulator --recurse-submodules
git --git-dir makair-simulator/.git --work-tree=makair-simulator checkout test-with-ui
git --git-dir makair-simulator/makair-firmware/.git --work-tree=makair-simulator/makair-firmware/ checkout dev
make -C makair-simulator/build

git clone https://github.com/makers-for-life/makair-control-ui
cargo build --manifest-path=makair-control-ui/Cargo.toml
mkdir dev
socat -d -d pty,link=dev/tty0,raw,echo=0 pty,link=dev/tty1,raw,echo=0 & pid="$!"
wait pid
./makair-simulator/build/simulator -p dev/tty0 &
./makair-control-ui/target/debug/makair-control -p dev/tty1
