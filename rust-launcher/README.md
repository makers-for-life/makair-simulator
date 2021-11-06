# makair-simulator - Rust launcher
🥽 [Software] Rust launcher for the makair simulator, using crate cc with FFI.

## How to build for WASM (Web Assembly)

1. Install Emscripten **version 2.0.13** (see https://emscripten.org/docs/getting_started/downloads.html) and activate it (`source ./emsdk_env.sh`)
2. Run `cargo +nightly build`
3. Run a web server that supports cross-origin isolatation: `python3 httpserver.py`
4. Open http://localhost:8000/makair-simulator-test.html in a Chromium-based web browser

## Sucessfully tested with the following setup

**Ubuntu** 20.04.3 LTS

**rustc** nightly (06/11/2021)

**emcc** (Emscripten gcc/clang-like replacement + linker emulating GNU ld) 2.0.13

**clang** version 14.0.0 (https://github.com/llvm/llvm-project 9403514e764950a0dfcd627fc90c73432314bced)

**Chrome** Version 95.0.4638.54 (Build officiel) (64 bits)
