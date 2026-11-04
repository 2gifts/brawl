#include <ec/ec_mgr.h>
#include <gf/gf_heap_manager.h>
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
ecEffectWork* lbl_805A0164 = 0;
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

void ecMgr::render() {
    m_traceMgr.renderXlu();
    if (unk6C) {
        u32 index = unk70;
        ecEffectWork* work = &unk60[index];
        for (; index <= unk74; ++index, ++work) {
            if (work->unk7C) {
                work->render();
            }
        }
    }
    if (unk40_) {
        gfHeapManager::getHeap(Heaps::Effect);
    }
}

void ecMgr::setHeapInstance(int heapId) {
    if (heapId == 0xFF) {
        unk48 = 3;
    } else {
        unk48 = heapId;
    }
}

void ecMgr::preSet2dScene(bool enabled) {
    if (enabled) {
        unk54 = unk50 ? unk50 : unk4C;
    } else {
        unk54 = unk4C;
    }
}

void ecMgr::setMdlPriority(u32 handleId, int priority) {
    ecEffectWork* work;
    if (handleId && unk6C) {
        if (lbl_805A0164 && lbl_805A0164->unk7C == handleId) {
            work = lbl_805A0164;
            goto workFound;
        } else {
            ecEffectWork* current = &unk60[unk70];
            for (u32 index = unk70; index <= unk74; ++index, ++current) {
                if (current->unk7C == handleId) {
                    lbl_805A0164 = current;
                    work = current;
                    goto workFound;
                }
            }
        }
    }
    work = 0;
workFound:
    if (work && work->unk44) {
        nw4r::g3d::ScnMdl* model = work->unk44;
        model->SetPriorityDrawOpa(priority);
        model->SetPriorityDrawXlu(priority);
    }
}
