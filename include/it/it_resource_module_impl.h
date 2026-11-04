#pragma once

#include <so/resource/so_resource_module_impl.h>
#include <types.h>

class itResourceModuleImpl : public soResourceModuleImpl {
public:
    // HYPOTHESIS: resource parameter layouts inferred from the item constant lookups.
    u8 unk18[0x18];
    struct {
        float unk0[16];
        u8 unk40[0x20];
        void** unk60;
    }* unk30;
    struct {
        float unk0[42];
        u8 unkA8[0x44];
        void* unkEC;
    }* unk34;
    struct {
        void* unk0;
        void* unk4;
        void* unk8;
        void* unkC;
        void* unk10;
        void* unk14;
        void* unk18;
        void* unk1C;
        void* unk20;
        void* unk24;
        void* unk28;
        void* unk2C;
        void* unk30;
        void* unk34;
        void* unk38;
    }* unk38;
};
