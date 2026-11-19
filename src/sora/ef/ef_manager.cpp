#include <gf/gf_camera.h>
#include <ef/ef_manager.h>
#include <ef/ef_particle.h>
#include <gf/gf_application.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_memory_util.h>
#include <gm/gm_global.h>
#include <sr/sr_common.h>
#include <types.h>
#include <ef/ef_screen.h>

using namespace nw4r::ef;

efPostRenderCallback lbl_805A0108;
efManager* lbl_805A0110;

void efManager::create() {
    lbl_805A0110 = new (Heaps::System) efManager();
}

efManager::efManager() : gfTask("EffectManager", Category_Effect, 0xc, 7, true) {
    unk138 = 0;
    unk40.m_effectSystem = EffectSystem::GetInstance();
    void* heap = gfHeapManager::alloc(Heaps::Effect, 0x456e0);
    unk12C = heap;
    MemoryManager* memoryManager = new (Heaps::System) MemoryManager(heap, 0x456e0, 0x80, 0x80, 0x100, 0x300);
    unk40.m_effectSystem->SetMemoryManager(memoryManager, 1);
    unk40.m_memoryManager = memoryManager;
    u8* buf = static_cast<u8*>(gfHeapManager::alloc(Heaps::Effect, 0x6000));
    unk130 = buf;
    gfMemFill(buf, 0, 0x6000);
    unk40.unk6C = buf;
    unk40.unk70 = buf + 0x3000;
    buf = static_cast<u8*>(gfHeapManager::alloc(Heaps::Effect, 0x2800));
    unk134 = buf;
    gfMemFill(buf, 0, 0x2800);
    unk40.unk64 = buf;
    unk40.unk68 = buf + 0x1400;
    unk40.unk74 = buf;
    unk40.unk78 = buf + 0x1400;
    unk40.unk7C = unk40.unk6C;
    unk40.unk80 = unk40.unk70;
    g_gfApplication->unk118.add(&lbl_805A0108);
    efScreen::create();
    efParticle::setSlowParam(unk40.unk74, unk40.unk78, unk40.unk7C, unk40.unk80);
}

efManager::~efManager() {
    efScreen::destroy();
    g_gfApplication->unk118.remove(&lbl_805A0108);
    gfHeapManager::free(unk12C);
    gfHeapManager::free(unk130);
    gfHeapManager::free(unk134);
    unk12C = NULL;
    unk130 = NULL;
    unk134 = NULL;
    lbl_805A0110 = NULL;
}

void* MemoryManager::AllocHeap(u32 size) {
    u32 headerSize = nw4r::ut::RoundUp(sizeof(MemInfo), 32);
    size = nw4r::ut::RoundUp(size, 32);

    for (MemInfo* pIt = mFreeMem; pIt != NULL; pIt = pIt->next) {
        if (pIt->size < size) {
            continue;
        }

        if (pIt->size < size + headerSize + 32) {
            if (pIt->prev != NULL) {
                pIt->prev->next = pIt->next;
            } else {
                mFreeMem = pIt->next;
            }

            if (pIt->next != NULL) {
                pIt->next->prev = pIt->prev;
            }

            if (mActiveMem != NULL) {
                mActiveMem->prev = pIt;
            }

            pIt->next = mActiveMem;
            pIt->prev = NULL;
            pIt->active = true;
            mActiveMem = pIt;
        } else {
            MemInfo* pNewFree = reinterpret_cast<MemInfo*>(
                (reinterpret_cast<u8*>(pIt) + headerSize + size));

            pNewFree->prev = pIt->prev;
            pNewFree->next = pIt->next;
            pNewFree->size = pIt->size - size - headerSize;
            pNewFree->active = false;
            pNewFree->chainNext = pIt->chainNext;

            if (pIt->chainNext != NULL) {
                pIt->chainNext->chainPrev = pNewFree;
            }

            pNewFree->chainPrev = pIt;
            pIt->chainNext = pNewFree;

            if (pIt->prev != NULL) {
                pIt->prev->next = pNewFree;
            } else {
                mFreeMem = pNewFree;
            }

            if (pIt->next != NULL) {
                pNewFree->next->prev = pNewFree;
            }

            if (mActiveMem != NULL) {
                mActiveMem->prev = pIt;
            }

            pIt->next = mActiveMem;
            pIt->prev = NULL;
            pIt->size = size;
            pIt->active = true;
            mActiveMem = pIt;
        }

        return reinterpret_cast<u8*>(pIt) + headerSize;
    }

    return NULL;
}

