#include <gf/gf_thread.h>
#include <gf/gf_task.h>
#include <revolution/OS/OSThread.h>
#include <gf/gf_crypto_aes.h>
#include <gf/gf_heap_manager.h>
#include <nt/nt_etc_checkdata.h>
#include <revolution/OS/OSHardware.h>
#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0458[0x8] __attribute__((aligned(4))) = { 0 };

void nteCheckData::showError(s32 error, s32 kind) {
    if (kind == 6) {
        unk37C = true;
    } else if (kind == 7) {
        unk37C = true;
    }
}

void nteCheckData::notifyDoneNDProc(s32 proc, s32 result, u32 size) {
    switch (proc) {
    case 1:
        if (result != 0) {
            unk37C = true;
        }
        break;
    case 2:
        break;
    case 3:
        if (result == 0) {
            gfCryptoAES crypto;
            crypto.m_freeDst = true;
            if (crypto.decrypt(unk358, size, unk350)) {
                while (!crypto.m_isDone) {
                    OSSleepTicks(OS_MSEC_TO_TICKS(1));
                }
                if (crypto.m_result == 0) {
                    if (unk354 == 1) {
                        _reflectSetting(crypto.m_dst, crypto.m_dstSize);
                    } else if (unk354 == 2) {
                        _storeDelivData(crypto.m_dst, crypto.m_dstSize);
                    }
                }
            }
        }
        if (unk358) {
            gfHeapManager::free(unk358);
            unk358 = NULL;
        }
        unk35C = true;
        break;
    }
}

// TODO: clean up, give variables real names
extern "C" void fn_80157A50(void*);

extern "C" void fn_801581F4(void* self) {
    fn_80157A50(reinterpret_cast<u8*>(self) - 0x340);
}

extern "C" void fn_80157BC4(void*);

extern "C" void fn_801581FC(void* self) {
    fn_80157BC4(reinterpret_cast<u8*>(self) - 0x344);
}

extern "C" void fn_80157A78(void*);

extern "C" void fn_80158204(void* self) {
    fn_80157A78(reinterpret_cast<u8*>(self) - 0x344);
}

// TODO: clean up, give variables real names
namespace jj_80157200 {
namespace Heaps {
enum HeapType { HEAP_TYPE_DUMMY = 0 };
}

class gfRunnable;

extern "C" {
extern char __vt__8gfThread[];
extern char lbl_804620C4[];

void createThread__8gfThreadFP10gfRunnableUlUlQ25Heaps8HeapType(
    void*, void*, unsigned long, unsigned long, int);
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void* fn_801510F4(void*, s8);
}

struct gfThread {
    void* vtable;
    char pad4[0x340 - 0x4];
    void* unk340;
    void* unk344;
    s8 unk348;
    char pad349[3];
    s32 unk34C;
    s32 unk350;
    void* unk354;
    s32 unk358;
    char pad35C[4];
    s32 unk360;
    char pad364[0xC];
    s32 unk370;
    s8 unk374;
    char pad375[3];
    void* unk378;
    s8 unk37C;
};

extern "C" gfThread* fn_80157200(gfThread* self, s32 heap)
{
    self->vtable = __vt__8gfThread;
    createThread__8gfThreadFP10gfRunnableUlUlQ25Heaps8HeapType(
        self, self, 0xF, 0x2000, 0x3A);

    self->vtable = lbl_804620C4;
    self->unk340 = lbl_804620C4 + 0xC;
    self->unk344 = lbl_804620C4 + 0x18;
    self->unk370 = 0;
    self->unk374 = 1;
    self->unk348 = 0;
    self->unk34C = 0;
    self->unk360 = 0;
    self->unk354 = 0;
    self->unk358 = 0;

    void* obj = __nw__FUlQ25Heaps8HeapType(0x360, heap);
    if (obj)
        obj = fn_801510F4(obj, 1);
    self->unk378 = obj;

    void* cb = self;
    if (self)
        cb = &self->unk340;
    ((gfThread*)self->unk378)->unk354 = cb;

    self->unk350 = heap;
    self->unk37C = 0;
    return self;
}
}

