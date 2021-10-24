# makair-simulator - Rust launcher
🥽 [Software] Rust launcher for the makair simulator, using crate cc with FFI.


## How to build for WASM (Web assembly)
1. Install emcc. See https://emscripten.org/docs/getting_started/downloads.html
2. Uncomment `#define EMBEDED_FOR_RUST` in includes/config.h
3. Run `export CXX=emcc; cargo build --target wasm32-unknown-emscripten`
5. On a computer, you might have an error related to "SharedArrayBuffer". Running a webserver on your working directory will fix this (`python3 -m http.server`). Then go to http://0.0.0.0:8000/makair-simulator-test.html