void efManager::processBegin() {
    unk40.m_isJustGameFrame = false;
    if (g_GameGlobal->isJustGameFrame()) {
        unk40.m_isJustGameFrame = true;
    }
    lbl_805A0100 = 1.0f / g_GameGlobal->getSlowRate();
    unk40.m_gameFrame = g_GameGlobal->getGameFrame();
    unk40.m_gameFrameDelta = g_GameGlobal->getGameFrameDelta();
    if (0.0f == unk40.m_gameFrame) {
        unk40.unk40_b6 = true;
    }
    unk40.unk40_b4 = false;
    if (unk40.unk40_b6) {
        unk40.unk40_b4 = true;
        unk40.unk54 = -1.0f;
        unk40.unk40_b5 = false;
        unk40.unk48 = unk40.m_gameFrame;
        unk40.unk40_b6 = false;
    } else if (unk40.m_isJustGameFrame) {
        if ((u32)unk40.m_gameFrame > unk40.unk48) {
            unk40.unk40_b4 = true;
            unk40.unk48 = unk40.m_gameFrame;
        }
        unk40.unk40_b5 = false;
    } else if (unk40.unk48 != (u32)(1.0f + unk40.m_gameFrame)) {
        unk40.unk40_b4 = true;
        unk40.unk48++;
    }
}

void efManager::renderPre() { }

void efManager::processDebug() {
    EffectSystem::GetInstance();
}

void efManager::addEffect(efEffect* effect) {
    m_effectLists[effect->m_group].PushBack(effect);
}

void efManager::removeEffect(efEffect* effect) {
    m_effectLists[effect->m_group].Erase(effect);
}

void MemoryManager::FreeHeap(void* pPtr) {
    u32 headerSize = nw4r::ut::RoundUp(sizeof(MemInfo), 32);

    MemInfo* pInfo =
        reinterpret_cast<MemInfo*>(static_cast<u8*>(pPtr) - headerSize);

    if (pInfo->prev != NULL) {
        pInfo->prev->next = pInfo->next;
    } else {
        mActiveMem = pInfo->next;
    }

    if (pInfo->next != NULL) {
        pInfo->next->prev = pInfo->prev;
    }

    MemInfo* pPrev = NULL;
    if (pInfo->chainPrev != NULL && !pInfo->chainPrev->active) {
        pPrev = pInfo->chainPrev;
    }

    MemInfo* pNext = NULL;
    if (pInfo->chainNext != NULL && !pInfo->chainNext->active) {
        pNext = pInfo->chainNext;
    }

    if (pPrev != NULL) {
        if (pNext != NULL) {
            if (pNext->prev != NULL) {
                pNext->prev->next = pNext->next;
            } else {
                mFreeMem = pNext->next;
            }

            if (pNext->next != NULL) {
                pNext->next->prev = pNext->prev;
            }

            pPrev->size +=
                pNext->size + pInfo->size + headerSize + headerSize;

            pPrev->chainNext = pNext->chainNext;

            if (pNext->chainNext != NULL) {
                pNext->chainNext->chainPrev = pPrev;
            }
        } else {
            pPrev->size += pInfo->size + headerSize;
            pPrev->chainNext = pInfo->chainNext;

            if (pInfo->chainNext != NULL) {
                pInfo->chainNext->chainPrev = pPrev;
            }
        }
    } else if (pNext != NULL) {
        if (pNext->prev != NULL) {
            pNext->prev->next = pInfo;
        } else {
            mFreeMem = pInfo;
        }

        if (pNext->next != NULL) {
            pNext->next->prev = pInfo;
        }

        pInfo->prev = pNext->prev;
        pInfo->next = pNext->next;
        pInfo->size += pNext->size + headerSize;
        pInfo->active = false;
        pInfo->chainNext = pNext->chainNext;

        if (pNext->chainNext != NULL) {
            pNext->chainNext->chainPrev = pPrev;
        }
    } else {
        if (mFreeMem != NULL) {
            mFreeMem->prev = pInfo;
        }

        pInfo->next = mFreeMem;
        pInfo->prev = 0;
        pInfo->active = false;
        mFreeMem = pInfo;
    }
}

