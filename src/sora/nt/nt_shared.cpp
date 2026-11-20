#include <nt/nt_report.h>
#include <gf/gf_heap_manager.h>
#include <string.h>
#include <nt/nt_offline.h>
#include <mu/wifi/mu_wifi_interface.h>
#include <nt/nt_send.h>
#include <revolution/OS/OSTime.h>
#include <gf/gf_callback.h>
#include <ip/ip_switch.h>
#include <gf/gf_error_manager.h>
#include <types.h>

// TODO: clean up, give variables real names
namespace jj_8013DB48 {
struct NtBufferObj {
    void* vtable;
    u32 m_size;
    s32 unk8;
    bool unkC;
    void* m_buf;
};

extern "C" unsigned char __vt__8NtBuffer[];

// TODO: clean up, give variables real names
extern "C" void fn_8013DB48(NtBufferObj* self) {
    self->m_size = 0;
    self->vtable = __vt__8NtBuffer;
    self->unk8 = 0;
    self->unkC = 0;
    self->m_buf = nullptr;
}
}

namespace jj_8013DB6C {

extern "C" unsigned char __vt__8NtBuffer[];
extern "C" void __dl__FPv(void*);

struct NtBufferDispatch : NtBuffer {
    virtual void freeBuffer() = 0;
};

// TODO: clean up, give variables real names
extern "C" NtBuffer* fn_8013DB6C(NtBuffer* self, int flag) {
    if (self) {
        if (self) {
            *(void**)self = __vt__8NtBuffer;
            static_cast<NtBufferDispatch*>(self)->freeBuffer();
        }
        if (flag > 0)
            __dl__FPv(self);
    }
    return self;
}
}

namespace jj_801410CC {
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void* fn_801410CC(void* self, int index) {
    void* root = *(void**)((char*)self + 0x454);
    if (root == 0) return 0;

    char* base = (char*)root;
    if (*(unsigned char*)(base + index * 0x18 + 0x14) == 0) return 0;

    unsigned short counter = *(unsigned short*)(base + 0xe);
    char* node = base;

    while (true) {
        char* next = *(char**)(node + 0x8);
        if (next == 0) return (void*)node;

        counter++;
        if (*(unsigned char*)(next + index * 0x18 + 0x14) == 0) return (void*)node;

        unsigned short nextIdx = *(unsigned short*)(next + 0xe);
        if (nextIdx != counter) return (void*)node;

        node = next;
    }
}
}

namespace jj_8013DBD8 {
extern "C" {
extern "C" unsigned char lbl_8045E7AC[];
extern "C" unsigned char __vt__8NtBuffer[];
void __dl__FPv(void*);
}
class NtBuffer {
public:
    virtual void fn_8013DBD8();
    virtual void allocBuffer();
    virtual void freeBuffer();
    unsigned int m_size;
    int unk8;
    bool unkC;
    void* m_buf;
};
struct BufferObj : NtBuffer {
    int a, b, c, d;
};
// TODO: clean up, give variables real names
extern "C" void* fn_8013DBD8(BufferObj* self, int flag) {
    if (self) {
        *(void**)self = lbl_8045E7AC;
        if (!self->unkC && self->m_buf)
            gfHeapManager::free(self->m_buf);
        self->m_buf = 0;
        self->m_size = 0;
        self->unk8 = 0;
        self->a = 0;
        self->b = 0;
        self->c = 0;
        self->d = 0;
        if (self) {
            *(void**)self = __vt__8NtBuffer;
            self->freeBuffer();
        }
        if (flag > 0)
            __dl__FPv(self);
    }
    return self;
}
}

namespace jj_80142BF0 {

extern "C" void netReport__FPCce(const char*, ...);
extern "C" char lbl_8045E600[0x48];
extern "C" char lbl_8045E648[0x14C];

struct NetBuf {
    char pad[0x23C];
    char* data;
    int cap;
    int stride;
    int head;
    int tail;
};

// TODO: clean up, give variables real names
extern "C" void fn_80142BF0(NetBuf* buf, int value) {
    int sum = buf->cap + (buf->tail - buf->head);
    int rem = sum % buf->cap;

    if (buf->cap - 1 - rem < 1) {
        netReport__FPCce(lbl_8045E600);
        return;
    }

    short item = (short)value;
    memcpy(buf->data + buf->tail * buf->stride, &item, buf->stride);

    int next = buf->tail + 1;
    buf->tail = next % buf->cap;
    netReport__FPCce(lbl_8045E648, value, buf->cap, next);
}
}

namespace jj_8013EFCC {

class Item {
public:
    virtual void v0();
    virtual void v1();
    virtual void clear();
    char pad[0x10];
};

struct Object {
    char pad0[0x20];
    void* buffer;
    char pad24[0x14];
    Item a;
    char pad4c[0xc];
    Item items[4];
    char pada8[0xa8];
    Item b;
    char pad164[4];
    Item c;
    char pad17c[0xb4];
    int x230;
    int x234;
    unsigned char flag;
    char pad239[3];
    void* buffer2;
    int x240;
    int x244;
    int x248;
    int x24c;
};

// TODO: clean up, give variables real names
extern "C" void fn_8013EFCC(Object* self) {
    self->b.clear();
    self->a.clear();
    gfHeapManager::free(self->buffer);
    self->buffer = 0;
    self->c.clear();
    if (!self->flag && self->buffer2 != 0)
        gfHeapManager::free(self->buffer2);
    self->buffer2 = 0;
    self->x230 = 0;
    self->x234 = 0;
    self->x240 = 0;
    self->x244 = 0;
    self->x248 = 0;
    self->x24c = 0;
    for (unsigned char i = 0; i < 4; ++i)
        self->items[i].clear();
}
}

namespace jj_801416CC {
struct Entry {
    int state; char pad[4]; Entry* next; char pad2[8];
    struct Player { unsigned char active; char pad[0x17]; } players[4];
};
struct Manager { char pad[0x454]; Entry* entries; };
struct Object {
    char pad[0x11]; unsigned char flag; char pad2[0x9e];
    struct Player { signed char active; char pad[0x1f]; } players[4];
};
extern "C" Manager* lbl_805A0390;
extern "C" void fn_80140C84(Object*);
inline bool checkPlayers(Object* obj, Entry::Player* players) {
    for (int i = 0; i < 4; i++) {
        if (obj->players[i].active && !players[(unsigned char)i].active) return false;
    }
    return true;
}
// TODO: clean up, give variables real names
extern "C" void fn_801416CC(Object* obj) {
    Entry::Player* playerData;
    Entry* second;
    Entry* first;
    fn_80140C84(obj);
    if (obj->flag == 1) {
        first = lbl_805A0390->entries;
        while (first) {
            if ((unsigned int)(first->state - 2) <= 1) first->state = 4;
            first = first->next;
        }
    } else {
        second = lbl_805A0390->entries;
        while (second) {
            if (second->state == 3) {
                playerData = second->players;
                if (checkPlayers(obj, playerData)) second->state = 4;
            }
            second = second->next;
        }
    }
}
}

