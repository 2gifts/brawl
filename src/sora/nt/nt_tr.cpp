#include <string.h>
#include <nt/nt_send.h>
#include <revolution/OS/OSTime.h>
#include <mu/wifi/mu_wifi_interface.h>
#include <nt/nt_match.h>
#include <nt/nt_net_report.h>
#include <nt/nt_tr.h>
#include <types.h>

// HYPOTHESIS: the matching manager instance (the map leaves it unnamed; nt_match.cpp types it as its own view).
extern NtMatching* lbl_805A0434;

NtTr::NtTr() : unk47DC(0) {
    lbl_805A03A8 = this;
    unk20 = 0;
    unk24 = 0;
    unk28 = 0;
    unk2C = 0;
    unk3C = 0;
}

NtTr::~NtTr() {
    lbl_805A03A8 = 0;
}

void NtTr::closeAID(u8 aid) {
    unk44[aid].init();
    u32 mask = ~(1 << aid);
    unk1C &= mask;
    unk38 &= mask;
    unk4808 &= mask;
    lbl_805A0434->watchResetNodeMissMatch();
    g_muWifiInterfaceTask->closeAIDWifi(aid);
}

extern u32 g_NtAIDBitmap;
extern u8 lbl_8059E742;
extern char lbl_8045EE20[0x44];
extern char lbl_8045EEA4[0x2B];
extern char lbl_8045EED0[0x2EC];

bool NtTr::refSendCmd(u8 aid, void** command, u16 size, u16* sequence, u8 type) {
    __typeof__(unk44[0].unk1CData[0])* packet;
    u16 packetSize = size + 0x10;
    __typeof__(unk44[0])* queue = &unk44[aid];
    if (!(g_NtAIDBitmap & (1 << aid)))
        return false;
    if (3 - ((queue->unk18 + 4 - queue->unk14) % 4) < 1)
        packet = 0;
    else
        packet = &queue->unk1CData[queue->unk18];
    if (!packet) {
        netReport(lbl_8045EE20);
        return false;
    }
    packet->unk0 = 1;
    packet->unk4 = ++queue->unk10;
    if (sequence)
        *sequence = packet->unk4;
    packet->unk6 = 9;
    packet->unk7 = 0xF7;
    packet->unkC = packetSize;
    packet->unkA = type;
    packet->unkE = packet->unk4;
    *command = packet->unk16;
    return true;
}

bool NtTr::isSendQueueEmpty() {
    unsigned int aid;
    for (aid = 0; aid < 4; ++aid) {
        u8 destination = aid;
        if (aid != lbl_8059E742 && (g_NtAIDBitmap & (1 << destination))) {
            if (lbl_805A03A8->unk44[aid].unk14 != lbl_805A03A8->unk44[aid].unk18)
                break;
        }
    }
    return aid == 4;
}

void NtTr::broadcastMyShortComment() {
    unk3E_b6 = 1;
}

void NtTr::broadcastMyName() {
    unk3E_b5 = 1;
}

bool NtTr::ntTrRecvQue::stock(u8 aid, u8 type, const void* command, u32 size) {
    if (size > 0x384) {
        netReport(lbl_8045EEA4, size);
        return false;
    }
    __typeof__(unk10[0])* packet = static_cast<__typeof__(unk10[0])*>(unk8);
    unk8 = packet->unk4;
    if (!packet) {
        netReport(lbl_8045EED0, aid);
        return false;
    }
    packet->unk8Data = aid;
    packet->unk9 = type;
    memcpy(packet->unkA, command, size);
    if (unk0)
        static_cast<__typeof__(unk10[0])*>(unk0)->unk0 = packet;
    packet->unk4 = unk0;
    packet->unk0 = 0;
    unk0 = packet;
    if (!unk4)
        unk4 = packet;
    return true;
}

void NtTr::defineBroadcastArea(void* area, u32 size0, u32 size1, u32 size2) {
    unk20 = reinterpret_cast<u32>(area);
    unk24 = size0;
    unk28 = size1;
    unk2C = size2;
}

// TODO: clean up, give variables real names
extern "C" void fn_80143C44(void) {}