u32 MemoryManager::GetNumFreeParticle() const {
    if (mParticleOM == NULL) {
        return 0;
    }
    return mParticleOM->GetNumFreeObject();
}

u32 MemoryManager::GetNumActiveParticle() const {
    if (mParticleOM == NULL) {
        return 0;
    }
    return mParticleOM->GetNumActiveObject();
}

u32 MemoryManager::GetNumAllocParticle() const {
    if (mParticleOM == NULL) {
        return 0;
    }
    return mParticleOM->GetNumAllocObject();
}

void MemoryManager::FreeParticle(void* pObject) {
    if (mParticleOM != NULL) {
        mParticleOM->Free(pObject);
    }
}

Particle* MemoryManager::AllocParticle() {
    if (mParticleOM == NULL) {
        return NULL;
    }
    return mParticleOM->Alloc();
}

u32 MemoryManager::GetNumFreeParticleManager() const {
    if (mParticleManagerOM == NULL) {
        return 0;
    }
    return mParticleManagerOM->GetNumFreeObject();
}

u32 MemoryManager::GetNumActiveParticleManager() const {
    if (mParticleManagerOM == NULL) {
        return 0;
    }
    return mParticleManagerOM->GetNumActiveObject();
}

u32 MemoryManager::GetNumAllocParticleManager() const {
    if (mParticleManagerOM == NULL) {
        return 0;
    }
    return mParticleManagerOM->GetNumAllocObject();
}

void MemoryManager::FreeParticleManager(void* pObject) {
    if (mParticleManagerOM != NULL) {
        mParticleManagerOM->Free(pObject);
    }
}

ParticleManager* MemoryManager::AllocParticleManager() {
    if (mParticleManagerOM == NULL) {
        return NULL;
    }
    return mParticleManagerOM->Alloc();
}

u32 MemoryManager::GetNumFreeEmitter() const {
    if (mEmitterOM == NULL) {
        return 0;
    }
    return mEmitterOM->GetNumFreeObject();
}

u32 MemoryManager::GetNumActiveEmitter() const {
    if (mEmitterOM == NULL) {
        return 0;
    }
    return mEmitterOM->GetNumActiveObject();
}

u32 MemoryManager::GetNumAllocEmitter() const {
    if (mEmitterOM == NULL) {
        return 0;
    }
    return mEmitterOM->GetNumAllocObject();
}

void MemoryManager::FreeEmitter(void* pObject) {
    if (mEmitterOM != NULL) {
        mEmitterOM->Free(pObject);
    }
}

Emitter* MemoryManager::AllocEmitter() {
    if (mEmitterOM == NULL) {
        return NULL;
    }
    return mEmitterOM->Alloc();
}

u32 MemoryManager::GetNumFreeEffect() const {
    if (mEffectOM == NULL) {
        return 0;
    }
    return mEffectOM->GetNumFreeObject();
}

u32 MemoryManager::GetNumActiveEffect() const {
    if (mEffectOM == NULL) {
        return 0;
    }
    return mEffectOM->GetNumActiveObject();
}

u32 MemoryManager::GetNumAllocEffect() const {
    if (mEffectOM == NULL) {
        return 0;
    }
    return mEffectOM->GetNumAllocObject();
}

void MemoryManager::FreeEffect(void* pObject) {
    if (mEffectOM != NULL) {
        mEffectOM->Free(pObject);
    }
}

Effect* MemoryManager::AllocEffect() {
    if (mEffectOM == NULL) {
        return NULL;
    }
    return mEffectOM->Alloc();
}