namespace jj_8013F9B8 {

extern "C" {
int getFriendAIDIndexAID__19muWifiInterfaceTaskFi(void* self, int idx);
void fn_800D2164(void* self, int idx, u8 value);
extern "C" void* lbl_805A0390;
extern "C" u8 lbl_8059E742;
extern "C" const f32 lbl_805A2AC8;
extern "C" const f32 lbl_805A2B14;
extern "C" const f32 lbl_805A2B18;
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8013F9B8(char* arg0) {
    muWifiInterfaceTask* task = g_muWifiInterfaceTask;
    u32 i;
    for (i = 0; i < 4; i++) {
        if ((u8)i == lbl_8059E742) {
            continue;
        }
        u32 off = (u8)i << 5;
        if (*(s8*)(arg0 + off + 0xB0) != 0) {
            int r = getFriendAIDIndexAID__19muWifiInterfaceTaskFi(task, (u8)i);
            if (r < 0) {
                continue;
            }
            ntFriendInfo::getInstance();
            f32 f = *(f32*)((char*)lbl_805A0390 + off + 0xBC);
            u32 n = (u32)f / (*(u32*)0x800000F8 / 4 / 1000);
            f32 x = (f32)n - lbl_805A2B14;
            if (x < lbl_805A2AC8) {
                x = lbl_805A2AC8;
            }
            s32 level = (s32)(x / lbl_805A2B18);
            if (level > 4) {
                level = 4;
            }
            fn_800D2164(task, r, (u8)(5 - level));
        }
    }
}
}

namespace jj_80143974 {
extern "C" u8 lbl_8059E742;
extern "C" u8* lbl_805A0390;
extern "C" const f32 lbl_805A2AC8;
extern "C" const f32 lbl_805A2ACC;
extern "C" const f32 lbl_805A2B40;
extern "C" const f32 lbl_805A2B44;
extern "C" const f32 lbl_805A2B48;

struct Ent {
    s8 active;
    u8 pad[0x1b];
    u16 current;
    u16 previous;
};

struct Self {
    u8 pad[0x1f8];
    f32 value;
    f32 smoothed;
};

static inline f32 clamp(f32 v, f32 lo, f32 hi) {
    if (v < lo)
        v = lo;
    if (v > hi)
        v = hi;
    return v;
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" f32 fn_80143974(Self* self) {
    f32 sum = lbl_805A2AC8;
    f32 count = lbl_805A2AC8;
    u16 current;
    Ent* e = (Ent*)(lbl_805A0390 + 0xb0);
    u8 i = 0;

    for (; i < 4; ++i, ++e) {
        if (i != lbl_8059E742 && e->active != 0) {
            current = e->current;
            int diff = current - e->previous;
            e->previous = current;
            f32 v = clamp((f32)diff, lbl_805A2AC8, lbl_805A2B40);
            count += lbl_805A2ACC;
            sum += v;
        }
    }

    if (count > lbl_805A2AC8)
        sum /= count;

    self->value = sum;
    if (lbl_805A2AC8 == self->smoothed)
        self->smoothed = (f32)(f64)sum;
    else
        self->smoothed = (lbl_805A2B44 * self->smoothed) / lbl_805A2B48 +
                         (f32)(f64)sum / lbl_805A2B48;
    return sum;
}
}

namespace jj_80142A80 {

extern "C" NtSend* lbl_805A03D4;

struct SendEntry {
    char pad[8];
    s32 size;
    char pad2[4];
    void* data;
};

struct SendFlag {
    s8 active;
    u8 priority;
    char pad[30];
};

struct Object {
    char pad[0x58];
    SendEntry entries[4];
    char pad2[8];
    SendFlag flags[4];
};

// TODO: clean up, give variables real names
extern "C" void fn_80142A80(Object* self) {
    u8 done[4];
    done[0] = self->flags[0].active == 0;
    done[1] = self->flags[1].active == 0;
    done[2] = self->flags[2].active == 0;
    done[3] = self->flags[3].active == 0;

    for (;;) {
        SendFlag* flag;
        SendEntry* entry;
        entry = self->entries;
        flag = self->flags;
        u8 best = 0;
        int selected = -1;

        for (u8 i = 0; i < 4; i++, entry++, flag++) {
            if (!done[i] && flag->active && entry->size != 0 &&
                best <= flag->priority) {
                best = flag->priority;
                selected = i;
            }
        }

        if (selected >= 0) {
            done[selected] = true;
            SendEntry* chosen = &self->entries[selected];
            lbl_805A03D4->pushback(selected, chosen->data, chosen->size);
            chosen->size = 0;
        }

        if (selected < 0)
            break;
    }
}
}

namespace jj_80140604 {

extern "C" {
void fn_8013E470(void* self, u16 a, s32 b);
void fn_8013FDD4(void* self);
void setSIDPlayer__6NtBaseFUcUc(u8 a, s32 b);
void netReport__FPCce(const char*, ...);
}

struct Mgr {
    char pad0[0x50];
    s32 unk50;
    s32 unk54;
    char pad58[0xA8 - 0x58];
    s32 unkA8;
    char padAC[0x144 - 0xAC];
    u16 unk144;
    u16 unk146;
    char pad148[0x178 - 0x148];
    u8* unk178;
    char pad17C[0x194 - 0x17C];
    u32 unk194;
    char pad198[0x204 - 0x198];
    f32 unk204;
};

struct Arg {
    char pad0[4];
    u8 unk4;
    u8 unk5;
    char pad6[4];
    u8 unkA[4];
    u8 unkE;
    char padF[5];
    f32 unk14;
};

struct Pair {
    u8 a;
    u8 b;
};

extern "C" Mgr* lbl_805A0390;
extern "C" NtSend* lbl_805A03D4;
extern "C" u8 lbl_805A0409;
extern "C" u8 lbl_8059E742;
extern "C" Pair lbl_805A0418[4];
extern "C" char lbl_8045E1D0[];

// TODO: clean up, give variables real names
extern "C" u32 fn_80140604(u8 arg0, u32 arg1, Arg* arg2) {
    u32 i;
    u32 temp_r3;
    u32 temp_r7;
    u8 t;
    u8* buf;

    if (arg1 < 0x18) {
        netReport__FPCce(lbl_8045E1D0);
        return arg1;
    }
    if (lbl_805A0390->unkA8 != 4) {
        lbl_805A0390->unkA8 = 4;
        fn_8013FDD4(lbl_805A0390);
        fn_8013E470(lbl_805A0390, (u16)(arg2->unk5 + (arg2->unk4 << 8)), 1);
        lbl_805A0390->unk146 = lbl_805A0390->unk144;
        i = 0;
        do {
            setSIDPlayer__6NtBaseFUcUc((u8)i, arg2->unkA[(u8)i]);
            i++;
        } while (i < 4);
        lbl_805A0390->unk50 = lbl_805A0390->unk54 * lbl_805A0418[lbl_8059E742].b;
        lbl_805A0409 = arg2->unkE;
        lbl_805A0390->unk204 = arg2->unk14;
    }
    buf = lbl_805A0390->unk178;
    buf[0] = 8;
    buf[1] = 0xF8;
    t = buf[2] & 0xFC;
    buf[3] = 0x18;
    buf[2] = (u8)((t & 3) | 4);
    temp_r3 = arg2->unk5 + (arg2->unk4 << 8);
    buf[4] = (u8)(temp_r3 >> 8);
    buf[5] = (u8)temp_r3;
    temp_r7 = lbl_805A0390->unk194;
    buf[6] = (u8)(temp_r7 >> 24);
    buf[7] = (u8)(temp_r7 >> 16);
    buf[8] = (u8)(temp_r7 >> 8);
    buf[9] = (u8)temp_r7;
    lbl_805A03D4->pushback(arg0, buf, 0x18);
    return 0x18;
}
}

namespace jj_8014044C {

struct Entry {
    u8 flag;
    u8 pad1;
    u16 id;
    u32 tick;
};

struct Obj {
    char pad000[0x130];
    s32 unk130;
    char pad134[0x10];
    u16 unk144;
    char pad146[6];
    u32 unk14C;
    char pad150[0x5A];
    s8 unk1AA;
    s8 unk1AB;
    char pad1AC[0x40];
    s32 unk1EC;
    s32 unk1F0;
    f32 unk1F4;
    char pad1F8[0x58];
    Entry unk250[0x40];
};

extern "C" {
void fn_8014BF68(char* p, int x);
u32 OSGetTick(void);
f32 setErrorStatus__10NtMatchingFl(int x);
u64 netMinReport__FPCce(const char* fmt, ...);
void netReport__FPCce(const char* fmt, ...);
}

extern "C" int lbl_805A0434;
extern "C" u8 lbl_8059E742;
extern "C" char lbl_8045E188[];
extern "C" NtReport lbl_8045E1C0;

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8014044C(Obj* obj) {
    if (obj->unk14C == 0xB4) {
        int p = lbl_805A0434;
        fn_8014BF68((char*)p, 0);
        *(s32*)(p + 0x3430) = 0;
    }

    if (obj->unk14C % 60 == 1) {
        s32 prev = obj->unk1EC;
        s32 var = 100;
        obj->unk1AA = (s8)(obj->unk144 - prev);
        obj->unk1EC = obj->unk144;
        obj->unk1AB = (u8)(obj->unk130 - obj->unk1F0);
        obj->unk1F0 = obj->unk130;
        u8 b = *((u8*)obj + (lbl_8059E742 << 5) + 0xB1);
        if (b > 8) {
            var = 100 - (b - 8) * 10;
            if (var < 20) {
                var = 20;
            }
        }
        u8 c = obj->unk1AB;
        if (c > 3) {
            var -= (c - 3) * 10;
        }
        if (var < 0) {
            var = 0;
        }
        obj->unk1F4 = (f32)var;
    } else if (obj->unk14C % 60 == 2) {
        u32 tick = OSGetTick();
        Entry* e = obj->unk250;
        for (int i = 0; i < 0x40; i++, e++) {
            if (e->flag != 0 && (u32)(tick - e->tick) >= ((*(u32*)0x800000F8 >> 2) * 10)) {
                netReport__FPCce(lbl_8045E188, 0xA, e->id, e->flag);
                u64 ret = netMinReport__FPCce(lbl_8045E188, 0xA, e->id, e->flag);
                lbl_8045E1C0.report((const char*)(u32)ret);
                setErrorStatus__10NtMatchingFl(0xA);
            }
        }
    }
}
}

namespace jj_80141814 {
extern "C" {
void netReport__FPCce(const char*, ...);
void netMinReport__FPCce(const char*, ...);
void fn_80140EBC(void*, int);
void closeAID__4NtTrFUc(int, int);
void setErrorStatus__10NtMatchingFl(int);
extern "C" unsigned char lbl_8059E742;
extern "C" int lbl_805A03A8;
extern "C" char lbl_8045E380[];
extern "C" char lbl_8045E3A8[];
}
// TODO: clean up, give variables real names
inline void update(unsigned char* self) {
    self[0x11] = 0;
    self[0x12] = 0;
    for (unsigned char j = 0; j < 4; j++) {
        if (*(signed char*)(self + 0xB0 + j * 0x20) != 0) {
            self[0x12] |= 1 << j;
            self[0x11]++;
        }
    }
}
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" int fn_80141814(void* arg0) {
    unsigned char result;
    unsigned char* data;
    unsigned char* entry;
    unsigned char i;
    unsigned char* self = (unsigned char*)arg0;
    unsigned int tick = OSGetTick();
    entry = self + 0xB0;
    data = (unsigned char*)(*(unsigned int*)(self + 0x454)) + 0x14;
    result = 1;
    for (i = 0; i < 4; i++, entry += 0x20) {
        if (i != lbl_8059E742 && (signed char)entry[0] != 0 && data[i * 0x18] == 0) {
            result = 0;
            unsigned int stored = *(unsigned int*)(entry + 0x14);
            unsigned int clock = *(unsigned int*)0x800000F8;
            unsigned int threshold = ((clock >> 2) / 1000) * 60;
            if (tick - stored > threshold) {
                unsigned short counter = *(unsigned short*)(entry + 4);
                counter++;
                *(unsigned short*)(entry + 4) = counter;
                if (counter >= 0x1F4) {
                    netReport__FPCce(lbl_8045E380, i, stored);
                    netMinReport__FPCce(lbl_8045E3A8);
                    self[i * 0x20 + 0xB0] = 0;
                    update(self);
                    closeAID__4NtTrFUc(lbl_805A03A8, i);
                    setErrorStatus__10NtMatchingFl(0xA);
                    *(unsigned short*)(entry + 4) = 0;
                }
                *(unsigned int*)(entry + 0x14) = tick;
                fn_80140EBC(self, 1);
            }
        }
    }
    return result;
}
}