// TODO: clean up, give variables real names
namespace jj_8014403C {
extern "C" void* memset(void*, int, unsigned long);
extern "C" const float lbl_805A2B50;

struct Entry {
    unsigned char flags : 8;
    unsigned char a;
    unsigned char b;
    unsigned char pad47;
    float value;
    unsigned short c;
    char pad4e[6];
    unsigned short d;
    char pad56[2];
    unsigned int e;
    unsigned int f;
    char buffer[0xe30];
    char padEnd[4];
};

struct Flags {
    unsigned char a : 2;
    unsigned char b : 1;
    unsigned char c : 1;
    unsigned char d : 2;
    unsigned char e : 1;
    unsigned char f : 1;
};

struct Object {
    char pad0[0x18];
    unsigned int a;
    unsigned int b;
    char pad20[0x14];
    unsigned char c;
    char pad35[9];
    Flags flags;
    char pad3f[5];
    Entry entries[4];
};

extern "C" void fn_8014403C(Object* self) {
    float value = lbl_805A2B50;
    unsigned int i = 0;
    do {
        Entry* entry = &self->entries[(unsigned char)i];
        entry->flags = 0;
        entry->a = 0;
        entry->b = 0;
        entry->value = value;
        entry->c = 0;
        entry->d = 0;
        entry->f = 0;
        entry->e = 0;
        memset(entry->buffer, 0, 0xe30);
        ++i;
    } while (i < 4u);

    char* base = (char*)self;
    char* first = base + 0x3994;
    *(unsigned int*)(base + 0x3988) = 0;
    *(unsigned int*)(base + 0x3984) = 0;
    *(void**)(base + 0x398c) = first;
    *(void**)(base + 0x3998) = first + 0x390;
    *(void**)(base + 0x3d28) = first + 0x720;
    *(void**)(base + 0x40b8) = first + 0xab0;
    *(unsigned int*)(base + 0x4448) = 0;
    self->a = 0;
    self->c = 0;
    self->b = 0;
    self->flags.b = 0;
    self->flags.d = 0;
    self->flags.a = 0;
    self->flags.e = 0;
}
}

namespace jj_80143B48 {
extern "C" void* memset(void*, int, unsigned long);
extern "C" void fn_80143C48(void*, int);
extern "C" const float lbl_805A2B50;

struct Entry {
    unsigned char flags;
    unsigned char a, b, pad3;
    float value;
    unsigned short c;
    unsigned char padA[6];
    unsigned short d;
    unsigned short pad12;
    int e, f;
    unsigned char data[0xE30];
    unsigned char padE4C[4];
};

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_80143B48(void* arg) {
    float value = lbl_805A2B50;
    Entry* p = (Entry*)((char*)arg + 0x44);
    unsigned int i = 0;
    char* self = (char*)arg;

    do {
        p->f = 0;
        p->e = 0;
        memset(p->data, 0, 0xE30);
        p->flags &= ~0xFF;
        p->a = 0;
        p->b = 0;
        p->value = value;
        p->c = 0;
        p->d = 0;
        p->f = 0;
        p->e = 0;
        memset(p->data, 0, 0xE30);
        ++i;
        ++p;
    } while (i < 4);

    char* base = self + 0x3994;
    *(int*)(self + 0x3988) = 0;
    *(int*)(self + 0x3984) = 0;
    *(void**)(self + 0x398C) = base;
    *(void**)(self + 0x3998) = base + 0x390;
    *(void**)(self + 0x3D28) = base + 0x720;
    *(void**)(self + 0x40B8) = base + 0xAB0;
    *(int*)(self + 0x4448) = 0;
    fn_80143C48(self, 0);
}
}

namespace jj_8014615C {
extern "C" void netReport__FPCce(const char*, ...);
extern "C" void *memcpy(void *, const void *, unsigned long);
extern char lbl_8045E7E8[];

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" int fn_8014615C(char *base, int idx, char *data, int len) {
    char *s = base + idx * 0x18;
    char *msg = lbl_8045E7E8;
    unsigned int size;
    unsigned int write;

    if (*(int *)(s + 0x4810) != 1) {
        netReport__FPCce(msg + 0x878);
        return 0;
    }

