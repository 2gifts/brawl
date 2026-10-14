#pragma once

#include <so/so_module_accesser.h>
#include <types.h>

// Modules of the accesser whose classes are not reconstructed yet are reached through their vtable slots. `vptr` is the
// offset of the vtable pointer inside the module, `slot` the byte offset of the entry in the vtable.
// HYPOTHESIS: the module and method names; only the slot offsets and the argument shapes are established.
inline void* wnModuleAt(const soModuleAccesser* acc, int offset) {
    return *reinterpret_cast<void**>(reinterpret_cast<char*>(const_cast<soModuleEnumeration*>(acc->m_enumerationStart)) + offset);
}

inline void* wnSlotFn(void* module, int vptr, int slot) {
    return *reinterpret_cast<void**>(*reinterpret_cast<char**>(reinterpret_cast<char*>(module) + vptr) + slot);
}

template <class R>
inline R wnCall(void* module, int vptr, int slot) {
    return reinterpret_cast<R (*)(void*)>(wnSlotFn(module, vptr, slot))(module);
}

template <class R, class A>
inline R wnCall(void* module, int vptr, int slot, A a) {
    return reinterpret_cast<R (*)(void*, A)>(wnSlotFn(module, vptr, slot))(module, a);
}