namespace jj_80140790 {
struct Node {
    s32 type;
    u8 pad4[4];
    Node* next;
    u8 pad0c[2];
    u16 key;
    u8 pad10[4];
    u8 flag;
};
struct Main {
    u8 pad0[0xa8];
    s32 unkA8;
    u8 padAC[0x178 - 0xac];
    u8* unk178;
    u8 pad17c[0x194 - 0x17c];
    u32 unk194;
    u8 pad198[0x454 - 0x198];
    Node* unk454;
};
extern "C" void netReport__FPCce(const char*, ...);
extern "C" char lbl_8045E218[];
extern "C" Main* lbl_805A0390;
extern "C" NtSend* lbl_805A03D4;
extern "C" const f32 lbl_805A2AC8;
extern "C" const f32 lbl_805A2AD4;
extern "C" const f32 lbl_805A2B1C;
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" u32 fn_80140790(u8 arg0, u32 arg1, u8* arg2) {
    Node* n;
    u8* buf;
    u8* base;
    u32 start;
    u32 key;
    u32 r7;
    f32 f3;
    if (arg1 < 0x18) {
        netReport__FPCce(lbl_8045E218);
        return arg1;
    }
    if (lbl_805A0390->unkA8 > 3) return 0x18;
    key = (u16)(arg2[5] + (arg2[4] << 8));
    n = lbl_805A0390->unk454;
    while (n != 0) {
        if (n->key == key) {
            if ((u32)(n->type - 3) > 1U) goto invalid;
            goto found;
        }
        n = n->next;
    }
invalid:
    n = 0;
found:
    if (n == 0) return 0x18;
    ((Node*)((u8*)n + arg0 * 0x18))->flag = 1;
    start = arg2[9] + (arg2[8] << 8) + ((arg2[6] << 24) + (arg2[7] << 16));
    if (start != 0) {
        base = (u8*)lbl_805A0390 + ((arg0 << 5) & 0x1FE0);
        *(f32*)(base + 0xb8) = (f32)(OSGetTick() - start);
        f3 = *(f32*)(base + 0xbc);
        if (lbl_805A2AC8 == f3) {
            *(f32*)(base + 0xbc) = *(f32*)(base + 0xb8);
        } else {
            *(f32*)(base + 0xbc) = lbl_805A2B1C * f3 + lbl_805A2AD4 * *(f32*)(base + 0xb8);
        }
    }
    buf = lbl_805A0390->unk178;
    buf[0] = 8;
    buf[1] = 0xF8;
    u8 t = buf[2] & 0xFC;
    buf[3] = 0x18;
    buf[2] = (u8)((t & 3) | 8);
    {
        u32 k = arg2[5] + (arg2[4] << 8);
        buf[4] = (u8)(k >> 8);
        buf[5] = (u8)k;
    }
    r7 = lbl_805A0390->unk194;
    buf[6] = (u8)(r7 >> 24);
    buf[7] = (u8)(r7 >> 16);
    buf[8] = (u8)(r7 >> 8);
    buf[9] = (u8)r7;
    lbl_805A03D4->pushback(arg0, buf, 0x18);
    return 0x18;
}
}