    unsigned int buf = *(unsigned int *)(s + 0x4820);
    if (buf != 0) {
        netReport__FPCce(msg + 0x8A8);
        return 0;
    }

    write = *(unsigned int *)(s + 0x481C);
    size = *(unsigned int *)(s + 0x4814);

    if ((int)(size - write) < len) {
        netReport__FPCce(msg + 0x8D0, idx, size, write + len);
        *(s + 0x480C) = 0;
        *(int *)(s + 0x4810) = 0;
        return 0;
    }

    memcpy((void *)(buf + write), data + 0x20, len);
    *(unsigned int *)(s + 0x481C) += len;

    if ((int)*(unsigned int *)(s + 0x4814) == (int)*(unsigned int *)(s + 0x481C)) {
        *(s + 0x480C) = 0;
        *(int *)(s + 0x4810) = 0;
        netReport__FPCce(msg + 0x918, idx, *(unsigned int *)(s + 0x481C));
    }
    return 1;
}
}

namespace jj_80145B98 {

extern "C" void netReport__FPCce(const char*, ...);
extern "C" void* fn_800D9CB0(void* arg0);
extern "C" unsigned char lbl_8059E742;
extern "C" unsigned char lbl_8059E745;
extern "C" char lbl_8045EE64[];

struct NtRecv {
    char pad0[0x18];
    int state;
    unsigned int mask;
    void* buf;
    char pad24[0xC];
    unsigned int tick;
};

struct WifiEntry {
    char pad0[0xE8];
    unsigned char flag : 1;
    unsigned char rest : 7;
};

// TODO: clean up, give variables real names
extern "C" int fn_80145B98(NtRecv* self, unsigned int arg1) {
    unsigned char idx;
    unsigned char other;
    unsigned int flag;
    unsigned int task;
    unsigned int i;

    if (self->buf == 0) {
        netReport__FPCce(lbl_8045EE64);
        return 0;
    }
    if (arg1 == 0xFF) {
        return 0;
    }
    idx = lbl_8059E742;
    if (idx >= 4) {
        return 0;
    }
    other = lbl_8059E745;
    flag = 0;
    task = (unsigned int)g_muWifiInterfaceTask;
    if (idx == other && idx != 0xFF) {
        flag = 1;
    }
    ((WifiEntry*)(task + idx * 0x218))->flag = flag;
    if (arg1 == 0xFE) {
        self->mask |= g_NtAIDBitmap;
    } else if (arg1 == 0xFD) {
        i = 0;
        do {
            int valid;
            if ((unsigned char)i >= 4) {
                valid = 0;
            } else {
                valid = !(((unsigned char*)fn_800D9CB0((void*)task))[(unsigned char)i + 0x68] - 1);
            }
            if (valid) {
                self->mask |= (1u << (unsigned char)i);
            }
            i++;
        } while (i < 4);
    } else {
        self->mask |= g_NtAIDBitmap & (1 << arg1);
    }
    self->mask &= ~(1 << lbl_8059E742);
    self->state = 1;
    self->tick = OSGetTick();
    return 1;
}
}

