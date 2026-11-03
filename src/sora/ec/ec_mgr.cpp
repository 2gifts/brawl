#include <ec/ec_mgr.h>
#include <revolution/MTX/mtx.h>
#include <sr/sr_common.h>
#include <types.h>

// ecMgr::loadResource (the map name); st_loader_stage.cpp calls it by this stand-in name.
extern "C" void fn_8005EA4C(ecMgr* mgr, int resourceId, void* data, u32 size, int unk1, int unk2);

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
Mtx lbl_80497810 __attribute__((aligned(8))) = { 0 };
const void** lbl_80497840[0x138] __attribute__((aligned(8))) = { 0 };
extern const u8 lbl_80407060[];
extern const void* lbl_80449F98[];

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
ecMgr* g_ecMgr = 0;
unsigned char lbl_805A014C[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0150[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0154[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0158[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A015C[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0160[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0164[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0168[0x8] __attribute__((aligned(4))) = { 0 };

ecResource::ecResource() {
    unk0 = 0;
    unk4 = 0;
    unk8 = 0;
    unkC = 0;
}

ecResource::~ecResource() {
    release(false);
}

ecEffectWork::ecEffectWork() : unk60(0xFFFF) {
    unk5C = 0;
}

void ecMgr::create(u32 count, u32 resources, u32 limit) {
    const void** data;
    const u8* sizes;
    const void*** entries;
    u32 size;
    sizes = lbl_80407060;
    entries = lbl_80497840;
    data = lbl_80449F98;
    u32 offset = 1;
    for (u32 i = 0; i < 0x137; i++, entries++, sizes++) {
        size = *sizes;
        if (!size) {
            *entries = 0;
        } else {
            *entries = &data[offset];
            offset += size;
        }
    }
    PSMTXIdentity(lbl_80497810);
    if (!g_ecMgr) {
        g_ecMgr = new (Heaps::Effect) ecMgr(count, resources, limit);
    }
}

void ecMgr::registResource(char* name, void* data, int size, int flag) {
    int id = searchResourceID(name);
    if (id < 0x137) {
        if (unk42) {
            fn_8005EA4C(this, id, 0, 0, true, 0xFF);
        }
        fn_8005EA4C(this, id, data, size, flag != 0, 0xFF);
    }
}