namespace jj_8013D8A8 {
extern "C" {
void OSInitMutex(void*);
void __construct_array(void*, void (*)(void*), void (*)(void*), u32, s32);
void __dt__8NtBufferFv(void*);
void fn_8013DB48(void*);
void putPacketDelay__10NtMatchingFiUs(s32, s32, s32);
void putPacketLost__10NtMatchingFiUc(s32, s32, s32);
extern "C" unsigned char __vt__8NtBuffer[];
extern "C" unsigned char lbl_8045E794[];
extern "C" unsigned char lbl_8045E7AC[];
extern "C" unsigned char lbl_8045E7C0[];
extern "C" void* lbl_805A0390;
extern "C" s32 lbl_805A0434;
extern "C" const f32 lbl_805A2AC8;
extern "C" const f32 lbl_805A2ACC;
extern "C" const f32 lbl_805A2AD0;
extern "C" const f32 lbl_805A2AD4;
extern "C" const f32 lbl_805A2AD8;
extern "C" const f32 lbl_805A2ADC;
extern "C" const f32 lbl_805A2AE0;
extern "C" const f32 lbl_805A2AE4;
}

struct Byte1D8 {
    u8 a : 1;
    u8 b : 7;
};

struct Word1D8 {
    u32 a : 1;
    u32 b : 8;
    u32 c : 23;
};

struct Word198 {
    u32 a : 8;
    u32 b : 24;
};

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void* fn_8013D8A8(void* arg0) {
    char* p = (char*)arg0;
    s32 i;

    *(void**)(p + 0x00) = lbl_8045E794;
    *(s32*)(p + 0x04) = 0;
    *(u8*)(p + 0x12) = 0;
    *(u8*)(p + 0x13) = 0x14;
    *(u8*)(p + 0x14) = 0;
    *(u8*)(p + 0x15) = 0;
    *(u8*)(p + 0x16) = 0;
    *(f32*)(p + 0x18) = lbl_805A2AC8;
    *(f32*)(p + 0x1C) = lbl_805A2AC8;
    *(s32*)(p + 0x20) = 0;
    *(void**)(p + 0x38) = __vt__8NtBuffer;
    *(s32*)(p + 0x3C) = 0;
    *(s32*)(p + 0x40) = 0;
    *(u8*)(p + 0x44) = 0;
    *(s32*)(p + 0x48) = 0;
    *(s32*)(p + 0x4C) = 0;
    *(s32*)(p + 0x50) = 0;
    *(s32*)(p + 0x54) = 0;
    __construct_array(p + 0x58, fn_8013DB48, __dt__8NtBufferFv, 0x14, 4);
    *(s32*)(p + 0xA8) = 0;
    *(u8*)(p + 0xAC) = 1;
    *(s32*)(p + 0x130) = 0;
    *(s32*)(p + 0x134) = 0;
    *(s32*)(p + 0x138) = 0;
    *(s32*)(p + 0x13C) = 0;
    *(u16*)(p + 0x140) = 0xFC18;
    *(s16*)(p + 0x142) = 0;
    *(s16*)(p + 0x144) = 0;
    *(s16*)(p + 0x146) = 0;
    *(s16*)(p + 0x148) = 0;
    *(void**)(p + 0x150) = __vt__8NtBuffer;
    *(s32*)(p + 0x154) = 0;
    *(s32*)(p + 0x158) = 0;
    *(u8*)(p + 0x15C) = 0;
    *(s32*)(p + 0x160) = 0;
    *(s32*)(p + 0x164) = 0;
    *(s32*)(p + 0x16C) = 0;
    *(s32*)(p + 0x170) = 0;
    *(u8*)(p + 0x174) = 0;
    *(s32*)(p + 0x178) = 0;
    *(void**)(p + 0x168) = lbl_8045E7C0;
    OSInitMutex(p + 0x17C);
    ((Byte1D8*)(p + 0x1D8))->a = 0;
    ((Word198*)(p + 0x198))->a = 0;
    ((Word1D8*)(p + 0x1D8))->b = 0;
    *(s32*)(p + 0x194) = 0;
    *(u8*)(p + 0x199) = 0;
    *(u8*)(p + 0x1A5) = 0;
    *(u8*)(p + 0x1A6) = 0;
    *(u8*)(p + 0x1A7) = 0;
    *(u8*)(p + 0x1A8) = 0;
    *(u8*)(p + 0x1A9) = 0;
    *(u8*)(p + 0x1AA) = 0;
    *(u8*)(p + 0x1AB) = 0;
    *(u8*)(p + 0x1AC) = 0;
    *(u8*)(p + 0x1AD) = 0;
    *(s16*)(p + 0x1AE) = 0;
    *(s32*)(p + 0x1B0) = 0;
    *(s32*)(p + 0x1B4) = 0;
    *(s32*)(p + 0x1B8) = 0;
    *(u8*)(p + 0x1BC) = 0;
    *(s16*)(p + 0x1BE) = 0;
    *(s32*)(p + 0x1C0) = 0;
    *(s32*)(p + 0x1C4) = 0;
    *(s32*)(p + 0x1C8) = 0;
    *(s32*)(p + 0x1CC) = 0;
    *(s32*)(p + 0x1D0) = 0;
    *(s32*)(p + 0x1D4) = 0;
    *(u8*)(p + 0x1DA) = 0;
    *(s32*)(p + 0x1DC) = 0;
    *(s32*)(p + 0x1E0) = 0;
    *(s16*)(p + 0x1E4) = 0;
    *(s32*)(p + 0x1E8) = 0;
    *(s32*)(p + 0x1EC) = 0;
    *(s32*)(p + 0x1F0) = 0;
    *(f32*)(p + 0x1F4) = lbl_805A2AC8;
    *(f32*)(p + 0x1F8) = lbl_805A2AC8;
    *(f32*)(p + 0x1FC) = lbl_805A2AC8;
    *(f32*)(p + 0x200) = lbl_805A2ACC;
    *(f32*)(p + 0x204) = lbl_805A2AD0;
    *(f32*)(p + 0x208) = lbl_805A2AC8;
    *(f32*)(p + 0x20C) = lbl_805A2AC8;
    *(f32*)(p + 0x210) = lbl_805A2AC8;
    *(f32*)(p + 0x214) = lbl_805A2AC8;
    *(f32*)(p + 0x218) = lbl_805A2AC8;
    *(s32*)(p + 0x230) = 0;
    *(s32*)(p + 0x234) = 0;
    *(u8*)(p + 0x238) = 0;
    *(s32*)(p + 0x23C) = 0;
    *(void**)(p + 0x22C) = lbl_8045E7AC;
    *(s32*)(p + 0x240) = 0;
    *(s32*)(p + 0x244) = 0;
    *(s32*)(p + 0x248) = 0;
    *(s32*)(p + 0x24C) = 0;
    *(s32*)(p + 0x450) = 0;
    *(s32*)(p + 0x454) = 0;
    *(s32*)(p + 0x458) = 0;
    lbl_805A0390 = arg0;
    *(f32*)(p + 0x210) = lbl_805A2AD4;
    *(f32*)(p + 0x214) = lbl_805A2AD8;
    *(f32*)(p + 0x218) = lbl_805A2ADC;
    *(f32*)(p + 0x21C) = lbl_805A2AD8;
    *(f32*)(p + 0x220) = lbl_805A2AE0;
    *(f32*)(p + 0x224) = lbl_805A2AE4;
    *(f32*)(p + 0x228) = lbl_805A2ACC;

    i = 0;
    do {
        putPacketDelay__10NtMatchingFiUs(lbl_805A0434, i, 0);
        putPacketLost__10NtMatchingFiUc(lbl_805A0434, i, 0);
        i++;
    } while (i < 6);

    return arg0;
}
}

namespace jj_8013DDEC {

struct Sub {
    u8 unk0;
    u8 unk1;
    u32 unk4;
    u8 unk8;
    u8 unk9;
    u16 unkA;
    u32 unkC;
    u32 unk10;
    u32 unk14;
};

struct Elem {
    u32 unk0;
    Elem* next;
    u32 unk8;
    u8 unkC;
    u8 unkD;
    u16 unkE;
    u32 unk10;
    Sub subs[4];
    char* unk74;
};

struct Mgr {
    char pad0[0x14];
    u8 unk14;
    char pad15[0x20 - 0x15];
    Elem* elems;
    char pad24[0x48 - 0x24];
    char* buf48;
    char pad4C[0xA8 - 0x4C];
    u32 unkA8;
    char padAC[0x154 - 0xAC];
    u32 unk154;
    char pad158[0x160 - 0x158];
    void* unk160;
    s32 unk164;
    char pad168[0x450 - 0x168];
    u32 unk450;
    u32 unk454;
    Elem* unk458;
};

extern "C" Mgr* lbl_805A0390;

static inline void resetMgr(Mgr* g) {
    Elem* p;
    int i;
    Elem* e = g->elems;
    for (i = 0; i < 20; i++) {
        char* q = g->buf48 + g->unk164 * i;
        e->unk0 = 0;
        e->next = 0;
        e->unk8 = 0;
        e->unkC = 0;
        e->unkD = 0;
        e->unkE = 0;
        for (u8 j = 0; j < 4; j++) {
            e->subs[j].unk0 = 0;
            e->subs[j].unk1 = 0;
            e->subs[j].unk8 = 0;
            e->subs[j].unk9 = 0;
            e->subs[j].unk4 = 0;
            e->subs[j].unkA = 0;
            e->subs[j].unkC = 0;
            e->subs[j].unk10 = 0;
        }
        e->unk74 = q;
        e++;
    }
    g->unk450 = 0;
    int j = 0;
    g->unk454 = 0;
    g->unk14 = 0;
    p = g->elems;
    g->unk458 = p;
    for (; j < 20; j++, p++) {
        if (j == 19) {
            p->next = 0;
        } else {
            p->next = p + 1;
        }
        p->unk8 = 0;
    }
    memset(g->unk160, 0, g->unk154);
}

// TODO: clean up, give variables real names
extern "C" void fn_8013DDEC(void) {
    lbl_805A0390->unkA8 = 0;
    resetMgr(lbl_805A0390);
}
}

namespace jj_8013FB0C {

struct Sub {
    u8 unk0;
    u8 unk1;
    u32 unk4;
    u8 unk8;
    u8 unk9;
    u16 unkA;
    u32 unkC;
    u32 unk10;
    u32 unk14;
};

struct Elem {
    u32 unk0;
    Elem* next;
    u32 unk8;
    u8 unkC;
    u8 unkD;
    u16 unkE;
    u32 unk10;
    Sub subs[4];
    char* unk74;
};

struct Mgr {
    char pad0[0x14];
    u8 unk14;
    char pad15[0x20 - 0x15];
    Elem* elems;
    char pad24[0x48 - 0x24];
    char* buf48;
    char pad4C[0xA8 - 0x4C];
    u32 unkA8;
    char padAC[0x154 - 0xAC];
    u32 unk154;
    char pad158[0x160 - 0x158];
    void* unk160;
    s32 unk164;
    char pad168[0x450 - 0x168];
    u32 unk450;
    u32 unk454;
    Elem* unk458;
};

extern "C" Mgr* lbl_805A0390;

static inline void resetMgr(Mgr* g) {
    Elem* e = g->elems;
    int i;
    for (i = 0; i < 20; i++) {
        char* p = g->buf48 + g->unk164 * i;
        e->unk0 = 0;
        e->next = 0;
        e->unk8 = 0;
        e->unkC = 0;
        e->unkD = 0;
        e->unkE = 0;
        for (u8 j = 0; j < 4; j++) {
            e->subs[j].unk0 = 0;
            e->subs[j].unk1 = 0;
            e->subs[j].unk8 = 0;
            e->subs[j].unk9 = 0;
            e->subs[j].unk4 = 0;
            e->subs[j].unkA = 0;
            e->subs[j].unkC = 0;
            e->subs[j].unk10 = 0;
        }
        e->unk74 = p;
        e++;
    }
    g->unk450 = 0;
    g->unk454 = 0;
    g->unk14 = 0;
    g->unk458 = g->elems;
    e = g->unk458;
    for (i = 0; i < 20; i++, e++) {
        if (i == 19) {
            e->next = 0;
        } else {
            e->next = e + 1;
        }
        e->unk8 = 0;
    }
    memset(g->unk160, 0, g->unk154);
}

// TODO: clean up, give variables real names
extern "C" void fn_8013FB0C(void) {
    lbl_805A0390->unkA8 = 0;
    resetMgr(lbl_805A0390);
}
}

