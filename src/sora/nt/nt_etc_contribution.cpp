#include <nt/nt_etc_contribution.h>
#include <nt/nt_etc_global.h>
#include <nt/nt_etc_sbs.h>
#include <revolution/OS/OSTime.h>
#include <types.h>

bool nteContribution::upload(s32 kind, void* data, u32 size) {
    bool terminated = OSIsThreadTerminated(&m_thread) == 1;
    if (terminated) {
        return false;
    }
    if (unk354 != 100) {
        return false;
    }
    if (!data || !size) {
        return false;
    }
    s32 job = -1;
    if (kind == 0) {
        job = 2;
    } else if (kind == 1) {
        job = 1;
    } else if (kind == 2) {
        job = 3;
    }
    if (job == -1) {
        return false;
    }
    unk354 = job;
    unk358 = data;
    unk35C = size;
    OSResumeThread(&m_thread);
    return true;
}

void nteContribution::notifySBSEndProc(s32 notification, s32 result) {
    unk380 = unk368->getErrorCode(result);
    if (unk380) {
        unk384 = true;
        unk36C = true;
    }
}

bool nteContribution::isAbleToContribute() {
    bool valid;
    bool able = false;
    s64 now = nteGlobal::getTime(&valid);
    if (valid) {
        s64 start = nteGlobal::getContribute_Start();
        s64 end = nteGlobal::getContribute_End();
        s64 last = nteGlobal::getLastContributed();
        if (start && end && now >= start && now <= end) {
            if (nteGlobal::isInfinityContribute()) {
                able = true;
            } else {
                OSCalendarTime currentDate;
                OSCalendarTime lastDate;
                OSTicksToCalendarTime(now, &currentDate);
                OSTicksToCalendarTime(last, &lastDate);
                if (currentDate.year != lastDate.year ||
                    currentDate.month != lastDate.month ||
                    currentDate.mday != lastDate.mday) {
                    able = true;
                }
            }
        }
    }
    return able;
}

// TODO: clean up, give variables real names
extern "C" void fn_8014F6E4(void);

extern "C" void fn_80154DF8(void) {
    fn_8014F6E4();
}

struct Fn154C4CObj;
extern "C" void fn_80154C4C(Fn154C4CObj* self, void* p);
extern "C" void fn_80154C60(void* self, int value, int idx);

extern "C" void fn_80154DFC(void* self, int value, int idx) {
    fn_80154C60(reinterpret_cast<u8*>(self) - 0x340, value, idx);
}


extern "C" void fn_80154E04(void* self, void* p) {
    fn_80154C4C((Fn154C4CObj*)(reinterpret_cast<u8*>(self) - 0x344), p);
}

extern "C" void fn_80154C8C(void*);

extern "C" void fn_80154E0C(void* self) {
    fn_80154C8C(reinterpret_cast<u8*>(self) - 0x348);
}

#pragma dont_inline on
struct Fn154C4CObj {
    unsigned char pad0[0x36c];
    unsigned char a;
    unsigned char pad1[7];
    unsigned char b;
    unsigned char pad2[3];
    void* ptr;
};

extern "C" void fn_80154C4C(Fn154C4CObj* self, void* p) {
    self->ptr = p;
    self->b = 1;
    self->a = 1;
}

extern "C" void fn_80154C60(void* self, int value, int idx) {
    *(int*)((char*)self + 0x378) = value;
    *(unsigned char*)((char*)self + 0x37C) = 1;
    if (idx == 6) {
        *(unsigned char*)((char*)self + 0x36C) = 1;
        return;
    }
    if (idx == 7) {
        *(unsigned char*)((char*)self + 0x36C) = 1;
    }
}
#pragma dont_inline reset

// TODO: clean up, give variables real names
namespace jj_80154420 {
extern "C" void OSResumeThread(void*);
extern "C" void exit__6gfTaskFv(void*);
extern "C" void* __dt__8gfThreadFv(void*, short);
extern "C" void __dl__FPv(void*);
extern char lbl_804616D8[];

extern "C" void* fn_80154420(void* p, int flag)
{
    if (p != 0) {
        char* t = (char*)p;
        *(char**)(t + 0x000) = lbl_804616D8;
        *(char**)(t + 0x340) = lbl_804616D8 + 0xC;
        *(char**)(t + 0x344) = lbl_804616D8 + 0x18;
        *(char**)(t + 0x348) = lbl_804616D8 + 0x24;
        if (*(unsigned char*)(t + 0x34C) == 0) {
            *(signed char*)(t + 0x36C) = 1;
            OSResumeThread(t + 8);
        }
        exit__6gfTaskFv(*(void**)(t + 0x364));
        exit__6gfTaskFv(*(void**)(t + 0x368));
        __dt__8gfThreadFv(p, 0);
        if (flag > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_801542C4 {
extern "C" {
extern char __vt__8gfThread[];
extern char lbl_804616D8[];

void createThread__8gfThreadFP10gfRunnableUlUlQ25Heaps8HeapType(
    void*, void*, unsigned long, unsigned long, int);
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void* __ct__6nteDWCFv(void*);
void* fn_80151B38(void*, int);
}

struct Dwc {
    char pad0[0x60];
    void* cb60;
    void* cb64;
};

struct Worker {
    char pad0[0x4C];
    void* cb4C;
    void* cb50;
    char pad54[0x24];
};

struct Thread {
    void* vtable;
    char pad4[0x33C];
    void* cb340;
    void* cb344;
    void* cb348;
    char f34C;
    char pad34D[3];
    int f350;
    int f354;
    int f358;
    int f35C;
    int heap;
    Dwc* dwc;
    Worker* worker;
    char f36C;
    char pad36D[3];
    int f370;
    char f374;
    char pad375[3];
    int f378;
    char f37C;
    char pad37D[3];
    int f380;
    char f384;
    char pad385[3];
    int f388;
};

extern "C" Thread* fn_801542C4(Thread* self, int heap)
{
    self->vtable = __vt__8gfThread;
    createThread__8gfThreadFP10gfRunnableUlUlQ25Heaps8HeapType(
        self, self, 0x10, 0x2000, 0x3A);

    self->vtable = lbl_804616D8;
    self->cb340 = lbl_804616D8 + 0xC;
    self->cb344 = lbl_804616D8 + 0x18;
    self->cb348 = lbl_804616D8 + 0x24;
    self->f34C = 0;
    self->f350 = 0;
    self->f354 = 100;
    self->f358 = 0;
    self->f35C = 0;

    void* obj = __nw__FUlQ25Heaps8HeapType(0x3C8, heap);
    if (obj)
        obj = __ct__6nteDWCFv(obj);
    self->dwc = (Dwc*)obj;

    void* a = self;
    if (self)
        a = &self->cb344;
    self->dwc->cb60 = a;

    void* b = self;
    if (self)
        b = &self->cb340;
    self->dwc->cb64 = b;

    obj = __nw__FUlQ25Heaps8HeapType(0x78, heap);
    if (obj)
        obj = fn_80151B38(obj, 0);
    self->worker = (Worker*)obj;

    void* c = self;
    if (self)
        c = &self->cb340;
    self->worker->cb50 = c;

    void* d = self;
    if (self)
        d = &self->cb348;
    self->worker->cb4C = d;

    self->heap = heap;
    self->f36C = 0;
    self->f370 = 0;
    self->f374 = 0;
    self->f378 = 0;
    self->f37C = 0;
    self->f380 = 0;
    self->f384 = 0;
    self->f388 = 0;
    return self;
}
}