// TODO: clean up, give variables real names
namespace jj_80143D80 {

extern "C" int fn_80145828(void* self, int idx, void** out, int a, int b, int c);
extern "C" void fn_8014BC24(int* p);
extern "C" void fn_800D2894(char* base, int idx);

extern "C" const float lbl_805A2B50;
extern "C" unsigned int g_NtAIDBitmap;
extern "C" int* lbl_805A0434;
extern "C" char* g_muWifiInterfaceTask;

union FlagByte {
    unsigned char flag;
    unsigned char bits : 8;
};

struct Entry {
    FlagByte fb;
    unsigned char b1;
    unsigned char b2;
    unsigned char pad0;
    float value;
    unsigned short w8;
    unsigned short pad1[3];
    unsigned short w10;
    unsigned short pad2;
    int w14;
    int w18;
    unsigned char data[0xE30];
    unsigned char tail[4];
};

struct Object {
    unsigned char prefix[0x44];
    Entry entries[4];
};

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_80143D80(unsigned char* self, int idx) {
    unsigned char* entry = NULL;
    Entry* e = &((Object*)self)->entries[idx];
    void* out1;
    void* out2;
    int mask;

    e->fb.bits = 0;
    e->b1 = 0;
    e->b2 = 0;
    e->value = lbl_805A2B50;
    e->w8 = 0;
    e->w10 = 0;
    e->w18 = 0;
    e->w14 = e->w18;
    memset(e->data, 0, sizeof(e->data));

    mask = 1;
    e->fb.bits = mask;
    e->w10 = (unsigned short)(OSGetTick() % 10000);
    mask <<= idx;

    if (g_NtAIDBitmap & mask) {
        if (fn_80145828(self, idx, &out1, 0, 0, 6) != 0) {
            if (3 - ((e->w18 + 4 - e->w14) % 4) < 1) {
                entry = NULL;
            } else {
                entry = &e->data[e->w18 * 0x38C];
            }
            if (entry != NULL) {
                e->w18 = (*(int*)((char*)e + 0x18) + 1) % 4;
            }
        }
    }

    if (g_NtAIDBitmap & mask) {
        if (fn_80145828(self, idx, &out2, 0, 0, 6) != 0) {
            unsigned char* p;
            if (3 - ((e->w18 + 4 - e->w14) % 4) < 1) {
                p = NULL;
            } else {
                p = &e->data[e->w18 * 0x38C];
            }
            if (p != NULL) {
                e->w18 = (1 + *(int*)((char*)e + 0x18)) % 4;
            }
        }
    }

    fn_8014BC24(lbl_805A0434);
    fn_800D2894(g_muWifiInterfaceTask, idx);
}
}

namespace jj_801454A4 {
typedef unsigned char u8;
typedef int s32;

extern "C" {
void OSLockMutex(void*);
void OSUnlockMutex(void*);
void netReport__FPCce(const char*, ...);
void fn_800C8F68(const char*, ...);
void fn_800D0E58(void*, u8, int, int);
u8 fn_800C8548();
int fn_800C8524();
void fn_800D1314(void*, u8);
void fn_800D55E8(void*);
void fn_80146878(void*, int);
extern char* lbl_805A03A8;
extern char* lbl_805A0434;
extern char* g_muWifiInterfaceTask;
extern char lbl_8045E7E8[];
extern unsigned char lbl_8049EC48[];
extern u8 lbl_8059E744;
extern u8 lbl_8059E745;
extern char g_Network[];
}

struct TopBit { u8 f : 1; u8 rest : 7; };
struct Bit4 { u8 hi : 3; u8 f : 1; u8 lo : 4; };
struct StateWord { int st : 8; int rest : 24; };

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_801454A4(u8 arg0, int arg1, int arg2) {
    char* base = lbl_8045E7E8;
    (lbl_805A03A8 + arg0 * 0xE50)[0x46] = 1;
    int len = arg2 - 0x10;
    int data = arg1 + 0x10;
    int expected = *(int*)(lbl_805A03A8 + 0x28);
    if (len != expected) {
        netReport__FPCce(base + 0x3D8, arg0, expected, len);
        fn_800C8F68(base + 0x414, arg0, expected, arg2 - 0x10);
        return;
    }
    expected = (int)g_muWifiInterfaceTask;
    OSLockMutex(lbl_8049EC48);
    fn_800D0E58((void*)expected, arg0, data, arg2 - 0x10);
    OSUnlockMutex(lbl_8049EC48);
    char* task = g_muWifiInterfaceTask;
    if (lbl_8059E745 != lbl_8059E744) {
        lbl_8059E745 = lbl_8059E744;
        netReport__FPCce(base + 0x43C, lbl_8059E744);
    }
    lbl_805A0434[0x33CE] = 1;
    char* e = task + arg0 * 0x218;
    if (((TopBit*)(e + 0x280))->f) return;
    ((TopBit*)(e + 0x280))->f = 1;
    if (((TopBit*)(task + 0xFC9))->f && *(long long*)(e + 0x248) == 0) {
        netReport__FPCce(base + 0x478);
    }
    if (arg0 == fn_800C8548()) {
        netReport__FPCce(base + 0x49C);
        task[0xFD3] = arg0;
        u8 j = fn_800C8548();
        char* q;
        if (j == 0xFF) {
            q = 0;
        } else {
            q = task + j * 0x218 + 0x70;
        }
        int flag;
        if (q != 0) {
            flag = ((Bit4*)(q + 0x78))->f;
        } else {
            flag = 1;
        }
        if (flag != 0) {
            netReport__FPCce(base + 0x4C4);
            fn_800C8F68(base + 0x510);
            if (((TopBit*)(task + 0xFC9))->f) {
                if (((StateWord*)(task + 0xFCC))->st < 4) {
                    ((StateWord*)(task + 0xFCC))->st = 5;
                }
            } else {
                fn_80146878(g_Network, 1);
            }
            return;
        }
    } else {
        if (fn_800C8524() != 0) {
            netReport__FPCce(base + 0x52C);
            fn_800D1314(task, arg0);
        } else {
            netReport__FPCce(base + 0x554);
        }
    }
    if ((u8)lbl_805A0434[0x33F6] == 1) {
        netReport__FPCce(base + 0x574);
        return;
    }
    if (((TopBit*)(task + 0xFC9))->f) {
        fn_800D55E8(task);
    }
}
}

