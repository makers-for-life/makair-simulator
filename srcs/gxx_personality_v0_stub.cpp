#include "../includes/config.h"

#ifdef SIMULATOR_WASM

// This file is a "hack" to fix a linking problem when compiling for wasm through rust. see
// https://stackoverflow.com/questions/67474533/error-in-compiling-rust-into-webassembly-using-emscripten-on-windows
// and https://github.com/rust-lang/rust/issues/85821
#include "stdint.h"
#include "unwind.h"

extern "C" _Unwind_Reason_Code __gxx_personality_v0(int version,
                                                    _Unwind_Action actions,
                                                    uint64_t exceptionClass,
                                                    _Unwind_Exception* unwind_exception,
                                                    _Unwind_Context* context) {
    return _URC_NO_REASON;
}

#endif