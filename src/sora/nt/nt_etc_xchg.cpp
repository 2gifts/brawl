#include <gf/gf_archive.h>
#include <types.h>
#include <nt/nt_etc_xchg.h>
#include <gf/gf_heap_manager.h>
#include <string.h>

// Function bodies below are taken from https://github.com/jj-link/brawl.

// TODO: clean up, give variables real names
extern "C" unsigned int fn_8014E708(unsigned int x) {
    return ((x + 0x1D45U) / 7474U) & 0xFFFFU;
}

void nteExchange::mail_AddSendTo(u64 recipient) {
    if (m_unk70 < 8) {
        m_unk30[m_unk70++] = recipient;
    }
}

void nteExchange::mail_ClearSendTo() {
    memset(m_unk30, 0, sizeof(m_unk30));
    m_unk70 = 0;
}

u16 nteExchange::getRemainSendPoint() {
    u32 size = _getMailSentSizeToday();
    if (size >= 0x32000) {
        return 0;
    }
    return (0x32000 - size) / 10240;
}

void nteExchange::run() {
    switch (m_job) {
    case 0:
        break;
    case 1:
        if (friend_makeList()) m_result = 0;
        else m_result = -1;
        break;
    case 2: {
        int result = _mail_SendMail();
        if (result == 0) m_result = 0;
        else if (result == -1) m_result = -1;
        else m_result = -4;
        break;
    }
    case 3:
        m_unk88 = gfHeapManager::alloc(m_heap, 0x23000, 0x20);
        if (m_unk88) {
            m_result = _mail_ReceiveMail(m_jobArg, m_unk80);
            gfHeapManager::free(m_unk88);
            m_unk88 = 0;
        } else {
            m_result = -1;
        }
        break;
    case 4:
        if (_openLibrary(1)) m_result = 0;
        else m_result = -1;
        break;
    }
    m_jobDone = true;
}

// TODO: clean up, give variables real names
extern "C" void fn_8014E6B4(void* self, u32 value) {
    *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x7c) = value;
}

extern "C" void fn_8014F0F0(void*);

// TODO: clean up, give variables real names
extern "C" void fn_8014F6CC(void* self) {
    fn_8014F0F0(reinterpret_cast<u8*>(self) - 0x18);
}

// TODO: clean up, give variables real names
namespace jj_8014E6A8 {
extern "C" void fn_8014E6A8(void* p1, unsigned int p2, unsigned int p3)
{
    *(unsigned int*)((char*)p1 + 0x74) = p2;
    *(unsigned int*)((char*)p1 + 0x78) = p3;
}
}

// TODO: clean up, give variables real names
namespace jj_8014F584 {
typedef unsigned char u8;

extern "C" {
int OSIsThreadTerminated(void*);
int OSResumeThread(void*);
void* __dt__8gfThreadFv(void*, short);
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void createThread__8gfThreadFP10gfRunnableUlUlQ25Heaps8HeapType(
    void*, void*, unsigned long, unsigned long, int);
extern "C" unsigned char __vt__8gfThread[];
void* fn_80038984(void*, int);
}

struct Base0 {
    int pad0;
    int heap;
    u8 pad8[0x0C];
};

struct RunBase {
    int a;
    int b;
};

struct Object : Base0, RunBase {
    u8* thread;
    void* obj20;
    int field24;
    int field28;
    u8 pad2C[0x60];
    u8 flag;
};

inline bool terminated(u8* thread) {
    return OSIsThreadTerminated(thread + 8) == 1;
}

// TODO: clean up, give variables real names
extern "C" int fn_8014F584(Object* self, int value) {
    if (self->thread != 0) {
        if (!terminated(self->thread))
            return 0;
        __dt__8gfThreadFv(self->thread, 1);
        self->thread = 0;
    }

    int heap = self->heap;
    if (self->obj20 == 0) {
        void* o = __nw__FUlQ25Heaps8HeapType(0x194, heap);
        if (o) {
            o = fn_80038984(o, heap);
        }
        self->obj20 = o;
        if (!o)
            return 0;
    }

    u8* thread = (u8*)__nw__FUlQ25Heaps8HeapType(0x340, heap);
    if (thread) {
        RunBase* r = self;
        *(void**)thread = __vt__8gfThread;
        createThread__8gfThreadFP10gfRunnableUlUlQ25Heaps8HeapType(
            thread, r, 0x10, 0x2000, heap);
    }
    self->thread = thread;
    if (!thread)
        return 0;

    self->field24 = 3;
    self->field28 = value;
    self->flag = 0;
    OSResumeThread(thread + 8);
    return 1;
}
}

