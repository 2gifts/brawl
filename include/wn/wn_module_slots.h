#pragma once

#include <so/so_module_accesser.h>
#include <types.h>

// Modules of the accesser whose classes are not reconstructed yet get minimal interfaces here: the pure virtual pads only
// place the used methods in their vtable slots. HYPOTHESIS: the names; only slot offsets and argument shapes are known.
inline void* wnModuleAt(const soModuleAccesser* acc, int offset) {
    return *reinterpret_cast<void**>(reinterpret_cast<char*>(const_cast<soModuleEnumeration*>(acc->m_enumerationStart)) + offset);
}

struct wnSlotBase8 {
    int unk0, unk4;
};

class wnGlowModuleIface {
public:
    virtual void pad8() = 0;
    virtual void padc() = 0;
    virtual void pad10() = 0;
    virtual void pad14() = 0;
    virtual void pad18() = 0;
    virtual void setAttack(int a) = 0; // +0x1c
    virtual void pad20() = 0;
    virtual void pad24() = 0;
    virtual void pad28() = 0;
    virtual void pad2c() = 0;
    virtual void pad30() = 0;
    virtual void pad34() = 0;
    virtual void pad38() = 0;
    virtual void pad3c() = 0;
    virtual void pad40() = 0;
    virtual void pad44() = 0;
    virtual void pad48() = 0;
    virtual void pad4c() = 0;
    virtual void pad50() = 0;
    virtual void pad54() = 0;
    virtual void pad58() = 0;
    virtual void pad5c() = 0;
    virtual void pad60() = 0;
    virtual void pad64() = 0;
    virtual void pad68() = 0;
    virtual void pad6c() = 0;
    virtual void pad70() = 0;
    virtual void pad74() = 0;
    virtual void setWhole(int a) = 0; // +0x78
};

class wnPhysicsModuleIface : public wnSlotBase8 {
public:
    virtual void pad8() = 0;
    virtual void padc() = 0;
    virtual void pad10() = 0;
    virtual void pad14() = 0;
    virtual void pad18() = 0;
    virtual void pad1c() = 0;
    virtual void pad20() = 0;
    virtual void pad24() = 0;
    virtual void pad28() = 0;
    virtual void unk2c() = 0; // +0x2c
    virtual void unk30(int a) = 0; // +0x30
    virtual void pad34() = 0;
    virtual void pad38() = 0;
    virtual void unk3c() = 0; // +0x3c
    virtual void unk40() = 0; // +0x40
    virtual void pad44() = 0;
    virtual void pad48() = 0;
    virtual void set2nd(u8 a) = 0; // +0x4c
    virtual void changeStatus() = 0; // +0x50
};

