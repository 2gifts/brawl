#pragma once

#include <types.h>

class soModuleAccesser;

// HYPOTHESIS: view of the catch module (enumeration +0x3C, typed void* in the shared header). Only the slots
// used by the throw statuses are named: 0x2C and 0x3C. The filler slots are not identified.
class ftCatchModuleView {
public:
    virtual void unk00();
    virtual void unk04();
    virtual void unk08();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual void notifyThrowExit(int, int); // slot 0x2C
    virtual void unk30();
    virtual void unk34();
    virtual void unk38();
    virtual bool checkThrowDamage(soModuleAccesser* moduleAccesser, void* damageInfo); // slot 0x3C
};
