#pragma once

#include <so/so_module_accesser.h>
#include <types.h>

// Modules of the accesser whose classes are not reconstructed yet are reached through their vtable slots. `vptr` is the
// offset of the vtable pointer inside the module, `slot` the byte offset of the entry in the vtable.
// HYPOTHESIS: the module and method names; only the slot offsets and the argument shapes are established.
#define WN_MODULE_SLOT(R, module, vptr, slot, ...) \
    ((R (*)(void*, ##__VA_ARGS__))(*reinterpret_cast<void**>(*reinterpret_cast<char**>(reinterpret_cast<char*>(module) + (vptr)) + (slot))))