namespace jj_8014F320 {

extern "C" char lbl_80461030[];
extern "C" void fn_80014ED4(gfArchive*, const char*, s32, s32);
extern "C" u16 fn_8014E1EC(void);
extern "C" u16* getData__9gfArchiveF11ARCNodeTypeiPiUs(
    gfArchive*, s32, u16, u32*, u16);

static inline s32 findLine(u16* data, u32 size, s32 remaining, u32& idx) {
    idx = 0;
    while (idx < size && remaining > 0) {
        if (data[idx++] == 0) {
            remaining--;
            if (remaining == 0) {
                break;
            }
        }
    }
    return remaining;
}

// TODO: clean up, give variables real names
extern "C" u32 fn_8014F320(s32* arg0, s32 arg1, u16* arg2, u32 arg3) {
    u32 idx;
    u16* data;
    u32 count;
    u32 size;
    s32 remaining;

    memset(arg2, 0, arg3 * 2);
    count = 0;
    gfArchive archive;
    fn_80014ED4(&archive, lbl_80461030, arg0[1], 0);

    data = getData__9gfArchiveF11ARCNodeTypeiPiUs(
        &archive, 1, fn_8014E1EC(), &size, 0xFFFE);
    if (data == 0) {
        data = getData__9gfArchiveF11ARCNodeTypeiPiUs(
            &archive, 1, 1, &size, 0xFFFE);
    }

    if (data != 0) {
        remaining = findLine(data, size, arg1 - 1, idx);
        if (remaining == 0) {
            while (idx < size) {
                u16 t = data[idx];
                if (t == 0) {
                    break;
                }
                if (t == 0xD) {
                    arg2[count] = 0xA;
                } else {
                    arg2[count] = t;
                }
                count++;
                idx++;
                if (count >= arg3) {
                    break;
                }
            }
        }
    }

    return count;
}
}

