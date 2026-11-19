#include <sr/sr_common.h>
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

// TODO: clean up, give variables real names
namespace jj_8009D2DC {
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void fn_8009D468(void*);
extern "C" void* lbl_805A0280;
extern "C" const float lbl_805A1DC8;
inline void clear(int* begin, int* end) {
    int* p = begin;
    do { *p = 0; ++p; } while (p < end);
}
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void* fn_8009D2DC() {
    if (lbl_805A0280 == 0) {
        char* p = (char*)__nw__FUlQ25Heaps8HeapType(0x274, 1);
        if (p != 0) {
            *(int*)(p + 0x22C) = 0;
            clear((int*)(p + 0x230), (int*)(p + 0x244));
            fn_8009D468(p);
            *(float*)(p + 0x244) = lbl_805A1DC8;
            *(float*)(p + 0x248) = lbl_805A1DC8;
            *(float*)(p + 0x24C) = lbl_805A1DC8;
            *(float*)(p + 0x250) = lbl_805A1DC8;
            *(float*)(p + 0x254) = lbl_805A1DC8;
        }
        lbl_805A0280 = p;
    }
    return lbl_805A0280;
}
}

// TODO: clean up, give variables real names
namespace jj_8009DEE8 {

extern "C" {
void fn_8009D468(void*);
extern void* lbl_805A0280;
extern const float lbl_805A1DC8;
}
inline void clear(int* begin, int* end) {
    int* p = begin;
    do { *p = 0; ++p; } while (p < end);
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8009DEE8() {
    if (lbl_805A0280 == 0) {
        char* p = static_cast<char*>(operator new(0x274, (Heaps::HeapType)1));
        if (p != 0) {
            *reinterpret_cast<int*>(p + 0x22C) = 0;
            clear(reinterpret_cast<int*>(p + 0x230), reinterpret_cast<int*>(p + 0x244));
            fn_8009D468(p);
            float* f = reinterpret_cast<float*>(p + 0x244);
            f[0] = lbl_805A1DC8;
            f[1] = lbl_805A1DC8;
            f[2] = lbl_805A1DC8;
            f[3] = lbl_805A1DC8;
            f[4] = lbl_805A1DC8;
        }
        lbl_805A0280 = p;
    }
    char* s = static_cast<char*>(lbl_805A0280);
    *reinterpret_cast<int*>(s + 0x14) = 0;
    *reinterpret_cast<int*>(s + 0x18) = 0;
    *reinterpret_cast<int*>(s + 0x1C) = 0;
    *reinterpret_cast<int*>(s + 0x20) = 0;
    *reinterpret_cast<int*>(s + 0x24) = 0;
    *reinterpret_cast<int*>(s + 0x28) = 0;
}
}

namespace jj_8009E090 {
struct Obj {
    float x, y, z, w;
    unsigned char pad10[0x22C - 0x10];
    unsigned int unk22C;
    unsigned int unk230;
    unsigned int unk234[4];
    float unk244[5];
};

extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void fn_8009D468(Obj* p);
extern "C" Obj* lbl_805A0280;
extern "C" const float lbl_805A1DC8;

inline void zeroRange(int* begin, int* end) {
    int* p = begin;
    do { *p = 0; ++p; } while (p < end);
}

// TODO: clean up, give variables real names
extern "C" void fn_8009E090(float farg0) {
    if (lbl_805A0280 == 0) {
        Obj* obj = (Obj*)__nw__FUlQ25Heaps8HeapType(0x274, 1);
        if (obj != 0) {
            obj->unk22C = 0;
            zeroRange((int*)&obj->unk230, (int*)obj->unk244);
            fn_8009D468(obj);
            obj->unk244[0] = lbl_805A1DC8;
            obj->unk244[1] = lbl_805A1DC8;
            obj->unk244[2] = lbl_805A1DC8;
            obj->unk244[3] = lbl_805A1DC8;
            obj->unk244[4] = lbl_805A1DC8;
        }
        lbl_805A0280 = obj;
    }
    lbl_805A0280->z = lbl_805A0280->w * farg0;
}
}

