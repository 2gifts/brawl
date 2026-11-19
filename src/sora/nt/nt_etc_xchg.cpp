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
extern unsigned char __vt__8gfThread[];
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