MemoryManager::~MemoryManager() { }

// TODO: clean up, give variables real names
namespace jj_8005B6F0 {

extern s32 g_efScreen;

extern "C" {
void* fn_801647B4();
void fn_80164C0C(void* obj, f32* vec, s32* blk, s32 r6, f32 f1, f32 f2);
u32 __cvt_fp2unsigned(f32);
void fn_8005D7D8(s32 screen, f32 f);
}

class Class_8042EC38;
struct Class_8042EC38_base {
    Class_8042EC38* next;
    char pad04[0x44];
};

class Class_8042EC38 : public Class_8042EC38_base {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void fn_8005A898();
};

struct Node {
    Node* next;
    Node* prev;
};

struct ListEntry {
    s32 count;
    Node head;
};

struct Arg {
    char pad0[0x40];
    u8 flag;
    char pad41[0xB];
    f32 f4c;
    f32 f50;
    f32 f54;
    char pad58[0xB0];
    ListEntry lists[3];
};

struct BlkData {
    s32 w[12];
};

struct Blk {
    s32 w[12];
    s32 pad[2];
};

#pragma auto_inline off

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8005B6F0(Arg* arg0) {
    Blk b;
    Node* sentinel;
    void* obj;
    Node* node = (Node*)gfCameraManager::getManager();
    char* mgr = (char*)node;
    u32 k;
    ListEntry* list;
    f32 vec[3];

    obj = fn_801647B4();

    *(BlkData*)&b = *(BlkData*)mgr;
    vec[0] = *(f32*)(mgr + 0x6C);
    vec[1] = *(f32*)(mgr + 0x70);
    vec[2] = *(f32*)(mgr + 0x74);
    f32 f2 = *(f32*)(mgr + 0xE0);
    f32 f1 = *(f32*)(mgr + 0xDC);
    fn_80164C0C(obj, vec, b.w, b.w[10], f1, f2);

    list = arg0->lists;
    for (k = 0; k < 3; k++) {
        node = list->head.next;
        sentinel = &list->head;
        while (node != sentinel) {
            ((Class_8042EC38*)node)->fn_8005A898();
            node = node->next;
        }
        list++;
    }

    if (!((arg0->flag >> 7) & 1)) {
        b.pad[1] = __cvt_fp2unsigned(arg0->f4c);
        b.pad[0] = 0x43300000;
    }

    arg0->f54 = arg0->f4c;
    fn_8005D7D8(g_efScreen, arg0->f50);
}
}