namespace jj_8013FDD4 {

extern "C" {
extern "C" unsigned char lbl_8059E742;
extern "C" unsigned char* lbl_805A0434;
extern "C" const float lbl_805A2B10;
extern "C" const float lbl_805A2AC8;
unsigned char fn_800CB194(muWifiInterfaceTask*, int);
int getAssignedPlayerKind__19muWifiInterfaceTaskFi(muWifiInterfaceTask*, int);
int getLocalPlayerContNum__19muWifiInterfaceTaskFi(muWifiInterfaceTask*, int);
}

struct Entry {
    signed char active;
    char b1, b2, b3;
    short b4;
    char pad6[2];
    float b8, bc;
    char pad10[8];
    unsigned int tick;
    char pad1c[4];
};
struct State {
    char pad0[0x11];
    unsigned char count, mask;
    char pad13[0x9d];
    Entry entries[4];
    char pad130[0x69];
    unsigned char n;
    unsigned char ids[3];
    unsigned char ranks[4];
    unsigned char order[4];
    char pad1a5[0x17];
    char flag;
    char pad1bd[0x13];
    int value;
    int mode;
};

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" int fn_8013FDD4(State* self) {
    unsigned int offset;
    Entry* e;
    unsigned char bits;
    signed char* active;
    unsigned int i;
    self->flag = 0;
    self->count = 0;
    bits = lbl_805A0434[0x34a9];
    active = &self->entries[0].active;
    for (i = 0; i < 4; ++i) {
        offset = (unsigned char)i * 32;
        e = (Entry*)((char*)self + offset + 0xb0);
        e->active = 0;
        e->b4 = 0;
        e->b1 = 0;
        e->b3 = 0;
        e->b2 = 0;
        e->tick = OSGetTick();
        e->b8 = lbl_805A2B10;
        e->bc = lbl_805A2AC8;
        if (bits & (1 << (unsigned char)i)) {
            self->count++;
            active[offset] = 1;
        }
    }
    self->count = 0;
    self->mask = 0;
    for (unsigned char i = 0; i < 4; i++) {
        if (self->entries[i].active) {
            self->mask |= 1 << i;
            self->count++;
        }
    }
    muWifiInterfaceTask* wifi = g_muWifiInterfaceTask;
    self->n = 0;
    memset(self->ids, 0, 3);
    memset(self->ranks, 0xff, 4);
    for (int i = 0; i < 4; i++) {
        if (lbl_8059E742 == fn_800CB194(wifi, i) && getAssignedPlayerKind__19muWifiInterfaceTaskFi(wifi, i) == 0) {
            self->ids[self->n] = i;
            self->ranks[self->n] = getLocalPlayerContNum__19muWifiInterfaceTaskFi(wifi, i) - 1;
            self->n++;
        }
    }
    memset(self->order, 0xff, 4);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < self->n; j++) {
            if (self->ranks[j] == i) self->order[i] = j;
        }
    }
    self->mode = 0x13;
    self->value = 0;
    lbl_805A0434[0x33ce] = 1;
    return 1;
}
}

namespace jj_801419FC {
struct NetEntry {
    u8 flagA;
    u8 flagB;
    u8 pad[0x12];
    u32 tick;
};

struct NetElem {
    u32 state;
    NetElem* next;
    NetElem* prev;
    u8 unkC;
    u8 unkD;
    u16 seq;
    u8 unk10[4];
    NetEntry entries[4];
    u8* buf;
};

struct NetCtx {
    u8 pad0[0x14];
    u8 count;
    u8 pad15[0x50 - 0x15];
    u32 maxSize;
    u32 stride;
    u8 pad58[0x142 - 0x58];
    u16 seq;
    u8 pad144[0x164 - 0x144];
    u32 memSize;
    u8 pad168[0x1BC - 0x168];
    u8 flag1BC;
    u8 pad1BD[0x1D0 - 0x1BD];
    s32 delay;
    u8 pad1D4[0x450 - 0x1D4];
    NetElem* listHead;
    NetElem* listTail;
    NetElem* freeHead;
};

extern "C" {
u32 OSGetTick(void);
void fn_80141D28(NetCtx* self, u16 seq, u8* buf, NetEntry* entry);
void netReport__FPCce(const char* fmt, ...);
void* memcpy(void*, const void*, unsigned long);
void* memset(void*, int, unsigned long);
}

extern "C" char lbl_8045E3BC[];
extern "C" char lbl_8045E3D8[];
extern "C" u8 lbl_8059E742;
extern "C" u8 lbl_805A0418[8];
extern void (*lbl_805A0398)(s32*, u32*, u16);

// TODO: clean up, give variables real names
extern "C" void fn_801419FC(void* arg0) {
    NetElem* elem;
    NetCtx* self;
    NetEntry* entry;
    s32 spC;
    u32 sp8;
    u16 seq;
    u8 i;
    u8* p;

    self = (NetCtx*)arg0;
    if (self->delay > 0) {
        self->delay--;
        return;
    }
    elem = self->freeHead;
    if (elem == NULL) {
        elem = NULL;
    } else {
        self->freeHead = elem->next;
        elem->next = NULL;
        elem->prev = NULL;
        self->count++;
        if (self->listHead != NULL) {
            self->listHead->prev = elem;
            elem->next = self->listHead;
        }
        self->listHead = elem;
        if (self->listTail == NULL) {
            self->listTail = elem;
        }
    }
    if (elem != NULL) {
        memset(elem->buf + self->stride * lbl_805A0418[0], 0, self->memSize);
        if (self->flag1BC == 0 && lbl_805A0398 != NULL) {
            lbl_805A0398(&spC, &sp8, self->seq);
            if (spC == 0) {
                netReport__FPCce(lbl_8045E3BC, self->seq);
                if (elem->prev != NULL) {
                    elem->prev->next = elem->next;
                }
                if (elem->next != NULL) {
                    elem->next->prev = elem->prev;
                }
                if (self->listTail == elem) {
                    self->listTail = elem->prev;
                }
                if (self->listHead == elem) {
                    self->listHead = elem->next;
                }
                elem->state = 0;
                elem->next = self->freeHead;
                elem->prev = NULL;
                self->freeHead = elem;
                if (self->count != 0) {
                    self->count--;
                }
                return;
            }
            if (sp8 > self->maxSize) {
                netReport__FPCce(lbl_8045E3D8, sp8, self->maxSize);
            }
            memcpy(elem->buf + self->stride * lbl_805A0418[lbl_8059E742 * 2], (void*)spC, sp8);
        }
        entry = &elem->entries[lbl_8059E742];
        entry->tick = OSGetTick();
        seq = self->seq;
        elem->seq = seq;
        self->seq = seq + 1;
        elem->state = 2;
        elem->unkD = 0;
        for (i = 0; i < 4; i++) {
            p = (u8*)elem + i * 0x18;
            p[0x14] = 0;
            p[0x15] = 0;
            if (i == lbl_8059E742) {
                elem->entries[lbl_8059E742].flagA = 1;
                elem->entries[lbl_8059E742].flagB = 1;
            }
        }
        fn_80141D28(self, elem->seq, elem->unk10, entry);
    }
}
}

