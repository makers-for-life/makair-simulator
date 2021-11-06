# makair-simulator - Rust launcher
🥽 [Software] Rust launcher for the makair simulator, using crate cc with FFI.


## How to build for WASM (Web assembly)
1. Install emcc. See https://emscripten.org/docs/getting_started/downloads.html
2. Uncomment `#define EMBEDED_FOR_RUST` in includes/config.h
3. Run `cargo run --target wasm32-unknown-emscripten`
5. You need to run a cross isolated server (`python3 httpserver.py`). Then go to http://localhost:8000/makair-simulator-test.html


## Sucessfully tested with the following setup
**Ubuntu** 20.04.3 LTS

**rustc** nightly (06/11/2021)

**emcc** (Emscripten gcc/clang-like replacement + linker emulating GNU ld) 2.0.13 

**clang** version 14.0.0 (https://github.com/llvm/llvm-project 9403514e764950a0dfcd627fc90c73432314bced)

**Chrome** Version 95.0.4638.54 (Build officiel) (64 bits)
