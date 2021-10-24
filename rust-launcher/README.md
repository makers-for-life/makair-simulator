# makair-simulator - Rust launcher
🥽 [Software] Rust launcher for the makair simulator, using crate cc with FFI.


## How to build for WASM (Web assembly)
1. Install emcc. See https://emscripten.org/docs/getting_started/downloads.html
2. Uncomment `#define EMBEDED_FOR_RUST` in includes/config.h
3. Run `export CXX=emcc; cargo build --target wasm32-unknown-emscripten`
5. On a computer, you might have an error related to "SharedArrayBuffer". Running a webserver on your working directory will fix this (`python3 -m http.server`). Then go to http://0.0.0.0:8000/makair-simulator-test.html


## Sucessfully tested with the following setup
**Ubuntu** 20.04.3 LTS

**cargo** 1.57.0-nightly (7fbbf4e8f 2021-10-19)

**emcc** (Emscripten gcc/clang-like replacement + linker emulating GNU ld) 2.0.32 (2213884b85ae8803f4a420349d55e44587364606)

**clang** version 14.0.0 (https://github.com/llvm/llvm-project 9403514e764950a0dfcd627fc90c73432314bced)

**Chrome** Version 95.0.4638.54 (Build officiel) (64 bits)
