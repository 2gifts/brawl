#include <cm/cm_quake.h>
#include <gf/gf_heap_manager.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <string.h>
#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0280[0x8] __attribute__((aligned(4))) = { 0 };

void cmQuake::init() {
    unk10_0 = 0;
    unk14[0] = 0;
    unk14[1] = 0;
    unk14[2] = 0;
    unk14[3] = 0;
    unk14[4] = 0;
    unk14[5] = 0;
    unk0 = unk4 = 0.0f;
    unk8 = 1.0f;
    unkC = 1.0f;
    for (int index = 0; index < 16; ++index) {
        unk2C[index].unk0 = unk2C[index].unk4 = unk2C[index].unk8 = 0.0f;
        unk2C[index].unkC = 0;
        unk12C[index].unk0 = unk12C[index].unk4 = unk12C[index].unk8 = 0.0f;
        unk12C[index].unkC = 0;
    }
    unk270 = 0;
    unk258 = 0.0f;
    unk25C = 0.0f;
    unk260 = 0.0f;
    unk264 = 0.0f;
    unk268 = 0.0f;
    unk26C = 0.0f;
}

// HYPOTHESIS: five animation-name pointers followed by the remaining rodata word.
extern const struct { struct { const char* unk0[5]; } unk0; u32 unk14; } lbl_804078E0;

bool cmQuake::loadResource(const void* resource, u32 size, Heaps::HeapType heap) {
    if (!resource || !size) return false;
    if (unk22C) {
        if (unk22C) {
            gfHeapManager::free(unk22C);
            unk22C = NULL;
        }
        for (int index = 0; index < 5; ++index) {
            unk230[index] = NULL;
            unk244[index] = 0.0f;
        }
    }
    void* buffer = gfHeapManager::alloc(heap, size, 32);
    if (!buffer) return false;
    memcpy(buffer, resource, size);
    unk22C = buffer;
    nw4r::g3d::ResFile file(buffer);
    gfHeapManager::getMEMAllocator(heap);
    __typeof__(lbl_804078E0.unk0) names = lbl_804078E0.unk0;
    for (int index = 0; index < 5; ++index) {
        unk230[index] = file.GetResAnmChr(names.unk0[index]).ptr();
        if (unk230[index]) unk244[index] = static_cast<int>(unk230[index]->m_animLength);
    }
    return true;
}

void cmQuake::reqQuake(Amplitude amplitude, Vec3f* offset) {
    switch (amplitude) {
    case Amplitude_M:
        if (!(unk270 & 1)) unk26C = 0.0f;
        unk258 = 0.0f;
        unk270 |= 1;
        break;
    case Amplitude_S_Half:
        if ((unk270 & 2) && unk25C < 6.0f) break;
        unk25C = 0.0f;
        unk270 |= 2;
        break;
    case Amplitude_S:
        if ((unk270 & 4) && unk260 < 6.0f) break;
        unk260 = 0.0f;
        unk270 |= 4;
        break;
    case Amplitude_L:
        if ((unk270 & 8) && unk264 < 7.0f) break;
        unk264 = 0.0f;
        unk270 |= 8;
        break;
    case Amplitude_XL:
        if ((unk270 & 0x10) && unk268 < 11.0f) break;
        unk268 = 0.0f;
        unk270 |= 0x10;
        break;
    }
    for (int index = 0; index < 16; ++index) {
        if (unk2C[index].unkC == 0) {
            unk2C[index].unkC = amplitude;
            float x = offset->m_x;
            float y = offset->m_y;
            unk2C[index].unk0 = x;
            float z = offset->m_z;
            unk2C[index].unk4 = y;
            unk2C[index].unk8 = z;
        }
    }
}