namespace jj_801572DC {

extern "C" {
extern char lbl_804620C4[];
void* fn_80151180(void*, int);
void* __dt__8gfThreadFv(void*, short);
}

inline bool terminated(u8* thread) {
    return OSIsThreadTerminated((OSThread*)(thread + 8)) == 1;
}

extern "C" void* fn_801572DC(void* self, int flag) {
    u8* p = (u8*)self;
    if (p != 0) {
        *(char**)p = lbl_804620C4;
        *(char**)(p + 0x340) = lbl_804620C4 + 0xC;
        *(char**)(p + 0x344) = lbl_804620C4 + 0x18;
        if (!terminated(p)) {
            p[0x37C] = 1;
            OSResumeThread((OSThread*)(p + 8));
            while (!terminated(p)) {
                OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
            }
        }
        if (*(int*)(p + 0x378) != 0) {
            fn_80151180(*(void**)(p + 0x378), 1);
            *(int*)(p + 0x378) = 0;
        }
        if (*(void**)(p + 0x358) != 0) {
            gfHeapManager::free(*(void**)(p + 0x358));
            *(void**)(p + 0x358) = 0;
        }
        if (*(int*)(p + 0x370) != 0) {
            ((gfTask*)(*(u8**)(p + 0x370) + 4))->exit();
            *(int*)(p + 0x370) = 0;
        }
        __dt__8gfThreadFv(p, 0);
        if (flag > 0) {
            operator delete(p);
        }
    }
    return self;
}
}

namespace jj_801578C4 {
extern "C" {
int OSIsThreadTerminated(void* thread);
void OSResumeThread(void* thread);
void* fn_80150460(void);
u64 getTime__6nteDWCCFv(void* self);
void fn_8015051C(void);
s8 fn_800305B0(s32 arg);
u64 fn_8014FBB8(void);
void fn_8014FEFC(void);
u64 fn_801504AC(s32 arg);
void fn_801E1D80(u64 time, void* out);
void* fn_80038984(void* obj, s32 type);
void* __nw__FUlQ25Heaps8HeapType(u32 size, s32 type);
extern u8 lbl_805A0458;
}

struct Buf {
    u8 pad[0xC];
    s32 a;
    s32 b;
    s32 c;
    u8 pad2[0x10];
};

extern "C" s32 fn_801578C4(void* self) {
    s32 one;
    u8* p = (u8*)self;
    Buf b2;
    Buf b1;
    u64 t;
    void* obj;
    u64 t3;

    bool term = OSIsThreadTerminated(p + 8) == 1;
    if (term) {
        return 0;
    }
    if (*(s32*)(p + 0x354) != 0) {
        return 0;
    }
    t = getTime__6nteDWCCFv(fn_80150460());
    if (t == 0) {
        return 0;
    }
    fn_8015051C();
    one = 1;
    p[0x37D] = one;
    p[0x37E] = fn_800305B0(3);
    fn_801E1D80(fn_8014FBB8(), &b1);
    fn_801E1D80(fn_801504AC(0), &b2);
    if (lbl_805A0458 == 0 && b2.c == b1.c && b2.b == b1.b && b2.a == b1.a) {
        fn_8014FEFC();
        p[0x348] = one;
        p[0x374] = one;
        p[0x37C] = one;
        OSResumeThread(p + 8);
        return 1;
    }
    if (*(s32*)(p + 0x370) == 0) {
        obj = __nw__FUlQ25Heaps8HeapType(0x194, *(s32*)(p + 0x350));
        if (obj != 0) {
            obj = fn_80038984(obj, *(s32*)(p + 0x350));
        }
        *(void**)(p + 0x370) = obj;
        if (obj == 0) {
            fn_8014FEFC();
            p[0x348] = one;
            p[0x374] = one;
            p[0x37C] = one;
            OSResumeThread(p + 8);
            return 1;
        }
    }
    t3 = fn_8014FBB8();
    *(u32*)(p + 0x368) = (u32)(t3 >> 32);
    *(u32*)(p + 0x36C) = (u32)t3;
    p[0x374] = 0;
    *(s32*)(p + 0x354) = 1;
    OSResumeThread(p + 8);
    return 1;
}
}