// TODO: clean up, give variables real names
namespace jj_8005B838 {
struct Mtx34 {
    unsigned int w[12];
};

class Class_8042EC38;

struct Class_8042EC38_base {
    Class_8042EC38* next;
    char pad0[0x0D - 0x04];
    unsigned char flags;
    char pad1[0x48 - 0x0E];
};

class Class_8042EC38 : public Class_8042EC38_base {
public:
    virtual void fn_8005C798();
    virtual void fn_8005CAA0();
    virtual void fn_8005CA18();
    virtual void fn_8005A898();
    virtual void fn_8005CE70();
};

struct CamData {
    char pad0[0xA8];
    int flags;
    float ac;
    float b0;
    float b4;
    char pad1[4];
    float bc;
    float c0;
    float c4;
    float c8;
};

struct Self {
    char pad0[0x84];
    Mtx34 m84;
    Mtx34 mB4;
    char pad1[0xF0 - 0xE4];
    unsigned int unkF0;
    char pad2[0x138 - 0xF4];
    unsigned int unk138;
};

struct Sub {
    char pad0[0x108];
    unsigned int unk108;
};

extern "C" {
void GXSetFog(int, unsigned char*, float, float, float, float);
void gfDrawResetLight__Fv();
void* fn_801AC554(void*, int);
void fn_801A7DBC(void**, void*);
void fn_801A8048(void**);
void fn_801A80CC(void**);
void fn_801A8194(void**);
void fn_801ECBF0(void*, float, float, float, float, float, float);
void fn_801ECCE8(void*, float, float, float, float, float, float, float, float);
void fn_801F51DC(void*, int);
void fn_801F5258(void*, int);
void* fn_801647B4();
void fn_8005F668(void*);
void setIdentity__6MatrixFv(void*);
}

extern float lbl_805A18EC;
extern float lbl_805A1900;
extern float lbl_805A1904;
extern float lbl_805A1908;
extern void* g_gfSceneRoot;
extern void* g_ecMgr;

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8005B838(Self* self, int arg1) {
    Mtx34 spA0;
    Mtx34 sp70;
    Mtx34 sp40;
    Mtx34 sp10;
    void* spC;
    unsigned int sp8;
    Sub* p;
    int mode;
    void* root;
    void* r3;
    CamData* cam;
    Class_8042EC38* node;
    Class_8042EC38* sentinel;
    unsigned int flag27;
    int var0;
    unsigned char* mgr;

    sp8 = 0;
    GXSetFog(0, (unsigned char*)&sp8, lbl_805A18EC, lbl_805A1900, lbl_805A18EC, lbl_805A1900);
    p = (Sub*)((char*)self + arg1 * 12);
    if (p->unk108 != 0) {
        gfDrawResetLight__Fv();
        mode = 0;
        if (arg1 > 1) {
            mode = 1;
        }
        root = g_gfSceneRoot;
        if (mode == 0) {
            r3 = *(void**)((char*)root + 0x40);
        } else if (mode == 1) {
            r3 = *(void**)((char*)root + 0x44);
        } else {
            r3 = 0;
        }
        spC = fn_801AC554(r3, 0);
        fn_801A7DBC(&spC, &spA0);
        self->m84 = spA0;
        cam = (CamData*)spC;
        if (cam->flags == 0) {
            fn_801ECBF0(&sp40, cam->ac, cam->b0, lbl_805A1904, lbl_805A1908, lbl_805A1904, lbl_805A1904);
            self->mB4 = sp40;
        } else {
            float c8 = cam->c8;
            float c4 = cam->c4;
            float c0 = cam->c0;
            float bc = cam->bc;
            fn_801ECCE8(&sp10, bc, c0, c4, c8, lbl_805A1904, lbl_805A1908, lbl_805A1904, lbl_805A1904);
            self->mB4 = sp10;
        }
        setIdentity__6MatrixFv(&sp70);
        fn_801F51DC(&sp70, 0);
        fn_801F5258(&sp70, 0);
        self->unkF0 = 0;
        fn_801A8194(&spC);
        fn_801A8048(&spC);
        fn_801A80CC(&spC);
        flag27 = 0;
        var0 = 0;
        if (mode == 0) {
            var0 = self->unk138;
        }
        if (var0 != 0) {
            mgr = (unsigned char*)fn_801647B4();
            mgr[0x5064] = 1;
            flag27 = 1;
        }
        sentinel = (Class_8042EC38*)((char*)p + 0x10C);
        for (node = sentinel->next; node != sentinel; node = node->next) {
            if ((node->flags >> 6) & 1) {
                node->fn_8005CE70();
            }
        }
        if (flag27 == 1) {
            mgr = (unsigned char*)fn_801647B4();
            mgr[0x5064] = 0;
        }
    } else if (arg1 == 1 && self->unk138 != 0) {
        self->unk138 = 0;
    }
    if (arg1 == 1) {
        fn_8005F668(g_ecMgr);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_8005C5B0 {
extern "C" void __dl__FPv(void*);

extern "C" void* fn_8005C5B0(void* arg0, int arg1) {
    if (arg0 != 0 && arg1 > 0) {
        __dl__FPv(arg0);
    }
    return arg0;
}
}

namespace jj_8005C5F0 {
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void* fn_8005AD3C(void*);
extern void* lbl_805A0110;

// TODO: clean up, give variables real names
extern "C" void fn_8005C5F0() {
    if (lbl_805A0110 == 0) {
        void* p = __nw__FUlQ25Heaps8HeapType(0x13C, 2);
        if (p != 0) {
            p = fn_8005AD3C(p);
        }
        lbl_805A0110 = p;
    }
}
}