namespace jj_80145D38 {

extern "C" int fn_80145828(void* self, int idx, void** out, int size, int a, int b);
extern "C" void netReport__FPCce(const char*, ...);
extern "C" unsigned int g_NtAIDBitmap;
extern "C" unsigned char lbl_8059E742;
extern "C" char lbl_8045EE80[];

struct Entry {
    unsigned char pad[0x14];
    int w14;
    int w18;
    unsigned char data[0xE50 - 0x1C];
};

struct Object {
    unsigned char pad0[0x18];
    int state;
    unsigned int mask;
    char* buf;
    int stride;
    int size;
    unsigned char pad2[0x44 - 0x2C];
    Entry entries[4];
};

extern "C" Object* lbl_805A03A8;

// TODO: clean up, give variables real names
static inline bool isPending(unsigned int aid) {
    Entry* e = &lbl_805A03A8->entries[aid];
    return e->w14 != e->w18;
}

// TODO: clean up, give variables real names
static inline unsigned int findBusy() {
    unsigned int i;
    for (i = 0; i < 4; i++) {
        unsigned char a = i;
        if (i != lbl_8059E742 && (g_NtAIDBitmap & (1 << a)) && isPending(i))
            break;
    }
    return i;
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
static inline int sendTo(Object* self, unsigned char aid, void* src, int size) {
    void* out;
    int ret;
    if (!(g_NtAIDBitmap & (1 << aid)))
        return 0;
    ret = fn_80145828(self, aid, &out, size, 0, 0xE);
    if (ret == 0)
        return ret;
    if (size != 0)
        memcpy(out, src, size);
    Entry* e = &self->entries[aid];
    unsigned char* p;
    if (3 - ((e->w18 + 4 - e->w14) % 4) < 1) {
        p = NULL;
    } else {
        p = &e->data[e->w18 * 0x38C];
    }
    if (p == NULL)
        return 0;
    e->w18 = (*(int*)((char*)e + 0x18) + 1) % 4;
    return 1;
}

// TODO: clean up, give variables real names
extern "C" void fn_80145D38(Object* self) {
    switch (self->state) {
    case 1: {
        if (findBusy() != 4)
            return;
        self->mask &= g_NtAIDBitmap;
        if (self->mask == 0) {
            self->state = 0;
            return;
        }
        char* src = self->buf + lbl_8059E742 * self->stride;
        unsigned char i;
        for (i = 0; i < 4; i++) {
            if ((g_NtAIDBitmap & (1 << i)) && (self->mask & (1 << i))) {
                if (sendTo(self, i, src, self->size))
                    netReport__FPCce(lbl_8045EE80, i);
            }
        }
        self->mask = 0;
        self->state = 2;
        break;
    }
    case 2:
        if (findBusy() == 4)
            self->state = 0;
        break;
    }
}
}