namespace jj_8013F658 {
struct Sub { u8 unk0; u8 unk1; u32 unk4; u8 unk8; u8 unk9; u16 unkA; u32 unkC; u32 unk10; u32 unk14; };
struct Elem { u32 unk0; Elem* next; u32 unk8; u8 unkC; u8 unkD; u16 unkE; u32 unk10; Sub subs[4]; char* unk74; };
struct Mgr {
 char pad0[0x14]; u8 unk14; char pad15[0x20-0x15]; Elem* elems;
 char pad24[0x48-0x24]; char* buf48; char pad4C[0xA8-0x4C]; u32 unkA8;
 char padAC[0x154-0xAC]; u32 unk154; char pad158[0x160-0x158]; void* unk160; s32 unk164;
 char pad168[0x450-0x168]; u32 unk450; u32 unk454; Elem* unk458;
};
extern "C" Mgr* lbl_805A0390;
extern "C" int g_muWifiInterfaceTask;
extern "C" void clearPrepareLoadStage__19muWifiInterfaceTaskFv(int);
extern "C" void setSource__8ipSwitchFi(void*, int);
extern "C" void* lbl_805A0428;
static inline void resetMgr(Mgr* g) {
 int i;
 Elem* e = g->elems;
 for (i = 0; i < 20; i++) {
  char* p = g->buf48 + g->unk164 * i;
  e->unk0 = 0; e->next = 0; e->unk8 = 0; e->unkC = 0; e->unkD = 0; e->unkE = 0;
  for (u8 j = 0; j < 4; j++) {
   e->subs[j].unk0 = 0; e->subs[j].unk1 = 0; e->subs[j].unk8 = 0; e->subs[j].unk9 = 0;
   e->subs[j].unk4 = 0; e->subs[j].unkA = 0; e->subs[j].unkC = 0; e->subs[j].unk10 = 0;
  }
  e->unk74 = p; e++;
 }
 g->unk450 = 0; g->unk454 = 0; g->unk14 = 0; g->unk458 = g->elems;
 { Elem* p = g->unk458;
 for (int j = 0; j < 20; j++, p++) {
  if (j == 19) p->next = 0; else p->next = p + 1;
  p->unk8 = 0;
 } }
 memset(g->unk160, 0, g->unk154);
}
// TODO: clean up, give variables real names
extern "C" void fn_8013F658() {
 clearPrepareLoadStage__19muWifiInterfaceTaskFv(g_muWifiInterfaceTask);
 lbl_805A0390->unkA8 = 0;
 resetMgr(lbl_805A0390);
 ipSwitch* sw = ipSwitch::getInstance();
 if (*(int*)sw == 1) {
  setSource__8ipSwitchFi(sw, 0);
  if (lbl_805A0428 != 0) {
   ((u8*)lbl_805A0428)[8] = 0;
   gfCallBack* cb = (gfCallBack*)lbl_805A0428;
   cb->userProc(); cb = cb->m_next;
   while (cb != 0) { cb->userProc(); cb = cb->m_next; }
  }
 }
}
}

namespace jj_801400EC {

struct Sub {
    u8 a, b;
    u32 c;
    u8 d, e;
    u16 f;
    u32 g, h, i;
};
struct Elem {
    u32 a;
    Elem* next;
    Elem* link;
    u8 b, c;
    u16 d;
    u32 e;
    Sub sub[4];
    char* buf;
};
struct State {
    char pad0[0x14];
    u8 flag;
    char pad15[0xb];
    Elem* elems;
    char pad24[0x24];
    char* buf;
    char pad4c[0x5c];
    int state;
    char padac[0xa0];
    u32 timer;
    char pad150[4];
    u32 size;
    char pad158[8];
    void* data;
    int stride;
    char pad168[0x3f];
    s8 result;
    char pad1a8[0x2a8];
    u32 x450;
    Elem* list;
    Elem* freeList;
};
extern "C" void fn_80142EB0(State*);
static inline void reset(State* self) {
        Elem* e = self->elems;
        int i;
        for (i = 0; i < 20; i++) {
            char* p = self->buf + self->stride * i;
            e->a = 0; e->next = 0; e->link = 0;
            e->b = 0; e->c = 0; e->d = 0;
            for (u8 j = 0; j < 4; j++) {
                e->sub[j].a = 0; e->sub[j].b = 0;
                e->sub[j].d = 0; e->sub[j].e = 0;
                e->sub[j].c = 0; e->sub[j].f = 0;
                e->sub[j].g = 0; e->sub[j].h = 0;
            }
            e->buf = p;
            e++;
        }
        self->x450 = 0; self->list = 0; self->flag = 0;
        self->freeList = self->elems;
        e = self->freeList;
        for (i = 0; i < 20; i++, e++) {
            if (i == 19) e->next = 0;
            else e->next = e + 1;
            e->link = 0;
        }
        memset(self->data, 0, self->size);
}


// TODO: clean up, give variables real names
extern "C" s32 fn_801400EC(State* self) {
    self->result = 0;
    if (g_NtMatchingStatus != 9) return 0;
    switch (self->state) {
    case 0:
        reset(self);
        self->state = 1;
        break;
    case 1:
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        self->timer = 0;
    case 6:
        self->timer++;
        fn_80142EB0(self);
        break;
    }
    return self->result != 0;
}
}

namespace jj_80141EBC {

struct NetEntry {
    u8 flag;
    u8 pad1;
    u8 mode;
    u8 pad3;
    u16 unk4;
    u8 pad6[0x14 - 0x6];
    u32 tick;
    u32 stamp;
    u16 unk1C;
    u16 pad1E;
};

extern "C" {
void netReport__FPCce(const char*, ...);
u32 OSGetTick(void);
void syncCallback__15gfFileIOManagerFUcUs(s32, u16);
void netMinReport__FPCce(const char*, ...);
void setErrorStatus__10NtMatchingFl(int);
void closeAID__4NtTrFUc(s32, int);
void* fn_801410CC(void*, int);
extern "C" char lbl_8045E018[];
extern "C" u8 lbl_8059E742;
extern "C" void* lbl_805A0390;
extern "C" s32 lbl_805A03A8;
extern "C" u8 lbl_805A0418[8];
extern "C" const f32 lbl_805A2ADC;
extern "C" const f32 lbl_805A2AEC;
}

// TODO: clean up, give variables real names
inline void update(unsigned char* self) {
    self[0x11] = 0;
    self[0x12] = 0;
    for (unsigned char j = 0; j < 4; j++) {
        if (*(signed char*)(self + 0xB0 + j * 0x20) != 0) {
            self[0x12] |= 1 << j;
            self[0x11]++;
        }
    }
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" s32 fn_80141EBC(void* arg0) {
    char* base = lbl_8045E018;
    u8* self = (u8*)arg0;
    u8* head;
    u32 tick = OSGetTick();

    if (*(void**)(self + 0x454) == NULL) {
        return -1;
    }

    s32 r22 = *(s32*)(self + 0x50) + 3;
    NetEntry* e;
    u8 i;

    for (i = 0, e = (NetEntry*)(self + 0xB0); i < 4; i++, e++) {
        if (i == lbl_8059E742) continue;
        if ((s8)e->flag == 0) continue;

        u32 ms;
        s32 elapsed;
        elapsed = (s32)tick - (s32)e->stamp;
        ms = *(u32*)0x800000F8 / 4 / 1000;

        if (elapsed > (s32)(ms * 20000)) {
            netReport__FPCce(base + 0x440, (u32)elapsed / ms, 0x4E20, i);

            u32 k;
            for (k = 0; k < 4; k++) {
                netReport__FPCce(
                    base + 0x46C,
                    tick,
                    *(u32*)(self + (u8)k * 0x20 + 0xC8),
                    *(u8*)(self + (u8)k * 0x20 + 0xB2));
            }

            *(u8*)(self + (u8)i * 0x20 + 0xB0) = 0;
            update(self);

            closeAID__4NtTrFUc(lbl_805A03A8, i);

            s32 j;
            u8* p;
            u32 r18m;
            u32 b19;
            b19 = 1u << i;
            r18m = *(u8*)(self + 0x12) & ~b19;
            p = (u8*)(self + 0x250);

            for (j = 0; j < 0x40; j++, p += 8) {
                if (p[0] != 0) {
                    p[0] = (u8)(p[0] | b19);
                    if (r18m == (r18m & p[0])) {
                        netReport__FPCce(
                            base + 0x4A0, *(u16*)(p + 2), p[1], 0x22B8, i);
                        syncCallback__15gfFileIOManagerFUcUs(0, *(u16*)(p + 2));
                        p[0] = 0;
                        p[1] = 0;
                        *(u16*)(p + 2) = 0;
                        *(u32*)(p + 4) = 0;
                    }
                }
            }

            netMinReport__FPCce(base + 0x4D8);
            setErrorStatus__10NtMatchingFl(10);
        } else {
            *(u32*)(self + 0x170) = 0;
            u8* r19 = *(u8**)(self + 0x178);
            r19[0] = 8;
            r19[1] = 0xF8;
            r19[2] = (u8)((r19[2] & 3) | 0x14);

            u16 r16 = *(u16*)(self + 0x148);
            u8* p = (u8*)fn_801410CC(arg0, i);
            if (p != NULL) {
                r16 = *(u16*)(p + 0xE);
            }

            r19[8] = (u8)(r16 >> 8);
            r19[9] = (u8)r16;

            f32 f1 = (f32)((s32)e->unk1C - (s32)r16);
            u32 q1 = *(u32*)((char*)lbl_805A0390 + 0x1B4) /
                     (*(u32*)0x800000F8 / 4 / 1000);
            f32 val = f1 + (lbl_805A2ADC + ((f32)q1 / lbl_805A2AEC));
            *(f32*)((char*)lbl_805A0390 + 0x20C) = (f32)(s16)val;

            u8* q;
            if (p == NULL) goto clearTime;
            q = p + i * 0x18;
            if (*(u32*)(q + 0x20) == 0) goto clearTime;
            {
                u32 v = *(u32*)(q + 0x20) + (OSGetTick() - *(u32*)(q + 0x24));
                r19[4] = (u8)(v >> 24);
                r19[5] = (u8)(v >> 16);
                r19[6] = (u8)(v >> 8);
                r19[7] = (u8)v;
            }
            goto timeReady;

        clearTime:
            r19[4] = 0;
            r19[5] = 0;
            r19[6] = 0;
            r19[7] = 0;

        timeReady:
            ;

            u32 r18 = 0;
            u8* r17 = r19 + 0xD;
            head = *(u8**)(self + 0x454);
            u32 off = i * 0x18;

            while (head != NULL) {
                if (head[off + 0x15] == 0) break;
                head = *(u8**)(head + 8);
            }

            if (head != NULL) {
                s32 h = *(u16*)(head + 0xE);
                r19[0xA] = (u8)(h >> 8);
                r19[0xB] = (u8)h;

                u32 n = 0x10;
                while (head != NULL && n != 0) {
                    r17[0] = head[0x10];
                    r17[1] = head[0x11];
                    r17[2] = head[0x12];

                    memcpy(
                        r17 + 3,
                        *(u8**)(head + 0x74) +
                            (*(u32*)(self + 0x54)) *
                                lbl_805A0418[2 * lbl_8059E742],
                        *(u32*)(self + 0x50));

                    e->unk4 = 0;
                    r17 += r22;
                    n--;
                    r18++;
                    e->mode = 1;
                    e->tick = tick;
                    *(u32*)head = 3;
                    head[0xD] = 1;
                    head = *(u8**)(head + 8);
                }
            }

            u8* ch;
            s32 sz;
            r19[0xC] = (u8)r18;
            sz = (s32)(r22 * r18) + 0xD;

            if (sz >= 0x400) {
                netReport__FPCce(base + 0x4F0);
            } else {
                r19[2] = (u8)((r19[2] & 0xFC) | (((u32)sz >> 8) & 3));
                r19[3] = (u8)sz;

                ch = self + i * 0x14;
                void* src = *(void**)(self + 0x178);
                if ((s32)(*(u32*)(ch + 0x5C) - *(u32*)(ch + 0x60)) >= sz) {
                    memcpy(
                        *(u8**)(ch + 0x68) + *(u32*)(ch + 0x60),
                        src,
                        sz);
                    *(u32*)(ch + 0x60) = *(u32*)(ch + 0x60) + sz;
                }
            }
        }
    }

    return 0;
}
}