#include <gf/gf_thread.h>
#include <gf/gf_task.h>
#include <revolution/OS/OSThread.h>
#include <revolution/OS/OSTime.h>
// TODO: clean up, give variables real names
namespace jj_8014E478 {
extern "C" {
void __ct__15nteWiiConnect24FQ25Heaps8HeapType(void*);
void* memset(void*, int, unsigned long);
extern "C" char lbl_80461048[];
}

extern "C" void* __ct__11nteExchangeFQ25Heaps8HeapType(void* self) {
    char* p = (char*)self;
    __ct__15nteWiiConnect24FQ25Heaps8HeapType(self);
    *(int*)(p + 0x88) = 0;
    *(char**)(p + 0x94) = lbl_80461048;
    *(char**)(p + 0x14) = lbl_80461048 + 8;
    *(char**)(p + 0x18) = lbl_80461048 + 0x14;
    memset(p + 0x30, 0, 0x40);
    *(short*)(p + 0x70) = 0;
    *(int*)(p + 0x74) = 0;
    *(int*)(p + 0x78) = 0;
    *(int*)(p + 0x7c) = 0;
    *(int*)(p + 0x1c) = 0;
    *(int*)(p + 0x20) = 0;
    *(char*)(p + 0x8c) = 1;
    *(int*)(p + 0x90) = 0;
    memset(p + 0x80, 0, 8);
    *(int*)(p + 0x24) = 0;
    *(int*)(p + 0x28) = 0;
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_8014E520 {

extern "C" {
extern "C" char lbl_80461048[];
void* __dt__15nteWiiConnect24Fv(void*, int);
}

inline bool terminated(gfThread* thread) {
    return OSIsThreadTerminated((OSThread*)((u8*)thread + 8)) == 1;
}

extern "C" void* __dt__11nteExchangeFv(void* self, int flag) {
    if (self != 0) {
        u8* p = (u8*)self;
        gfThread* thread = *(gfThread**)(p + 0x1C);
        *(char**)(p + 0x94) = lbl_80461048;
        *(char**)(p + 0x14) = lbl_80461048 + 8;
        *(char**)(p + 0x18) = lbl_80461048 + 0x14;
        if (thread != 0) {
            while (!terminated(*(gfThread**)(p + 0x1C))) {
                OSSleepTicks((*(u32*)0x800000F8 / 4) / 1000);
            }
            delete *(gfThread**)(p + 0x1C);
            *(gfThread**)(p + 0x1C) = 0;
        }
        void* buffer = *(void**)(p + 0x88);
        if (buffer != 0) {
            gfHeapManager::free(buffer);
            *(void**)(p + 0x88) = 0;
        }
        u8* task = *(u8**)(p + 0x20);
        if (task != 0) {
            ((gfTask*)(task + 4))->exit();
            *(u8**)(p + 0x20) = 0;
        }
        __dt__15nteWiiConnect24Fv(self, 0);
        if (flag > 0) {
            operator delete(self);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_8014E720 {
extern "C" {
s32 _getMailSentSizeToday__15nteWiiConnect24Fv(void);
s32 _openLibrary__15nteWiiConnect24Fi(void*, s32);
s32 fn_8014F320(void*, s32, void*, s32);
s32 _procGeneralError__15nteWiiConnect24Fi(void*, s32);
s32 NWC24InitMsgObj(void*, s32);
s32 NWC24SetMsgToId(void*, s32, s32, s32);
s32 NWC24SetMsgSubject(void*, void*, s32);
s32 NWC24SetMsgText(void*, void*, s32, s32, s32);
s32 NWC24SetMsgAttached(void*, s32, s32, s32);
s32 NWC24SetMsgTag(void*, u16);
s32 NWC24SetMsgLedPattern(void*, s32);
void* __ct__11gfCryptoAESFv(void*);
s32 encrypt__11gfCryptoAESFPCvUlQ25Heaps8HeapType(void*, u32, u32, u32);
void* __dt__11gfCryptoAESFv(void*, s32);
s32 sendMails__15nteWiiConnect24FUlPP11NWC24MsgObj(void*, u32, s32*);
int sprintf(char*, const char*, ...);
u32 strlen(const char*);
void OSSleepTicks(s64);
extern "C" char lbl_8059E7E0[6];
}

extern "C" s32 _mail_SendMail__11nteExchangeFv(void* arg) {
    char sp430[0x390];
    char sp230[0x200];
    char sp130[0x100];
    char sp30[0x100];
    char sp10[0x20];
    s32 sp8[2];
    s32 r31;
    s32 r;
    s32 t0;
    s32 t1;
    u16 i;

    if (*(s32*)((u8*)arg + 0x7c) == 0) {
        return -1;
    }
    _getMailSentSizeToday__15nteWiiConnect24Fv();
    if (_openLibrary__15nteWiiConnect24Fi(arg, 1) == 0) {
        return -1;
    }
    sprintf(sp10, lbl_8059E7E0, *(s32*)((u8*)arg + 0x7c));
    r31 = fn_8014F320(arg, *(s32*)((u8*)arg + 0x7c), sp230, 0x100);
    r = NWC24InitMsgObj(sp130, 1);
    if (r != 0) {
        _procGeneralError__15nteWiiConnect24Fi(arg, r);
        return -1;
    }
    for (i = 0; i < *(u16*)((u8*)arg + 0x70); i++) {
        u8* e = (u8*)arg + (u32)i * 8;
        r = NWC24SetMsgToId(sp130, (s32)e, *(s32*)(e + 0x30), *(s32*)(e + 0x34));
        if (r != 0) {
            _procGeneralError__15nteWiiConnect24Fi(arg, r);
            return -1;
        }
    }
    r = NWC24SetMsgSubject(sp130, sp10, strlen(sp10));
    if (r != 0) {
        _procGeneralError__15nteWiiConnect24Fi(arg, r);
        return -1;
    }
    __ct__11gfCryptoAESFv(sp430);
    ((u8*)sp430)[0x380] = 1;
    if (*(s32*)((u8*)arg + 0x78) != 0 && *(s32*)((u8*)arg + 0x74) != 0) {
        if (encrypt__11gfCryptoAESFPCvUlQ25Heaps8HeapType(sp430, *(u32*)((u8*)arg + 0x74),
                        *(u32*)((u8*)arg + 0x78), *(u32*)((u8*)arg + 0x4)) == 0) {
            __dt__11gfCryptoAESFv(sp430, -1);
            return -1;
        }
        while (*(u8*)(sp430 + 0x370) == 0) {
            OSSleepTicks((s64)((*(u32*)0x800000F8 >> 2) / 1000));
        }
        if (*(s32*)(sp430 + 0x374) != 0) {
            __dt__11gfCryptoAESFv(sp430, -1);
            return -1;
        }
        t0 = *(s32*)(sp430 + 0x378);
        t1 = *(s32*)(sp430 + 0x37C);
        r = NWC24SetMsgAttached(sp130, t0, t1, 0x30000);
        if (r != 0) {
            _procGeneralError__15nteWiiConnect24Fi(arg, r);
            __dt__11gfCryptoAESFv(sp430, -1);
            return -1;
        }
    }
    __dt__11gfCryptoAESFv(sp430, -1);
    r = NWC24InitMsgObj(sp30, 2);
    if (r != 0) {
        _procGeneralError__15nteWiiConnect24Fi(arg, r);
        return -1;
    }
    for (i = 0; i < *(u16*)((u8*)arg + 0x70); i++) {
        u8* e = (u8*)arg + (u32)i * 8;
        r = NWC24SetMsgToId(sp30, (s32)e, *(s32*)(e + 0x30), *(s32*)(e + 0x34));
        if (r != 0) {
            _procGeneralError__15nteWiiConnect24Fi(arg, r);
            return -1;
        }
    }
    r = NWC24SetMsgText(sp30, sp230, r31 * 2, 0x10010, 2);
    if (r != 0) {
        _procGeneralError__15nteWiiConnect24Fi(arg, r);
        return -1;
    }
    r = NWC24SetMsgTag(sp30, (u16)(*(s32*)((u8*)arg + 0x7c) + 1));
    if (r != 0) {
        _procGeneralError__15nteWiiConnect24Fi(arg, r);
        return -1;
    }
    r = NWC24SetMsgLedPattern(sp30, 0x2019);
    if (r != 0) {
        _procGeneralError__15nteWiiConnect24Fi(arg, r);
        return -1;
    }
    sp8[0] = (s32)sp130;
    sp8[1] = (s32)sp30;
    r = sendMails__15nteWiiConnect24FUlPP11NWC24MsgObj(arg, 2, sp8);
    return -(r == 0);
}
}

// TODO: clean up, give variables real names
namespace jj_8014EA8C {
extern "C" {
void OSSleepTicks(s64);
void __ct__11gfCryptoAESFv(void*);
void __dt__11gfCryptoAESFv(void*, s32);
s32 decrypt__11gfCryptoAESFPCvUlQ25Heaps8HeapType(void*, s32, s32, s32);
s32 requestListCountVF__14gfCollectionIOFi(void*, s32);
s32 isEnableVFSaveLoad__14gfCollectionIOFv(void*);
void fn_8014CD5C(void);
void startSearchMail__15nteWiiConnect24FUl(void*, s32);
s32 getNextMailForMe__15nteWiiConnect24FP11NWC24MsgObjPUl(void*, void*, s32*);
s32 _procGeneralError__15nteWiiConnect24Fi(void*, s32);
s32 fn_8039B160(void*, u32*);
s32 fn_8039B17C(void*, s32, s32*);
s32 fn_8039B1B0(void*, s32*);
s32 fn_8039B5AC(s32, s32);
s32 fn_8039FD68(void*, u8*, s32);
s32 fn_8039FEB0(void*, s32, s32, s32);
void* memset(void*, s32, u32);
u32 srGetAppInitialCodeLocaleNum__Fv(void);
s32 srGetAppInitialCodeLocale__FUl(u32);
u32 strlen(const char*);
}

class Object_vptr_0 {
public:
    virtual u8 slot0(s32 a, s32 b, void* c);
};

#define WAIT_TICKS() OSSleepTicks((s64)(((*(u32*)0x800000F8) >> 2) / 1000))

inline s32 parseName(const char* name) {
    s32 result = 0;
    if (name == NULL || strlen(name) != 3) return 0;
    if (name[0] == 'd' && name[1] == '0') result = name[2] - '0';
    if (result < 0 || result >= 4) result = 0;
    return result;
}

extern "C" s32 _mail_ReceiveMail__11nteExchangeFiPUs(void* arg0, void* arg1, u16* arg2) {
    s32 r21;
    void* r20;
    u8 sp160[0x384];
    u8 sp60[0x100];
    u8 sp40[0x20];
    u16 sp38[4];
    u32 sp30;
    s32 sp2c;
    s32 sp28;
    s32 sp24;
    s32 sp20;
    s32 sp1c;
    s32 sp18;
    s32 sp14;
    s32 sp10;
    s32 spc;
    s32 sp8;
    s32 r29;
    u32 i;
    u32 count;
    s32 t;
    s32 rr;

    if (arg1 == NULL) {
        arg1 = arg0;
        if (arg0 != NULL) {
            arg1 = (u8*)arg0 + 0x18;
        }
    }
    r21 = -1;
    memset(arg2, 0, 8);
    memset(sp38, 0, 8);
    r20 = *(void**)((u8*)arg0 + 0x20);
    if (r20 == NULL) {
        return -1;
    }
    if (isEnableVFSaveLoad__14gfCollectionIOFv(r20) == 0) {
        goto done;
    }
    if (requestListCountVF__14gfCollectionIOFi(r20, 1) == 0) {
        goto done;
    }
    while (*(u8*)((u8*)r20 + 0x5C) == 0) {
        WAIT_TICKS();
    }
    if (*(s32*)((u8*)r20 + 0x60) != 0) {
        goto done;
    }
    sp38[1] = (u16)*(u32*)((u8*)r20 + 0xE8);
    if (requestListCountVF__14gfCollectionIOFi(r20, 2) == 0) {
        goto done;
    }
    while (*(u8*)((u8*)r20 + 0x5C) == 0) {
        WAIT_TICKS();
    }
    if (*(s32*)((u8*)r20 + 0x60) != 0) {
        goto done;
    }
    sp38[2] = (u16)*(u32*)((u8*)r20 + 0xE8);
    if (requestListCountVF__14gfCollectionIOFi(r20, 3) == 0) {
        goto done;
    }
    while (*(u8*)((u8*)r20 + 0x5C) == 0) {
        WAIT_TICKS();
    }
    if (*(s32*)((u8*)r20 + 0x60) != 0) {
        goto done;
    }
    sp38[3] = (u16)*(u32*)((u8*)r20 + 0xE8);
    r21 = 0;
    count = srGetAppInitialCodeLocaleNum__Fv();
    for (i = 0; i < count; i++) {
        sp2c = 0;
        startSearchMail__15nteWiiConnect24FUl(arg0, srGetAppInitialCodeLocale__FUl(i));
        while (getNextMailForMe__15nteWiiConnect24FP11NWC24MsgObjPUl(arg0, sp60, &sp2c) != 0) {
            t = fn_8039FD68(sp60, sp40, 0x20);
            if (t != 0) {
                if ((t <= -0x10 && t >= -0x15) || t == -0x26 || t == -0x29 || t == -0x2B || t == -0x2E) {
                    fn_8039B1B0(sp60, &sp28);
                    fn_8039B5AC(1, sp28);
                }
                if (sp2c == 0) {
                    break;
                }
                continue;
            }
            r29 = parseName((const char*)sp40);
            if (r29 == 0) {
                fn_8039B1B0(sp60, &sp24);
                fn_8039B5AC(1, sp24);
                if (sp2c == 0) {
                    break;
                }
                continue;
            }
            if (r21 == -2) {
                arg2[r29] += 1;
                if (sp2c == 0) {
                    break;
                }
                continue;
            }
            if (sp38[r29] >= 0x3E8) {
                r21 = -3;
                arg2[r29] += 1;
                if (sp2c == 0) {
                    break;
                }
                continue;
            }
            if (fn_8039B160(sp60, &sp30) != 0) {
                fn_8014CD5C();
                if (sp2c == 0) {
                    break;
                }
                continue;
            }
            if (sp30 != 1) {
                fn_8039B1B0(sp60, &sp20);
                fn_8039B5AC(1, sp20);
                if (sp2c == 0) {
                    break;
                }
                continue;
            }
            sp1c = 0;
            t = fn_8039B17C(sp60, 0, &sp1c);
            if (t != 0) {
                if ((t <= -0x10 && t >= -0x15) || t == -0x26 || t == -0x29 || t == -0x2B || t == -0x2E) {
                    fn_8039B1B0(sp60, &sp18);
                    fn_8039B5AC(1, sp18);
                }
                if (sp2c == 0) {
                    break;
                }
                continue;
            }
            t = fn_8039FEB0(sp60, 0, *(s32*)((u8*)arg0 + 0x88), 0x23000);
            if (t != 0) {
                if ((t <= -0x10 && t >= -0x15) || t == -0x26 || t == -0x29 || t == -0x2B || t == -0x2E) {
                    fn_8039B1B0(sp60, &sp14);
                    fn_8039B5AC(1, sp14);
                }
                if (sp2c == 0) {
                    break;
                }
                continue;
            }
            __ct__11gfCryptoAESFv(sp160);
            sp160[0x380] = 1;
            if (decrypt__11gfCryptoAESFPCvUlQ25Heaps8HeapType(sp160, *(s32*)((u8*)arg0 + 0x88), sp1c, *(s32*)((u8*)arg0 + 0x4)) == 0) {
                fn_8039B1B0(sp60, &sp10);
                fn_8039B5AC(1, sp10);
                if (sp2c == 0) {
                    __dt__11gfCryptoAESFv(sp160, -1);
                    break;
                }
                __dt__11gfCryptoAESFv(sp160, -1);
                continue;
            }
            while (sp160[0x370] == 0) {
                WAIT_TICKS();
            }
            if (*(s32*)(sp160 + 0x374) != 0) {
                fn_8039B1B0(sp60, &spc);
                fn_8039B5AC(1, spc);
                if (sp2c == 0) {
                    __dt__11gfCryptoAESFv(sp160, -1);
                    break;
                }
                __dt__11gfCryptoAESFv(sp160, -1);
                continue;
            }
            if (((Object_vptr_0*)arg1)->slot0(*(s32*)(sp160 + 0x378), *(s32*)(sp160 + 0x37C), r20) == 0) {
                fn_8039B1B0(sp60, &sp8);
                rr = fn_8039B5AC(1, sp8);
                if (rr != 0 && _procGeneralError__15nteWiiConnect24Fi(arg0, rr) == 0) {
                    __dt__11gfCryptoAESFv(sp160, -1);
                    goto done;
                }
                sp38[r29] += 1;
            } else {
                arg2[r29] += 1;
                r21 = -2;
                if (sp2c == 0) {
                    __dt__11gfCryptoAESFv(sp160, -1);
                    break;
                }
                __dt__11gfCryptoAESFv(sp160, -1);
                continue;
            }
            __dt__11gfCryptoAESFv(sp160, -1);
        }
    }
done:
    return r21;
}
}

// TODO: clean up, give variables real names
namespace jj_8014F0F0 {
extern "C" {
void fn_80153488(int*);
void fn_80153498(int*, int);
int fn_80153504(int*, int);
int fn_80153810(int*);
int getDataType__10nteSectionFv(int*);
int getSize__10nteSectionFv(int*);
u8 getSections_Direct__7nteFileFPCUc(void*);
void* getSectionData_Direct__7nteFileFPUcUl(void*, u8);
int requestNewFileNameVF__14gfCollectionIOFi(void*, int);
int requestSaveVF__14gfCollectionIOFiPCcPvib(void*, int, void*, int, int, int);
}
extern "C" unsigned char lbl_80423690[];
extern "C" int fn_8014F0F0(void* arg0, void* collection, void* arg2, u8* target) {
    int entry[2];
    int value;
    u8 count;
    int success;
    u8 i;
    int type;
    unsigned char* status;
    count = getSections_Direct__7nteFileFPCUc(collection);
    status = lbl_80423690;
    success = 0;
    i = 0;
    while ((u8)i < count) {
        fn_80153488(entry);
        if (fn_80153504(entry, (int)getSectionData_Direct__7nteFileFPUcUl(collection, i)) == 0) {
            fn_80153498(entry, -1);
        } else if (getSize__10nteSectionFv(entry) == 0) {
            fn_80153498(entry, -1);
        } else {
            type = 0;
            if (getDataType__10nteSectionFv(entry) == 1) {
                type = 1;
            } else if (getDataType__10nteSectionFv(entry) == 2) {
                type = 2;
            } else if (getDataType__10nteSectionFv(entry) == 3) {
                type = 3;
            }
            if (type != 0) {
                value = getSize__10nteSectionFv(entry);
                while (status[0] == 0) {
                    OSSleepTicks(((*(u32*)0x800000F8) / 4) / 1000);
                }
                if ((u32)value > (u32)(*(u32*)(status + 0xC) << 17)) {
                    success = 1;
                    fn_80153498(entry, -1);
                    goto done;
                }
                if (requestNewFileNameVF__14gfCollectionIOFi(target, type) != 0) {
                    while (target[0x5C] == 0) {
                        OSSleepTicks(((*(u32*)0x800000F8) / 4) / 1000);
                    }
                    if (*(int*)(target + 0x60) == 0) {
                        if (requestSaveVF__14gfCollectionIOFiPCcPvib(target, type, target + 0x64, fn_80153810(entry), getSize__10nteSectionFv(entry), 0) != 0) {
                            while (target[0x5C] == 0) {
                                OSSleepTicks(((*(u32*)0x800000F8) / 4) / 1000);
                            }
                        }
                    }
                }
            }
            fn_80153498(entry, -1);
        }
        i++;
    }
done:
    return success;
}
}

// TODO: clean up, give variables real names
namespace jj_8014F490 {
struct gfRunnable {
    void* vtable;
};

struct gfThread {
    void* vtable;
    char pad4[4];
    char thread[0x318];
    char tail[0x20];
};

struct Prefix {
    char pad0[4];
    int heap;
    char pad8[0xC];
};

struct Owner : Prefix, gfRunnable {
    char pad18[4];
    gfThread* thread;
    char pad20[4];
    int arg1;
    int arg2;
    char pad2C[0x60];
    bool flag;
};

extern "C" {
    int OSIsThreadTerminated(void*);
    int OSResumeThread(void*);
    void* __dt__8gfThreadFv(gfThread*, short);
    void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
    void createThread__8gfThreadFP10gfRunnableUlUlQ25Heaps8HeapType(
        gfThread*, gfRunnable*, unsigned long, unsigned long, int);
    extern "C" char __vt__8gfThread[];
}

inline bool terminated(gfThread* thread) {
    return OSIsThreadTerminated(thread->thread) == 1;
}

inline void initialize(gfThread* thread, gfRunnable* runnable, int heap) {
    thread->vtable = __vt__8gfThread;
    createThread__8gfThreadFP10gfRunnableUlUlQ25Heaps8HeapType(
        thread, runnable, 0x10, 0x2000, heap);
}

extern "C" int _requestJob__11nteExchangeFii(Owner* self, int arg1, int arg2) {
    if (self->thread != 0) {
        if (!terminated(self->thread))
            return 0;
        __dt__8gfThreadFv(self->thread, 1);
        self->thread = 0;
    }

    int heap = self->heap;
    gfThread* thread = (gfThread*)__nw__FUlQ25Heaps8HeapType(0x340, heap);
    if (thread != 0) {
        initialize(thread, static_cast<gfRunnable*>(self), heap);
    }

    self->thread = thread;
    if (thread == 0)
        return 0;

    self->arg1 = arg1;
    self->arg2 = arg2;
    self->flag = false;
    OSResumeThread(thread->thread);
    return 1;
}
}

