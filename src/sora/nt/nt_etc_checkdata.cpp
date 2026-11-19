#include <revolution/OS/OSTime.h>
#include <nt/nt_offline.h>
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

// TODO: clean up, give variables real names
extern "C" void fn_801581FC(void* self) {
    fn_80157BC4(reinterpret_cast<u8*>(self) - 0x344);
}

extern "C" void fn_80157A78(void*);

// TODO: clean up, give variables real names
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

// TODO: clean up, give variables real names
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

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
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

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
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

// TODO: clean up, give variables real names
namespace jj_8015741C {

extern "C" {
int fn_800305B0(int);
int fn_8014CB40(ntFriendInfo*);
int fn_8014CB4C(ntFriendInfo*, int);
void fn_8014CB7C(ntFriendInfo*);
void fn_8014FEFC();
int fn_801515B4(void*, void*, const char*, const char*);
int fn_801516D4(void*, void*);
int fn_801517D0(void*, void*, const char*, const char*, const char*);
}

extern unsigned char lbl_80423690[];
extern char lbl_804620B0[];
extern char lbl_8059E818[5];
extern char lbl_8059E820[8];
extern char lbl_8059E828[1];
extern char lbl_8059E82C[6];

#define TICKS ((*(u32*)0x800000F8 / 4) / 1000)
#define EXPIRED (!(self->timeout == 0 ? true : OSGetTime() <= start + self->timeout * TICKS))

struct Context {
    char pad0[0x190];
    int count;
};

struct Handler {
    char pad0[0x34d];
    unsigned char ready;
};

struct Task {
    char pad0[0x344];
    char base[4];
    unsigned char finished;
    char pad349[3];
    int result;
    char pad350[4];
    int mode;
    char pad358[4];
    unsigned char done;
    char pad35d[3];
    unsigned int timeout;
    char pad364[12];
    Context* context;
    unsigned char busy;
    char pad375[3];
    Handler* handler;
    unsigned char cancel;
    unsigned char setting;
    unsigned char deliver;
};

static inline void* taskBase(Task* self) {
    void* result = self;
    if (self)
        result = self->base;
    return result;
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8015741C(Task* self) {
    if (self->cancel)
        return;

    self->result = 1;
    self->cancel = 0;
    s64 start = OSGetTime();

    if (self->deliver == 1) {
        if (fn_800305B0(3)) {
            unsigned char* global = lbl_80423690;
            if (!global[0])
                self->context->count++;

            while (!global[0]) {
                if (EXPIRED)
                    goto finish;
                OSSleepTicks(TICKS);
            }

            if ((*(u32*)(global + 12) << 17) < 0x4b000U)
                self->deliver = 0;
        } else {
            self->deliver = 0;
        }
    }

    int value = fn_8014CB40(ntFriendInfo::getInstance());
    if (!fn_8014CB4C(ntFriendInfo::getInstance(), value)) {
        fn_8014CB7C(ntFriendInfo::getInstance());
        fn_8014CB40(ntFriendInfo::getInstance());
    }

    if (!fn_801515B4(self->handler, taskBase(self),
                     lbl_8059E818, lbl_804620B0))
        goto finish;

    self->busy = 0;
    while (!self->handler->ready) {
        if (self->cancel)
            goto finish;
        if (EXPIRED)
            goto finish;
        OSSleepTicks(TICKS);
    }

    if (self->setting) {
        self->mode = 1;
        self->done = 0;
        if (!fn_801517D0(self->handler, taskBase(self),
                         lbl_8059E820, lbl_8059E828, lbl_8059E828))
            goto disconnect;

        while (!self->done) {
            if (self->cancel)
                goto disconnect;
            if (EXPIRED)
                goto disconnect;
            OSSleepTicks(TICKS);
        }
    }

    fn_8014FEFC();
    if (self->deliver) {
        self->mode = 2;
        self->done = 0;
        if (!fn_801517D0(self->handler, taskBase(self),
                         lbl_8059E82C, lbl_8059E828, lbl_8059E828))
            goto disconnect;

        while (!self->done) {
            if (self->cancel)
                goto disconnect;
            if (EXPIRED)
                goto disconnect;
            OSSleepTicks(TICKS);
        }
    }

    self->result = 0;

disconnect:
    if (fn_801516D4(self->handler, taskBase(self))) {
        while (self->handler->ready)
            OSSleepTicks(TICKS);
    }

finish:
    self->finished = 1;
    self->busy = 1;
}
}

// TODO: clean up, give variables real names
namespace jj_80157E8C {
extern "C" {
int getSections_Direct__7nteFileFPCUc(int arg0);
void fn_80153488(void* arg0);
void* getSectionData_Direct__7nteFileFPUcUl(int arg0, u8 arg1);
void fn_80153498(void* arg0, int arg1);
int fn_80153504(void* arg0, int arg1);
void* fn_80153818(void* arg0, int arg1);
void fn_80152FA4(void);
u64 getTime__9nteGlobalFPb(int arg0);
void OSTicksToCalendarTime(u64 arg0, void* arg1);
void setTimeStampFileSetting__9nteGlobalFUsUcUc(u16 a, u8 b, u8 c);
void setLastDownload__9nteGlobalFUsUcUc(u16 a, u8 b, u8 c);
}

struct ObjBase {
    int type;
    int pad4;
};
class Obj : public ObjBase {
public:
    virtual void __dt__18nteFileDataSettingFv(int arg1);
    virtual void fn_1();
    virtual void fn_2();
};

extern "C" int _reflectSetting__12nteCheckDataFPvUl(void* self, int arg1) {
    int entry[2];
    int timeData[8];

    if ((u8)getSections_Direct__7nteFileFPCUc(arg1) != 1) {
        return 0;
    }

    fn_80153488(entry);
    if (fn_80153504(entry, (int)getSectionData_Direct__7nteFileFPUcUl(arg1, 0)) == 0) {
        fn_80153498(entry, -1);
        return 0;
    }

    Obj* obj = (Obj*)fn_80153818(entry, *(int*)((char*)self + 0x350));
    if (obj == 0) {
        fn_80153498(entry, -1);
        return 0;
    }

    if (obj->type == 0) {
        fn_80152FA4();
        OSTicksToCalendarTime(getTime__9nteGlobalFPb(0), timeData);
        setTimeStampFileSetting__9nteGlobalFUsUcUc((u16)timeData[5], (u8)(timeData[4] + 1), (u8)timeData[3]);
        setLastDownload__9nteGlobalFUsUcUc((u16)timeData[5], (u8)(timeData[4] + 1), (u8)timeData[3]);
    }

    if (obj != 0) {
        obj->__dt__18nteFileDataSettingFv(1);
    }

    fn_80153498(entry, -1);
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_80157FD4 {


extern "C" {
int requestSaveVF__14gfCollectionIOFiPCcPvib(void*, int, int, int, void*, int);
int getDeliveryFileName__14gfCollectionIOFi(int);
void setLastDownload__9nteGlobalFUsUcUc(u16, u8, u8);
void setTimeStampFileDeliv__9nteGlobalFx(void);
void setEnableGotDeliveryData__9nteGlobalFUc(int);
u64 getTime__9nteGlobalFPb(int);
void fn_80153488(int*);
void fn_80153498(int*, int);
int fn_80153504(int*, int);
int fn_80153810(int*);
int getDataType__10nteSectionFv(int*);
char* getSize__10nteSectionFv(int*);
u8 getSections_Direct__7nteFileFPCUc(int);
void* getSectionData_Direct__7nteFileFPUcUl(int, u8);
void OSTicksToCalendarTime(u64, void*);
}

struct Fn80157Target {
    u8 pad5c[0x5c];
    u8 busy;
    u8 pad5d[3];
    int result;
};

struct Fn80157Object {
    u8 pad[0x370];
    Fn80157Target* target;
};

extern "C" int _storeDelivData__12nteCheckDataFPvUl(Fn80157Object* self, int arg1, int arg2) {
    int entry[2];
    int timeData[8];
    u32 success;
    u8 count;
    int index;
    char* value;
    u8 i;
    int type;

    if (self->target == 0) {
        return 0;
    }

    success = 0;
    count = getSections_Direct__7nteFileFPCUc(arg1);
    i = 0;

    while ((u8)i < count) {
        fn_80153488(entry);
        if (fn_80153504(entry, (int)getSectionData_Direct__7nteFileFPUcUl(arg1, (u8)i)) == 0) {
            fn_80153498(entry, -1);
        } else if (getSize__10nteSectionFv(entry) == 0) {
            fn_80153498(entry, -1);
        } else {
            type = 0;
            switch (getDataType__10nteSectionFv(entry)) {
            case 1:
                type = 1;
                break;
            case 2:
                type = 2;
                break;
            case 3:
                type = 3;
                break;
            }

            if (type != 0) {
                value = getSize__10nteSectionFv(entry);
                index = fn_80153810(entry);

                if (requestSaveVF__14gfCollectionIOFiPCcPvib(self->target, type, getDeliveryFileName__14gfCollectionIOFi(type),
                                index, value, 1) != 0) {
                    while (self->target->busy == 0) {
                        OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
                    }
                    if (self->target->result == 0) {
                        success = 1;
                    }
                }
            }

            if (type == 1) {
                setEnableGotDeliveryData__9nteGlobalFUc(2);
            }
            if (type == 2) {
                setEnableGotDeliveryData__9nteGlobalFUc(1);
            }
            if (type == 3) {
                setEnableGotDeliveryData__9nteGlobalFUc(4);
            }
            fn_80153498(entry, -1);
        }

        i++;
    }

    if (success == 1) {
        OSGetTime();
        setTimeStampFileDeliv__9nteGlobalFx();
        OSTicksToCalendarTime(getTime__9nteGlobalFPb(0), timeData);
        setLastDownload__9nteGlobalFUsUcUc((u16)timeData[5], (u8)(timeData[4] + 1),
                    (u8)timeData[3]);
    }

    return success;
}
}