namespace jj_80142EB0 {

struct gfCameraController_base { char pad[4]; };
class gfCameraController : public gfCameraController_base {
public:
    virtual void slot0();
    u8 active;
};

struct Sub {
    u8 a, b;
    u32 c;
    u8 d, e;
    u16 f;
    u32 g, h, i;
};
struct Elem {
    u32 a;
    Elem* next;
    Elem* link;
    u8 b, c;
    u16 d;
    u32 e;
    Sub sub[4];
    char* buf;
};
struct Peer { s8 a; char pad[2]; s8 b; char rest[28]; };
struct Mgr {
    char pad0[0x14];
    u8 flag;
    char pad15[0xb];
    Elem* elems;
    char pad24[0x24];
    char* buf;
    char pad4c[0x5c];
    int state;
    char padac[4];
    Peer peers[4];
    char pad130[0x24];
    u32 size;
    char pad158[8];
    void* data;
    int stride;
    char pad168[0x2e8];
    u32 x450;
    Elem* list;
    Elem* freeList;
};
struct State {
    char pad0[0x60]; int x60; char pad64[0x10]; int x74; char pad78[0x10];
    int x88; char pad8c[0x10]; int x9c; char pada0[8];
    int state; char padac[0x94]; u16 port; char pad142[2]; u16 port2;
    char pad146[0x63]; u8 tick; char pad1aa[0xa]; u32 time;
    char pad1b8[6]; u16 code; char pad1c0[0x18];
    bool flag : 1;
    signed mode : 8;
    unsigned : 7;
    u8 reason;
    u8 pad1db;
    int timer, x1e0, x1e4, error;
};
extern "C" Mgr* lbl_805A0390;
extern "C" gfCameraController* lbl_805A0428;
extern "C" int lbl_805A03E8;
extern "C" u8 lbl_805A0409, lbl_8059E742;
extern "C" char lbl_8045E018[], g_Network[];
extern "C" void* g_muWifiInterfaceTask;
extern "C" const float lbl_805A2B10;
extern void (*lbl_805A039C)(u16,u8);
extern void (*lbl_805A03A0)(u8,u16,u8);
extern "C" {
void netReport__FPCce(const char*, ...);
void netMinReport__FPCce(const char*, ...);
void fn_8013E470(State*, u16, int);
void fn_80140EBC(State*, int);
int fn_80141814(State*);
void initPlayerLive__19muWifiInterfaceTaskFv(void*);
void fn_800CEDDC(void*);
void fn_801416CC(State*);
void fn_801424F8(State*);
int fn_80141474(State*, Elem*);
void fn_801419FC(State*);
void fn_80141EBC(State*);
void fn_8013E0B4(State*);
void clearPrepareLoadStage__19muWifiInterfaceTaskFv(void*);
void setSource__8ipSwitchFi(void*, int);
void disconnectAll__7NetworkFb(void*, int);
void fn_80142A80(State*);
void fn_8014044C(State*);
}
static inline void resetMgr(Mgr* g) {
    int i;
    Elem* e = g->elems;
    for (i = 0; i < 20; i++) {
        char* p = g->buf + g->stride * i;
        e->a = 0; e->next = 0; e->link = 0;
        e->b = 0; e->c = 0; e->d = 0;
        for (u8 j = 0; j < 4; j++) {
            e->sub[j].a = 0; e->sub[j].b = 0;
            e->sub[j].d = 0; e->sub[j].e = 0;
            e->sub[j].c = 0; e->sub[j].f = 0;
            e->sub[j].g = 0; e->sub[j].h = 0;
        }
        e->buf = p;
        e++;
    }
    g->x450 = 0; g->list = 0; g->flag = 0;
    g->freeList = g->elems;
    e = g->freeList;
    for (i = 0; i < 20; i++, e++) {
        if (i == 19) e->next = 0;
        else e->next = e + 1;
        e->link = 0;
    }
    memset(g->data, 0, g->size);
}
// TODO: clean up, give variables real names
static inline void notifyCamera(int v) {
    if (lbl_805A0428) {
        lbl_805A0428->active = v;
        gfCameraController* p = lbl_805A0428;
        p->slot0();
        p = *(gfCameraController**)p;
        while (p) {
            p->slot0();
            p = *(gfCameraController**)p;
        }
    }
}
// TODO: clean up, give variables real names
static inline void clearMgr() {
    clearPrepareLoadStage__19muWifiInterfaceTaskFv(g_muWifiInterfaceTask);
    lbl_805A0390->state = 0;
    resetMgr(lbl_805A0390);
    ipSwitch* s = ipSwitch::getInstance();
    if (*(int*)s == 1) {
        setSource__8ipSwitchFi(s, 0);
        notifyCamera(0);
    }
}
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_80142EB0(State* self) {
    char* base = lbl_8045E018;
    if (gfErrorManager::getInstance()->m_isSuspended && self->error == 0) {
        self->error = 14;
        if (self->state == 6 && self->x1e0 == 0 && self->mode == 0) {
            self->flag = 1;
            self->timer = 80;
            self->mode = 2;
            self->reason = 100;
        }
        netReport__FPCce(base + 0x6e4);
    }
    if (lbl_805A03E8 == 0) {
        switch (self->state) {
        case 2:
            self->port2 = self->port + 1000;
            fn_8013E470(self, self->port2, 1);
            self->time = (u32)(lbl_805A2B10 * (float)((*(u32*)0x800000f8 / 4) / 1000));
            fn_80140EBC(self, 0);
            self->state = 3;
            break;
        case 3:
            if (fn_80141814(self)) self->state = 5;
            break;
        case 5: {
            fn_8013E470(self, self->port2, 0);
            notifyCamera(1);
            void* p = g_muWifiInterfaceTask;
            initPlayerLive__19muWifiInterfaceTaskFv(p);
            fn_800CEDDC(p);
            netReport__FPCce(base + 0x710);
            self->state = 6;
            break;
        }
        case 6: {
            fn_801416CC(self);
            fn_801424F8(self);
            Elem* e = lbl_805A0390->list;
            while (e) {
                if (!fn_80141474(self, e)) break;
                e = e->link;
            }
            fn_801419FC(self);
            self->tick += lbl_805A0409;
            if (self->tick >= 60) {
                self->tick -= 60;
                fn_80141EBC(self);
            }
            fn_8013E0B4(self);
            if (self->mode >= 1 && self->timer != 0) {
                --self->timer;
                if (self->mode == 1) {
                    if (self->timer == 0) {
                        netReport__FPCce(base + 0x730, self->reason);
                        if (!self->flag) {
                            if (lbl_805A039C) lbl_805A039C(self->code, self->reason);
                            clearMgr();
                        } else {
                            netReport__FPCce(base + 0x74c);
                            goto disconnect;
                        }
                    }
                } else {
                    int count = 0;
                    for (unsigned i = 0; i < 4; i++) {
                        if (i != lbl_8059E742 && lbl_805A0390->peers[i].a && !lbl_805A0390->peers[i].b)
                            count++;
                    }
                    if (self->timer == 0 || count == 0) {
                    disconnect:
                        if (lbl_805A03A0) lbl_805A03A0(lbl_8059E742, self->code, self->reason);
                        clearMgr();
                        disconnectAll__7NetworkFb(g_Network, 0);
                        if (self->error) netMinReport__FPCce(base + 0x768, self->error);
                    }
                }
            }
            break;
        }
        }
        fn_80142A80(self);
        self->x60 = 0; self->x74 = 0; self->x88 = 0; self->x9c = 0;
        fn_8014044C(self);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_8013E380 {
struct Bits { unsigned int flag:1; unsigned int field:8; unsigned int rest:23; };
struct ByteBits { unsigned char flag:1; };
extern "C" int ntRequestExit__8NtSharedFP15NetworkUserDataUc(void* self, signed char arg) {
    char* p = (char*)self;
    if (*(int*)(p + 0xa8) != 6 || *(int*)(p + 0x1e0) != 0) return 0;
    if ((int)((*(unsigned int*)(p + 0x1d8) << 1) & 0xff800000) >> 24) return 0;
    ((ByteBits*)(p + 0x1d8))->flag = 1;
    *(int*)(p + 0x1dc) = 0x50;
    ((Bits*)(p + 0x1d8))->field = 2;
    *(signed char*)(p + 0x1da) = arg;
    return 1;
}
}

// TODO: clean up, give variables real names
namespace jj_8013E3E8 {
extern "C" {
    void netReport__FPCce(const char*, ...);
    void flushLogToSDCard__Fv();
    extern "C" char lbl_8045E018[];

    struct S {
        unsigned pad : 1;
        signed int code : 8;
        unsigned pad2 : 7;
        unsigned b1da : 8;
        unsigned pad3 : 8;
        int unk1dc;
    };

    int ntRequestStop__8NtSharedFP15NetworkUserDataUc(int* arg0, char arg1) {
        if (arg0[0x2A] != 6 || arg0[0x78] != 0) {
            return 0;
        }
        S* s = (S*)((char*)arg0 + 0x1D8);
        int temp = s->code;
        if (temp != 0) {
            netReport__FPCce(lbl_8045E018, temp);
            return 0;
        }
        s->code = 1;
        s->unk1dc = 0;
        ((char*)arg0)[0x1DA] = arg1;
        flushLogToSDCard__Fv();
        return 1;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_8013F0B0 {


class gfCameraController;

struct gfCameraController_base {
    gfCameraController* next;
};

class gfCameraController : public gfCameraController_base {
public:
    virtual void slot0();
    u8 active;
};

struct Sub {
    u8 a, b;
    u32 c;
    u8 d, e;
    u16 f;
    u32 g, h, i;
};

struct Elem {
    u32 a;
    Elem* next;
    Elem* link;
    u8 b, c;
    u16 d;
    u32 e;
    Sub sub[4];
    char* buf;
};

struct Mgr {
    char pad0[0x14];
    u8 flag;
    char pad15[0xb];
    Elem* elems;
    char pad24[0x24];
    char* buf;
    char pad4c[0x5c];
    int state;
    char padac[0xa8];
    u32 size;
    char pad158[8];
    void* data;
    int stride;
    char pad168[0x2e8];
    u32 x450;
    Elem* list;
    Elem* freeList;
};

struct State {
    char pad0[0x4c];
    int x4c;
    int x50;
    int x54;
    char pad58[0x50];
    int state;
    char padac[0x94];
    u16 port;
    char pad142[2];
    u16 port2;
    char pad146[0x86];
    int x1cc;
};

struct PeerB {
    u8 a, b;
};

extern "C" Mgr* lbl_805A0390;
extern "C" gfCameraController* lbl_805A0428;
extern "C" int lbl_805A03E8;
extern "C" u8 lbl_8059E742;
extern "C" PeerB lbl_805A0418[4];
extern "C" char lbl_8045E018[];
extern "C" void* g_muWifiInterfaceTask;
extern "C" int g_NtMatchingStatus;

extern "C" {
void netReport__FPCce(const char*, ...);
void netMinReport__FPCce(const char*, ...);
void clearPrepareLoadStage__19muWifiInterfaceTaskFv(void*);
void setSource__8ipSwitchFi(void*, int);
int getAssignedPlayerKind__19muWifiInterfaceTaskFi(void*, int);
u8 fn_800CB194(void*, int);
void setSIDPlayer__6NtBaseFUcUc(u8, u8);
int fn_8013FDD4(State*);
void setErrorStatus__10NtMatchingFl(int);
}

static inline void resetMgr(Mgr* g) {
    Elem* e = g->elems;
    int i;

    for (i = 0; i < 20; i++) {
        char* p = g->buf + g->stride * i;
        e->a = 0;
        e->next = 0;
        e->link = 0;
        e->b = 0;
        e->c = 0;
        e->d = 0;

        for (u8 j = 0; j < 4; j++) {
            e->sub[j].a = 0;
            e->sub[j].b = 0;
            e->sub[j].d = 0;
            e->sub[j].e = 0;
            e->sub[j].c = 0;
            e->sub[j].f = 0;
            e->sub[j].g = 0;
            e->sub[j].h = 0;
        }

        e->buf = p;
        e++;
    }

    g->x450 = 0;
    g->list = 0;
    g->flag = 0;
    g->freeList = g->elems;
    e = g->freeList;

    for (i = 0; i < 20; i++, e++) {
        if (i == 19)
            e->next = 0;
        else
            e->next = e + 1;
        e->link = 0;
    }

    memset(g->data, 0, g->size);
}

extern "C" int ntStartSync__8NtSharedFP15NetworkUserData(State* self) {
    u8 counts[4];
    char* base = lbl_8045E018;

    if (lbl_805A03E8 != 0) {
        clearPrepareLoadStage__19muWifiInterfaceTaskFv(g_muWifiInterfaceTask);
        lbl_805A0390->state = 0;
        resetMgr(lbl_805A0390);

        ipSwitch* s = ipSwitch::getInstance();
        if (*(int*)s == 1) {
            setSource__8ipSwitchFi(s, 0);
            if (lbl_805A0428) {
                lbl_805A0428->active = 0;
                gfCameraController_base* current;
                current = (gfCameraController_base*)lbl_805A0428;
                ((gfCameraController*)current)->slot0();
                gfCameraController* p = current->next;
                while (p) {
                    p->slot0();
                    p = p->next;
                }
            }
        }
        return 0;
    }

    memset(counts, 0, 4);
    for (int i = 0; i < 4; i++) {
        if (getAssignedPlayerKind__19muWifiInterfaceTaskFi(g_muWifiInterfaceTask, i) == 0)
            counts[fn_800CB194(g_muWifiInterfaceTask, i)]++;
    }

    int total = 0;
    for (u8 i = 0; i < 4; i++) {
        total += counts[i];
        setSIDPlayer__6NtBaseFUcUc(i, counts[i]);
    }

    if (total == 0) {
        netReport__FPCce(base + 0x4c);
        for (u8 i = 0; i < 4; i++)
            setSIDPlayer__6NtBaseFUcUc(i, 1);
    }

    if (fn_8013FDD4(self) == 0) {
        setErrorStatus__10NtMatchingFl(10);
        netReport__FPCce(base + 0x90);
        netMinReport__FPCce(base + 0xb0);
        return 0;
    }

    if (g_NtMatchingStatus != 9) {
        setErrorStatus__10NtMatchingFl(10);
        netReport__FPCce(base + 0xc0, g_NtMatchingStatus);
        netMinReport__FPCce(base + 0xfc);
        return 0;
    }

    if (self->state == 0 && self->state == 1) {
        setErrorStatus__10NtMatchingFl(10);
        netReport__FPCce(base + 0x10c, self->state);
        netMinReport__FPCce(base + 0x13c);
        return 0;
    }

    netReport__FPCce(base + 0x150);
    u8 m = 0;
    for (int i = 0; i < 4; i++) {
        if (m < lbl_805A0418[i].b)
            m = lbl_805A0418[i].b;
    }

    int unit = self->x54;
    self->x4c = m * unit;
    int v50 = unit * lbl_805A0418[lbl_8059E742].b;
    int port = (self->port2 / 1000) * 1000;
    self->state = 2;
    self->x1cc = 0;
    self->x50 = v50;
    self->port = port;
    return 1;
}
}

