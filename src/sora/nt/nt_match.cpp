#include <ip/ip_switch.h>
#include <revolution/OS/OSMutex.h>
#include <nt/nt_send.h>
#include <revolution/OS/OSThread.h>
#include <mu/wifi/mu_wifi_interface.h>
#include <nt/network.h>
#include <nt/nt_match.h>
#include <nt/nt_net_report.h>
#include <nt/nt_offline.h>
#include <nt/nt_report.h>
#include <revolution/OS/OSTime.h>
#include <string.h>
#include <types.h>
#include <gf/gf_heap_manager.h>

class gfMemoryPool {
public:
    static void* alloc(void* heapAddr, size_t size, int align);
    void free(void* ptr);
};

struct OSMutex;
extern "C" void OSLockMutex(OSMutex* mutex);
extern "C" void OSUnlockMutex(OSMutex* mutex);
extern OSMutex lbl_8049EC78;

extern NtMatching* lbl_805A0434;

void DWCAllocType::ntDwcFree(int type, void* memory, u32 size) {
    if (memory != 0) {
        OSLockMutex(&lbl_8049EC78);
        gfHeapManager::free(memory);
        OSUnlockMutex(&lbl_8049EC78);
    }
}

void* DWCAllocType::ntDwcAlloc(int type, u32 size, u32 alignment) {
    if (size != 0) {
        OSLockMutex(&lbl_8049EC78);
        void* memory = gfHeapManager::alloc(Heaps::Network, size);
        OSUnlockMutex(&lbl_8049EC78);
        return memory;
    }
    return nullptr;
}

void* DWCAllocType::ntAlloc(int arg0, u32 size, u32 arg2) {
    void* result;

    if (size != 0) {
        OSLockMutex(&lbl_8049EC78);
        result = gfMemoryPool::alloc(lbl_805A0434->unk353C, size, 0x20);
        OSUnlockMutex(&lbl_8049EC78);
        return result;
    }
    return nullptr;
}

void DWCAllocType::ntFree(int arg0, void* arg1) {
    if (arg1 != 0) {
        OSLockMutex(&lbl_8049EC78);
        reinterpret_cast<gfMemoryPool*>(lbl_805A0434->unk353C)->free(arg1);
        OSUnlockMutex(&lbl_8049EC78);
    }
}


void netReport0(const char* fmt, ...) {
}

extern "C" void DWC_CreateFriendKeyToken(UnkFriendInfo::FriendData* token, u64 friendKey);
extern "C" int DWC_IsEqualFriendData(const UnkFriendInfo::FriendData* data1, const UnkFriendInfo::FriendData* data2);
extern "C" int DWC_IsValidFriendData(const UnkFriendInfo::FriendData* data);
extern "C" int DWC_GetGsProfileId(const void* userData, const UnkFriendInfo::FriendData* friendData);
extern u8 lbl_805A03D0;
extern OSMutex lbl_8049EC30;
extern char lbl_8045FA50[];
extern char lbl_8045FC14[];
extern char lbl_8045F9F8[0x20];
extern char lbl_8045FA18[0x1C];
extern char lbl_8045FA34[0x1C];

bool NtMatching::deleteFriend(int index) {
    unk3421 = index;
    deleteDisplayOrder(index, true);
    unk3411 = true;
    unk3412 = true;
    return true;
}

void NtMatching::freeDisplayOrder() {
    memset(unk12, 0, sizeof(unk12));
}

void NtMatching::updateServerCallback(int error, int isChanged, void* param) {
    if (gUnk805a03d8 != 7 && error == 0) {
        netReport(lbl_8045F9F8);
        if (isChanged) {
            netReport(lbl_8045FA18);
            ntFriendInfo::getInstance()->setDirtyFlag(true);
        }
    }
}

void NtMatching::changetFriendStatusCallback(int index, u8 status, const char* statusString, void* param) {
    if (gUnk805a03d8 != 7) {
        netReport(lbl_8045FA34, index);
        ntFriendInfo::getInstance()->setDirtyFlag(true);
    }
}

void NtMatching::deleteFriendListCallback(int deletedIndex, int srcIndex, void* param) {
    while (lbl_805A03D0 && !lbl_805A0434->unk33CC) {
        OSSleepTicks(OS_MSEC_TO_TICKS(1));
    }
    if (gUnk805a03d8 != 7) {
        netReport(lbl_8045FA50, srcIndex, deletedIndex);
        OSLockMutex(&lbl_8049EC30);
        lbl_805A0434->deleteDisplayOrder(deletedIndex, false);
        ntFriendInfo::getInstance()->setDirtyFlag(true);
        OSUnlockMutex(&lbl_8049EC30);
    }
}

int NtMatching::getAvailableFriendListIndex(u64 friendKey) {
    UnkFriendInfo::FriendData token;
    DWC_CreateFriendKeyToken(&token, friendKey);
    UnkFriendInfo::FriendData* friends = ntFriendInfo::getInstance()->getFriendInfo()->m_friendData;
    int index;
    for (index = 0; index < 64; index++) {
        if (DWC_IsEqualFriendData(&friends[index], &token)) {
            break;
        }
    }
    if (index < 64) {
        return index;
    }
    for (index = 0; index < 64; index++) {
        if (lbl_805A0434->unk12[index] == 0 && !DWC_IsValidFriendData(&friends[index])) {
            break;
        }
    }
    if (index == 64) {
        return -1;
    }
    return index;
}

void NtMatching::udpRecvCallback(u8 aid, u8* buffer, int size) {
    while (lbl_805A03D0 && !lbl_805A0434->unk33CC) {
        OSSleepTicks(OS_MSEC_TO_TICKS(1));
    }
    OSLockMutex(&lbl_8049EC30);
    NtShared::ntDecodeCmd(aid, buffer, size);
    OSUnlockMutex(&lbl_8049EC30);
}

void NtMatching::newClientCallback(int index, void* param) {
    if (gUnk805a03d8 != 7) {
        netReport(lbl_8045FC14);
    }
}

extern "C" u64 DWC_GetFriendKey(const UnkFriendInfo::FriendData* data);
extern char lbl_804602D0[0x15];
extern s32 lbl_805A03E8;
extern s32 lbl_805A03F4;
extern s32 lbl_805A03E0;
extern u8 lbl_805A0410[1];
extern u8 lbl_805A0414;
extern u8 lbl_805A0418[8];
extern u8 lbl_8059E743;
extern u8 lbl_8059E744;
extern u32 g_NtAIDBitmap;

void NtMatching::requestDisconnectAIDBitmap(u8 bitmap) {
    unk34AB |= bitmap;
}

bool NtMatching::isSaveFriendInfoDone() {
    return ntFriendInfoSave::isNotSaving();
}

u8 NtMatching::getCloseAIDBitmap() {
    u8 bitmap = unk34AA;
    unk34AA = 0;
    return bitmap;
}

void NtMatching::setSequenceWatch(int sequence, int state) {
    unk3430 = sequence;
    unk3434 = state;
    unk3438 = OSGetTick();
}

u64 NtMatching::getRegisterFriendCode(int index) {
    return DWC_GetFriendKey(&ntFriendInfo::getInstance()->getFriendInfo()->m_friendData[index]);
}

void NtMatching::shutdown() {
    if (!unk11) {
        unk33CD = 1;
    } else if (!unk33CC) {
        unk33CC = 1;
        OSResumeThread(&unkA0);
        netReport(lbl_804602D0);
    }
}

bool NtMatching::isNetShutdownErrorOccured() {
    switch (lbl_805A03E8) {
    case 7: {
        bool error;
        switch (lbl_805A03F4) {
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            error = true;
            break;
        default:
            error = false;
            break;
        }
        if (error) {
            return true;
        }
        break;
    }
    case 2:
    case 3:
    case 6:
    case 14:
        return true;
    }
    return false;
}

void NtMatching::resetWork() {
    g_NtMatchingStatus = 5;
    lbl_805A03E0 = 5;
    unk33CE = 0;
    unk33F5 = 0;
    unk33F6 = 0;
    unk3404 = 0;
    memset(lbl_805A0410, 0, 4);
    g_NtAIDBitmap = 0;
    lbl_805A0414 = 0;
    lbl_8059E742 = 0xFF;
    lbl_8059E745 = 0xFF;
    lbl_8059E743 = 0xFF;
    lbl_8059E744 = 0xFF;
    for (int index = 0; index < 8; index++) {
        lbl_805A0418[index] = 0;
    }
}

extern char lbl_80460264[0x38];
extern char lbl_8046029C[0x34];
extern char lbl_8045FF54[0x70];
extern char lbl_8045FFC4[0x2A0];
extern int lbl_805A03FC;
extern int lbl_805A0404;
extern int lbl_805A0400;
extern u8 lbl_805A0409;

bool NtMatching::requestSuspendMatching(bool suspend, bool wait) {
    int status = g_NtMatchingStatus;
    bool matching = false;
    if (status >= 7 && status <= 9) {
        matching = true;
    }
    if (!matching) {
        return false;
    }
    if (suspend) {
        unk34A8 = lbl_8059E745;
    }
    unk34A9 = wait;
    unk33F6 = suspend;
    unk33F5 = 1;
    if (suspend) {
        if (unk34A4) {
            netReport(lbl_80460264, unk34A4);
            unk34A4 = 2;
        } else {
            unk34A4 = 2;
            netReport(lbl_8046029C);
        }
    }
    return true;
}

int NtMatching::calcWinningPercentage() {
    u32 rounds = ntFriendInfo::getInstance()->unk0->unk8;
    u32 wins = ntFriendInfo::getInstance()->unk0->unkC;
    int percentage;
    if (rounds) {
        percentage = 100.0f * (float(wins) / float(rounds));
        if (percentage > 100) {
            percentage = 100;
        }
    } else {
        percentage = 50;
    }
    netReport(lbl_8045FFC4, percentage, wins, rounds);
    lbl_805A0434->unk3498 = percentage;
    return percentage;
}

bool NtMatching::startMatching(int mode, int rule, int players) {
    if (g_NtMatchingStatus != 5) {
        return false;
    }
    lbl_805A03FC = mode;
    lbl_805A0404 = rule;
    lbl_805A0400 = players;
    lbl_805A0434->unk33F4 = 0;
    lbl_805A0434->unk33F5 = 0;
    lbl_805A0434->unk33F6 = 0;
    lbl_805A0434->unk34A4 = 0;
    lbl_805A0434->unk34AA = 0;
    lbl_805A0434->unk34AB = 0;
    unk3428 = 0;
    unk342C = 0;
    lbl_805A0409 = 30;
    g_NtMatchingStatus = 5;
    lbl_805A03E0 = 5;
    unk33CE = 0;
    unk33F5 = 0;
    unk33F6 = 0;
    unk3404 = 0;
    memset(lbl_805A0410, 0, 4);
    g_NtAIDBitmap = 0;
    lbl_805A0414 = 0;
    lbl_8059E742 = 0xFF;
    lbl_8059E745 = 0xFF;
    lbl_8059E743 = 0xFF;
    lbl_8059E744 = 0xFF;
    for (int index = 0; index < 8; index++) {
        lbl_805A0418[index] = 0;
    }
    if (unk34A4) {
        unk34A4 = 0;
        netReport(lbl_8045FF54);
    }
    unk3430 = 0;
    NtBase::setMatchingStatus(6);
    return true;
}

u8 NtMatching::isShutdowned() {
    return unk33CD;
}

void NtMatching::resetWatchDogTimer() {
    unk3422w = 0x1C20;
}

void NtMatching::putPacketDelay(int, u16 delay) {
    unk3540 = delay;
}

void NtMatching::putPacketLost(int, u8 lost) {
    unk354C = lost;
}

void NtMatching::watchResetNodeMissMatch() {
    unk342C = 0;
}

void NtMatching::thSaveUserDataNow() {
    bool canSave = false;
    int status = gUnk805a03d8;
    if (status >= 3 && status <= 6) {
        canSave = true;
    }
    if (canSave) {
        ntFriendInfoSave* save = new ((Heaps::HeapType)6) ntFriendInfoSave;
        if (save != nullptr) {
            save->startSave((Heaps::HeapType)6, true);
        }
    }
}

bool NtMatching::retryMatching(int mode, int rule) {
    if (g_NtMatchingStatus != 5) {
        return false;
    }
    lbl_805A03FC = mode;
    lbl_805A0404 = rule;
    lbl_805A0400 = 0;
    lbl_805A0434->unk33F4 = 0;
    lbl_805A0434->unk33F5 = 0;
    lbl_805A0434->unk33F6 = 0;
    lbl_805A0434->unk34A4 = 0;
    lbl_805A0434->unk34AA = 0;
    lbl_805A0434->unk34AB = 0;
    unk3428 = 0;
    unk342C = 0;
    if (unk34A4) {
        unk34A4 = 0;
        netReport(lbl_8045FF54);
    }
    unk3430 = 0;
    NtBase::setMatchingStatus(6);
    return true;
}

int NtMatching::searchFriendID(u32 aid) {
    muWifiInterfaceTask* task = g_muWifiInterfaceTask;
    UnkFriendInfo* info = ntFriendInfo::getInstance()->getFriendInfo();
    if (aid > 4) {
        return -1;
    }
    if (!(g_NtAIDBitmap & (1 << aid))) {
        return -1;
    }
    muWifiAIDInfo* aidInfo = &task->m_aidInfos[aid];
    s64 profileId = *reinterpret_cast<s64*>(&aidInfo->unk1D8[0]);
    u8* userData = reinterpret_cast<u8*>(info) + 0x10;
    UnkFriendInfo::FriendData* fd = info->m_friendData;
    int index;
    for (index = 0; index < 64; index++, fd++) {
        s64 cur = DWC_GetGsProfileId(userData, fd);
        if (cur == profileId) {
            break;
        }
    }
    if (index >= 64) {
        index = -1;
    }
    aidInfo->m_friendAIDIndex = index;
    return index;
}

// TODO: clean up, give variables real names
namespace jj_8014BF68 {
extern "C" void netReport__FPCce(const char*, ...);
extern char lbl_8045F7A8[];

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8014BF68(char* arg0, int arg1) {
    char* base = lbl_8045F7A8;
    int temp_r0;
    if (arg1 != 0) {
        temp_r0 = *(int*)(arg0 + 0x34A4);
        if (temp_r0 != 0) {
            netReport__FPCce(base + 0xABC, temp_r0);
            *(int*)(arg0 + 0x34A4) = arg1;
            return;
        }
    } else {
        temp_r0 = *(int*)(arg0 + 0x34A4);
        if (temp_r0 == 0) {
            return;
        }
    }
    *(int*)(arg0 + 0x34A4) = arg1;
    if (arg1 != 0) {
        netReport__FPCce(base + 0xAF4);
        return;
    }
    netReport__FPCce(base + 0x7AC);
}
}

namespace jj_80149BC0 {
extern "C" {
extern char lbl_80460CF0[];
extern unsigned char __vt__8NtBuffer[];
extern char* lbl_805A0434;
void fn_80025A74(void*);
void free__13gfHeapManagerFPv(void*);
void __dl__FPv(void*);

struct NB { virtual void f0(); virtual void f1(); virtual void f2(); };
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void* fn_80149BC0(void* self, int flag) {
    if (self) {
        char* p = (char*)self;
        *(void**)p = lbl_80460CF0;
        fn_80025A74(*(void**)(lbl_805A0434 + 0x353c));
        void* b = *(void**)(p + 0x3538);
        if (b) {
            free__13gfHeapManagerFPv(b);
            *(void**)(lbl_805A0434 + 0x3538) = 0;
        }
        lbl_805A0434 = 0;
        NB* n = (NB*)(p + 0x33b8);
        if (n) {
            *(void**)(p + 0x33b8) = __vt__8NtBuffer;
            n->f2();
        }
        if (flag > 0) {
            __dl__FPv(p);
        }
    }
    return self;
}
}
}

// TODO: clean up, give variables real names
namespace jj_801474A0 {
typedef unsigned long long u64;

struct FriendData {
    unsigned int data[3];
};

struct NetworkState {
    char pad[0x3440];
    u64 id;
};

extern "C" void* getInstance__12ntFriendInfoFv();
extern "C" void* fn_8014CB40(void*);
extern "C" int fn_80350CB4(void*, u64);
extern "C" void fn_80351A08(FriendData*, u64);
extern "C" char* getFriendInfo__12ntFriendInfoFv(void*);
extern "C" int fn_80351C70(FriendData*, FriendData*);
extern "C" void netReport__FPCce(const char*, ...);

extern char lbl_8045F7B8[];
extern char lbl_8045F7DC[];

// TODO: clean up, give variables real names
extern "C" int fn_801474A0(NetworkState* self, u64 id, bool* found, bool* own) {
    *found = false;
    *own = false;

    if (fn_80350CB4(fn_8014CB40(getInstance__12ntFriendInfoFv()), id) == 0) {
        return 0;
    }

    if (id == self->id) {
        netReport__FPCce(lbl_8045F7B8);
        *own = true;
    }

    FriendData data;
    fn_80351A08(&data, id);

    FriendData* entry = (FriendData*)(
        getFriendInfo__12ntFriendInfoFv(getInstance__12ntFriendInfoFv()) + 0x50
    );

    for (int i = 0; i < 64; ++i, ++entry) {
        if (fn_80351C70(entry, &data) == 1) {
            netReport__FPCce(lbl_8045F7DC);
            *found = true;
            break;
        }
    }

    return 1;
}
}

namespace jj_80149AA0 {
extern "C" {
extern unsigned char __vt__8NtBuffer[];
extern char lbl_80460CF0[];
extern char* lbl_805A0434;
extern char lbl_8059E7C0;
void* alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(int, unsigned long);
void* create__12gfMemoryPoolFPvUlPCc(void*, unsigned long, const char*);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void* fn_80149AA0(void* arg0) {
    char* p = (char*)arg0;
    *(s32*)(p + 0x04) = 0;
    *(void**)(p + 0x00) = lbl_80460CF0;
    *(u8*)(p + 0x11) = 0;
    *(s32*)(p + 0x94) = 0;
    *(s32*)(p + 0x98) = 0;
    *(void**)(p + 0x33b8) = __vt__8NtBuffer;
    *(s32*)(p + 0x33bc) = 0;
    *(s32*)(p + 0x33c0) = 0;
    *(u8*)(p + 0x33c4) = 0;
    *(s32*)(p + 0x33c8) = 0;
    *(u8*)(p + 0x33cc) = 0;
    *(u8*)(p + 0x33cd) = 1;
    *(u8*)(p + 0x33ce) = 0;
    *(u8*)(p + 0x33f4) = 0;
    *(u8*)(p + 0x33f5) = 0;
    *(u8*)(p + 0x33f6) = 0;
    *(s32*)(p + 0x3404) = 0;
    *(s32*)(p + 0x3408) = 0;
    *(s32*)(p + 0x340c) = 0;
    *(u8*)(p + 0x3410) = 0;
    *(u8*)(p + 0x3411) = 0;
    *(u8*)(p + 0x3412) = 0;
    *(u8*)(p + 0x3413) = 0;
    *(s32*)(p + 0x341c) = 0;
    *(s32*)(p + 0x3418) = 0;
    *(s8*)(p + 0x3420) = -1;
    *(s8*)(p + 0x3421) = -1;
    *(u16*)(p + 0x3422) = 0x1c20;
    *(u16*)(p + 0x3424) = 0;
    *(s32*)(p + 0x3428) = 0;
    *(s32*)(p + 0x342c) = 0;
    *(s32*)(p + 0x3430) = 0;
    *(s32*)(p + 0x3434) = 0;
    *(u8*)(p + 0x343c) = 0;
    *(s32*)(p + 0x3444) = 0;
    *(s32*)(p + 0x3440) = 0;
    *(s32*)(p + 0x3448) = 0;
    *(s32*)(p + 0x3498) = 0;
    *(s32*)(p + 0x349c) = 0;
    *(s32*)(p + 0x34a0) = 0;
    *(s32*)(p + 0x34a4) = 0;
    *(u8*)(p + 0x34ac) &= 0xFFFFFF00;

    lbl_805A0434 = p;

    void* heap = alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(6, 0x20000);
    *(void**)(p + 0x3538) = heap;
    *(void**)(p + 0x353c) = create__12gfMemoryPoolFPvUlPCc(heap, 0x20000, &lbl_8059E7C0);

    return arg0;
}
}
}

namespace jj_80147784 {

extern "C" {
extern OSMutex lbl_8049EC30;
s32 fn_801475A4(void* self, s32 a, u64 id, s32 b);
int fn_801483A4(u64 id);
void fn_80147C9C(void* self, s32 idx);
void** getInstance__12ntFriendInfoFv(void);
void* memset(void* p, int c, unsigned int n);
}

struct FriendSelf {
    u8 pad0[0x3410];
    u8 unk3410;
    u8 pad3411;
    u8 unk3412;
    u8 pad3413[0x3418 - 0x3413];
    u32 unk3418;
    u32 unk341C;
    s8 unk3420;
};

// TODO: clean up, give variables real names
extern "C" s32 fn_80147784(FriendSelf* self, u64 id, s32 flag) {
    s8 idx;
    s32 t;
    s32 res;
    u8 r;
    unsigned char* base;
    unsigned char* p;
    unsigned char* q;

    if (flag != 0) {
        OSLockMutex(&lbl_8049EC30);
        res = fn_801475A4(self, -1, id, 1);
        OSUnlockMutex(&lbl_8049EC30);
        return res;
    }

    OSLockMutex(&lbl_8049EC30);
    r = fn_801483A4(id);
    self->unk3420 = r;
    idx = (s8)r;
    if (idx >= 0) {
        self->unk341C = (u32)id;
        self->unk3418 = (u32)(id >> 32);
        self->unk3410 = 1;
        q = (unsigned char*)*getInstance__12ntFriendInfoFv() + (idx * 4);
        q[0x350] = 0;
        q[0x351] = 0;
        q[0x352] = 0;
        q[0x353] = 0;
        t = self->unk3420;
        base = (unsigned char*)*getInstance__12ntFriendInfoFv() + t * 0x89;
        p = base + 0x581;
        memset(p, 0, 10);
        memset(p + 10, 0, 10);
        memset(p + 0x3B, 0, 0x4C);
        memset(p + 0x32, 0, 8);
        p[0x3A] = 0;
        p[0x87] = 0;
        p[0x88] = 0;
        fn_80147C9C(self, self->unk3420);
        self->unk3412 = 1;
    }
    OSUnlockMutex(&lbl_8049EC30);
    return self->unk3420;
}
}

namespace jj_80149EF8 {
class BufferSlots { public: virtual void slot0(); virtual void slot1(); virtual void freeBuffer(); };
extern "C" {
extern unsigned char __vt__8NtBuffer[];
void __dl__FPv(void*);
extern char lbl_8045F7A8[];
extern NtSend* lbl_805A03D4;
void netReport__FPCce(const char*, ...);
}
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_80149EF8(u8* self) {
    char* base = lbl_8045F7A8;
    if (self[0x11] == 0) return;
    if (lbl_805A03D4 != 0) {
        lbl_805A03D4->destroy();
        u8* s = (u8*)lbl_805A03D4;
        if (s != 0) {
            BufferSlots* b = (BufferSlots*)(s + 0x40);
            if (b != 0) {
                *(void**)(s + 0x40) = __vt__8NtBuffer;
                b->freeBuffer();
            }
            b = (BufferSlots*)(s + 0x24);
            if (b != 0) {
                *(void**)(s + 0x24) = __vt__8NtBuffer;
                b->freeBuffer();
            }
            __dl__FPv(s);
        }
        lbl_805A03D4 = 0;
    }
    if (self[0x33CD] == 0) {
        netReport__FPCce(base + 0xBB8);
        if (self[0x11] == 0) {
            self[0x33CD] = 1;
        } else if (self[0x33CC] == 0) {
            self[0x33CC] = 1;
            OSResumeThread((OSThread*)(self + 0xA0));
            netReport__FPCce(base + 0xB28);
        }
        while (self[0x33CD] == 0) {
            OSSleepTicks((s64)((OS_BUS_CLOCK_SPEED / 4) / 1000) * 16);
        }
        netReport__FPCce(base + 0xBF0);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_80148664 {
extern "C" {
void OSLockMutex(void*);
void OSUnlockMutex(void*);
void OSSleepTicks(int, unsigned int);
void netReport__FPCce(const char*, ...);
int fn_8033A7DC(void);
int fn_8033A798(void*, void*);
unsigned char fn_800C851C(void);
unsigned char fn_8033A77C(void*);
void* memcpy(void*, const void*, unsigned long);
extern int gUnk805a03d8;
extern int g_NtAIDBitmap;
extern int g_muWifiInterfaceTask;
extern char lbl_8045F7A8[];
extern unsigned char lbl_8049EC30[];
extern unsigned char lbl_805A03D0;
extern void* lbl_805A0434;
extern unsigned char lbl_805A0414;
extern unsigned char lbl_805A0410;
extern signed char lbl_8059E742;
}

struct Bits7C {
    unsigned char hi : 2;
    unsigned char mid : 4;
    unsigned char lo : 2;
};

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_80148664(int arg0, int arg1, int arg2, int arg3, int arg4) {
    unsigned char lowBits;
    unsigned char oldFlag;
    void* buf;
    char* base = lbl_8045F7A8;

    while (lbl_805A03D0 != 0 && *(unsigned char*)((char*)lbl_805A0434 + 0x33CC) == 0) {
        OSSleepTicks(0, (*(unsigned int*)0x800000F8 >> 2) / 1000);
    }
    if (gUnk805a03d8 != 7) {
        if (arg1 != 0) {
            netReport__FPCce(base + 0x380, arg3);
        } else {
            netReport__FPCce(base + 0x3B0, arg3, arg4);
        }
        if (arg0 != 0) {
            netReport__FPCce(base + 0x3F0);
        }
        OSLockMutex(lbl_8049EC30);
        int v = fn_8033A7DC();
        void* p = lbl_805A0434;
        *(int*)((char*)p + 0x3404) = v;
        int len = fn_8033A798(&buf, p);
        oldFlag = lbl_805A0414;
        lbl_805A0414 = len;
        memcpy(&lbl_805A0410, buf, len);
        int bitmap = fn_8033A7DC();
        g_NtAIDBitmap = bitmap;
        lowBits = bitmap;
        int task = g_muWifiInterfaceTask;
        unsigned char idx = fn_800C851C();
        if (idx == 0xFF) {
            idx = 4;
        }
        unsigned char* obj = (unsigned char*)(task + idx * 0x218);
        ((Bits7C*)(obj + 0xC7))->mid = lowBits;
        lbl_8059E742 = fn_8033A77C(obj);
        netReport__FPCce(base + 0x2E0, lbl_805A0414, g_NtAIDBitmap);
        if (oldFlag > lbl_805A0414 && lbl_805A0414 == 1) {
            netReport__FPCce(base + 0x304);
        }
        *(unsigned char*)((char*)lbl_805A0434 + 0x34AA) |= 1 << arg3;
        OSUnlockMutex(lbl_8049EC30);
    }
}
}

namespace jj_80149644 {
extern "C" {
struct ntFriendInfo;
void** getInstance__12ntFriendInfoFv(ntFriendInfo* self);
void netReport__FPCce(const char*, ...);
void fn_800C8F68(const char*, ...);
void fn_80146FCC(int);
void fn_8014B130(int);
void fn_801473AC(void);
extern char lbl_8045F7A8[];
extern int lbl_805A03E8;
extern int lbl_805A03F4;
extern int g_NtMatchingStatus;

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
int fn_80149644(char* self) {
    char* base = lbl_8045F7A8;
    char* p = (char*)*getInstance__12ntFriendInfoFv((ntFriendInfo*)self) + 0x4f0;
    if (p != 0) {
        p[0x87] = 1;
        self[0x33f4] = 1;
    }
    if (*(int*)(self + 0x34a4) != 0) {
        *(int*)(self + 0x34a4) = 0;
        netReport__FPCce(base + 0x7ac);
    }
    *(int*)(self + 0x3428) = 0;
    *(int*)(self + 0x342c) = 0;
    *(int*)(self + 0x34a4) = 0;
    *(int*)(self + 0x3430) = 0;
    *(int*)(self + 0x3434) = 0;
    int v;
    switch (lbl_805A03F4) {
    case 2: case 3: case 4: case 5: case 6: case 7:
        v = 1; break;
    default:
        v = 0; break;
    }
    if (v == 0) {
        int w;
        switch (lbl_805A03E8) {
        case 7: {
            int u;
            switch (lbl_805A03F4) {
            case 2: case 3: case 4: case 5: case 6: case 7:
                u = 1; break;
            default:
                u = 0; break;
            }
            if (u == 0) goto L;
            w = 1;
            break;
        }
        case 2: case 3: case 6: case 14:
            w = 1; break;
        default:
        L:
            w = 0; break;
        }
        if (w == 0) fn_801473AC();
    }
    switch (g_NtMatchingStatus) {
    case 5: case 10:
        return 1;
    case 7: case 8: case 9: case 12:
        fn_80146FCC(10);
        return 1;
    default:
        netReport__FPCce(base + 0xa84);
        fn_800C8F68(base + 0xaa0);
        fn_8014B130(6);
        return 0;
    }
}
}
}

namespace jj_80148810 {
extern "C" {
void OSLockMutex(void*);
void OSUnlockMutex(void*);
void OSSleepTicks(int, unsigned int);
void netReport__FPCce(const char*, ...);
void fn_8033A50C(void*, int);
void fn_8034E57C(void*);
void fn_80148664();
void fn_801489D4();
void* getInstance__12ntFriendInfoFv();
void fn_8014CB40();
unsigned long long fn_80351964();
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void* fn_80157200(void*, int);
void fn_801578C4();
void fn_80146FCC(int);
void* fn_8014C180();
void fn_8014C26C(void*, int, int);
void fn_800D221C(int, int);
extern unsigned char lbl_805A03D0;
extern void* lbl_805A0434;
extern int gUnk805a03d8;
extern int g_muWifiInterfaceTask;
extern char lbl_8045F7A8[];
extern unsigned char lbl_8049EC30[];
extern int lbl_805A0430;
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_80148810(int arg0, int arg1) {
    char* base = lbl_8045F7A8;
    while (lbl_805A03D0 != 0 && *(unsigned char*)((char*)lbl_805A0434 + 0x33CC) == 0) {
        OSSleepTicks(0, (*(unsigned int*)0x800000F8 >> 2) / 1000);
    }
    if (gUnk805a03d8 != 7) {
        OSLockMutex(lbl_8049EC30);
        netReport__FPCce(base + 0x408, arg0);
        if (arg0 == 0) {
            netReport__FPCce(base + 0x420, arg1);
            *(int*)((char*)lbl_805A0434 + 0x3448) = arg1;
            fn_8033A50C((void*)fn_80148664, 0);
            fn_8034E57C((void*)fn_801489D4);
            unsigned long long* obj = (unsigned long long*)((char*)lbl_805A0434 + 0x3440);
            getInstance__12ntFriendInfoFv();
            fn_8014CB40();
            unsigned long long v = fn_80351964();
            *obj = v;
            netReport__FPCce(base + 0x440, (unsigned int)v, (unsigned int)(v >> 32), (unsigned int)v);
            void* p = __nw__FUlQ25Heaps8HeapType(0x380, 6);
            if (p != 0) {
                p = fn_80157200(p, 6);
            }
            lbl_805A0430 = (int)p;
            fn_801578C4();
            fn_80146FCC(4);
            if (*(unsigned char*)((char*)lbl_805A0434 + 0x343C) != 0) {
                int w = gUnk805a03d8;
                int flag = 0;
                if (w >= 3 && w <= 6) {
                    flag = 1;
                }
                if (flag != 0) {
                    void* q = __nw__FUlQ25Heaps8HeapType(0x48, 6);
                    if (q != 0) {
                        q = fn_8014C180();
                    }
                    if (q != 0) {
                        fn_8014C26C(q, 6, 1);
                    }
                }
            }
            fn_800D221C(g_muWifiInterfaceTask, 1);
        } else {
            netReport__FPCce(base + 0x45C);
        }
        OSUnlockMutex(lbl_8049EC30);
    }
}
}

namespace jj_80149C70 {
class BufferDispatch { public: virtual void f0(); virtual void alloc(unsigned int, Heaps::HeapType); };
extern "C" {
void fn_80146FCC(int);
void fn_801473AC();
void* fn_8014B364(void*);
extern int lbl_805A03E0, lbl_805A03E8, lbl_805A03F4;
extern unsigned char lbl_805A03E4, lbl_805A0438;
extern OSMutex lbl_8049EC78, lbl_8049EC30, lbl_8049EC48;
extern NtSend* lbl_805A03D4;
extern unsigned char __vt__8NtBuffer[];
void* __nw__FUlQ25Heaps8HeapType(unsigned int, Heaps::HeapType);
}
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_80149C70(unsigned char* self, int arg1) {
    fn_80146FCC(0);
    lbl_805A03E0 = 0;
    self[0x33ce] = 0;
    *(int*)(self+0x94) = 0;
    *(int*)(self+0x98) = 0;
    lbl_805A03E4 = 0;
    self[0x33cc] = 0;
    lbl_805A0438 = 0;
    *(int*)(self+0x3444) = 0;
    *(int*)(self+0x3440) = 0;
    *(int*)(self+0x3408) = 0;
    *(int*)(self+0x3448) = 0;
    self[0x343c] = 0;
    self[0x33f4] = 0;
    self[0x33f5] = 0;
    self[0x33f6] = 0;
    *(int*)(self+0x340c) = 0;
    self[0x3410] = 0;
    self[0x3411] = 0;
    *(int*)(self+0x341c) = 0;
    *(int*)(self+0x3418) = 0;
    *(signed char*)(self+0x3420) = -1;
    *(signed char*)(self+0x3421) = -1;
    *(int*)(self+0x3494) = 100;
    *(int*)(self+0x349c) = 0;
    *(int*)(self+0x34a0) = 0;
    *(int*)(self+0x3428) = 0;
    *(int*)(self+0x342c) = 0;
    *(int*)(self+0x34a4) = 0;
    *(int*)(self+0x3430) = 0;
    *(int*)(self+0x3434) = 0;
    int v;
    switch (lbl_805A03F4) {
    case 2: case 3: case 4: case 5: case 6: case 7: v = 1; break;
    default: v = 0; break;
    }
    if (v == 0) {
        int w;
        switch (lbl_805A03E8) {
        case 7: {
            int u;
            switch (lbl_805A03F4) {
            case 2: case 3: case 4: case 5: case 6: case 7: u = 1; break;
            default: u = 0; break;
            }
            if (u == 0) goto L;
            w = 1; break;
        }
        case 2: case 3: case 6: case 14: w = 1; break;
        default:
        L: w = 0; break;
        }
        if (w == 0) fn_801473AC();
    }
    ((BufferDispatch*)(self+0x33b8))->alloc(0x8000, (Heaps::HeapType)6);
    OSInitMutex(&lbl_8049EC78);
    OSInitMutex(&lbl_8049EC30);
    OSInitMutex(&lbl_8049EC48);
    if (arg1 != 0) {
        memset(self+0x3b8, 0x55, 0x3000);
        memset(self+0xa0, 0, 0x318);
        OSCreateThread((OSThread*)(self+0xa0), fn_8014B364, 0, self+0x33b8, 0x3000, 11, 1);
        OSResumeThread((OSThread*)(self+0xa0));
        self[0x11] = 1;
        self[0x33cd] = 0;
    }
    unsigned char* p = (unsigned char*)__nw__FUlQ25Heaps8HeapType(0x54, (Heaps::HeapType)6);
    if (p) {
        *(void**)(p+0x24) = __vt__8NtBuffer;
        *(int*)(p+0x28) = 0;
        *(int*)(p+0x2c) = 0;
        p[0x30] = 0;
        *(int*)(p+0x34) = 0;
        *(void**)(p+0x40) = __vt__8NtBuffer;
        *(int*)(p+0x44) = 0;
        *(int*)(p+0x48) = 0;
        p[0x4c] = 0;
        *(int*)(p+0x50) = 0;
    }
    lbl_805A03D4 = (NtSend*)p;
    lbl_805A03D4->create();
    memset(self+0x12, 0, 0x40);
    memset(self+0x52, 0, 0x40);
}
}

namespace jj_8014A0F4 {


extern unsigned char* lbl_805A0434;
extern int gUnk805a03d8;
extern char lbl_804603B8[];

extern "C" unsigned long long fn_8014FEB4();
extern "C" int fn_80336928(void*, int);
extern "C" int fn_803369C0();
extern "C" void fn_8014A3A0();
extern "C" void fn_801475A4(void*, int, int, int, int);
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void* fn_8014C180(void*);
extern "C" void fn_8014C26C(void*, int, int);
extern "C" void fn_8014C964(void*);
extern "C" void fn_8014C970(void*);
extern "C" void fn_8014CB1C(void*, int);
extern "C" void fn_803369F0(void*);
extern "C" void fn_80148210();
extern "C" void fn_80148280();
extern "C" void fn_801482C8();
extern "C" void fn_80339E98(int, void (*)(), int, void (*)(), int, void (*)(), int);

inline int checkState(int v) {
    int flag = 0;
    if (v >= 3 && v <= 6) {
        flag++;
    }
    return flag;
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8014A0F4() {
    if (lbl_805A0434[0x33f4]) {
        unsigned char* p = *(unsigned char**)ntFriendInfo::getInstance() + 0x4f0;
        unsigned long long v = fn_8014FEB4();

        p[0x32] = v >> 56;
        p[0x33] = v >> 48;
        p[0x34] = v >> 40;
        p[0x35] = v >> 32;
        p[0x36] = v >> 24;
        p[0x37] = v >> 16;
        p[0x38] = v >> 8;
        p[0x39] = v;

        int result = fn_80336928(p, 0x89);
        if (((unsigned int)(-result | result) >> 31) != 0) {
            lbl_805A0434[0x33f4] = 0;
            netReport(lbl_804603B8);
        }
    }

    if (*(int*)ipSwitch::getInstance() != 1) {
        for (int i = 0; i < 2; i++) {
            fn_8014A3A0();
        }

        if (lbl_805A0434[0x3410] && fn_803369C0()) {
            fn_801475A4(
                lbl_805A0434,
                *(signed char*)(lbl_805A0434 + 0x3420),
                *(int*)(lbl_805A0434 + 0x3418),
                *(int*)(lbl_805A0434 + 0x341c),
                0
            );
            lbl_805A0434[0x3410] = 0;

            if (lbl_805A0434[0x3412]) {
                if (checkState(gUnk805a03d8)) {
                    void* p = __nw__FUlQ25Heaps8HeapType(0x48, 6);
                    if (p) {
                        p = fn_8014C180(p);
                    }
                    if (p) {
                        fn_8014C26C(p, 6, 1);
                    }
                }
                lbl_805A0434[0x3412] = 0;
            }
        }

        if (lbl_805A0434[0x3411] && fn_803369C0()) {
            int index = *(signed char*)(lbl_805A0434 + 0x3421);
            char* info = (char*)ntFriendInfo::getInstance()->getFriendInfo();

            fn_8014C964(ntFriendInfo::getInstance());
            fn_803369F0(info + index * 12 + 0x50);
            *(index * 4 + info + 0x350) = 0;
            fn_8014CB1C(ntFriendInfo::getInstance(), 1);
            fn_8014C970(ntFriendInfo::getInstance());
            fn_80339E98(0, fn_80148210, 0, fn_80148280, 0, fn_801482C8, 0);
            lbl_805A0434[0x3411] = 0;

            if (lbl_805A0434[0x3412]) {
                if (checkState(gUnk805a03d8)) {
                    void* p = __nw__FUlQ25Heaps8HeapType(0x48, 6);
                    if (p) {
                        p = fn_8014C180(p);
                    }
                    if (p) {
                        fn_8014C26C(p, 6, 1);
                    }
                }
                lbl_805A0434[0x3412] = 0;
            }
        }
    }
}
}

namespace jj_8014792C {
extern "C" {
int fn_803368A0(char*, int);
int fn_8035129C(void*);
int fn_803F6150(const void*, const void*, unsigned long);
void fn_80339E98(int, void (*)(int,int), int, void (*)(), int, void (*)(), int);
void fn_80148210(int,int);
void fn_80148280();
void fn_801482C8();
}
extern char lbl_8045F890[];
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8014792C(void* arg) {
    unsigned char* self = (unsigned char*)arg;
    unsigned char oldOrder[64];
    unsigned char newOrder[64];
    netReport(lbl_8045F890);
    char* info = (char*)ntFriendInfo::getInstance()->getFriendInfo();
    *(int*)(self + 0x3408) = fn_803368A0(info + 0x50, 64);
    memset(self + 0x12, 0, 64);
    char* entry = info + 0x50;
    for (int i = 0; i < 64; i++, entry += 12) {
        if (fn_8035129C(entry)) self[0x12 + i] = 1;
    }
    memcpy(oldOrder, info + 0x27c1, 64);
    for (int i = 0; i < 64; i++) {
        unsigned char n = oldOrder[i] - 1;
        if (n >= 64) oldOrder[i] = 0;
        else if (self[0x12 + n] == 0) oldOrder[i] = 0;
        else if (self[0x12 + n] != 1) oldOrder[i] = 0;
        else self[0x12 + n] = 2;
    }
    unsigned char count = 0;
    memset(newOrder, 0, 64);
    for (int i = 0; i < 64; i++) {
        if (oldOrder[i]) newOrder[count++] = oldOrder[i];
    }
    for (int i = 0; i < 64; i++) {
        if (self[0x12 + i] == 1) newOrder[count++] = i + 1;
    }
    if (fn_803F6150(newOrder, info + 0x27c1, 64)) memcpy(info + 0x27c1, newOrder, 64);
    memcpy(self + 0x52, info + 0x27c1, 64);
    fn_80339E98(0, fn_80148210, 0, fn_80148280, 0, fn_801482C8, 0);
}
}

// TODO: clean up, give variables real names
namespace jj_80149204 {

extern "C" {
int fn_8033CAC0(int, int);
void fn_8033CAE0(int*, int*);
int fn_80339F84(unsigned char, void*, void*, int, int, int, void*, int, int, int, int);
int fn_8033A0C8(unsigned char, void*, int, void*, int, int, int, int);
int fn_8033A200(int, void*, int, void*, int, int, void*, int);
unsigned char fn_8033C180(int, void*, int*);
unsigned char fn_8033C5D8(int, void*, char*);
int snprintf(char*, unsigned long, const char*, ...);
char* strcpy(char*, const char*);
extern char lbl_8045F7A8[];
extern char lbl_8059E770[4];
extern char lbl_8059E794[6];
extern char lbl_8059E7A0[8];
extern char lbl_8059E7A8[5];
extern char lbl_8059E7B0[5];
extern int lbl_805A03FC;
extern int lbl_805A0400;
extern int lbl_805A0404;
extern char* lbl_805A0434;
extern float lbl_805A2B68;
void fn_80148A74(void);
void fn_80148A94(void);
void fn_8014B874(void);
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" int fn_80149204(void) {
    int r;
    char* base = lbl_8045F7A8;
    char buf[0x80];
    int a2c, a28, a24, a20, a1c, a18;

    lbl_805A0434[0x3413] = 0;
    switch (lbl_805A03FC) {
    case 1:
        netReport(base + 0x868);
        {
            int x = *(int*)(lbl_805A0434 + 0x3494);
            if (x < 0) {
                x = 0x5265C00;
            }
            if (fn_8033CAC0(x, x) == 1) {
                fn_8033CAE0(&a28, &a2c);
                netReport(base + 0x7e0, a2c);
            } else {
                netReport(base + 0x7f0);
            }
        }
        strcpy(lbl_805A0434 + 0x344c, lbl_805A0434 + 0x33cf);
        {
            char** nameTable = (char**)(base + 0x848);
            char* serverName = (*(char***)&nameTable)[((int*)g_muWifiInterfaceTask)[0x1b]];
            snprintf(lbl_805A0434 + 0x344c, 0x3f, lbl_8059E794,
                     lbl_805A0434 + 0x33cf, serverName);
        }
        {
            int c = fn_8033C5D8(0x64, lbl_8059E7A0, lbl_805A0434 + 0x344c);
            netReport(base + 0x888, c, lbl_805A0434 + 0x344c);
        }
        snprintf(buf, 0x7f, base + 0x8b8, lbl_805A0434 + 0x344c);
        *(int*)(lbl_805A0434 + 0x3490) = 0;
        *(int*)(lbl_805A0434 + 0x348c) = 0;
        lbl_805A0434[0x34ac] &= 0xFFFFFF00;
        {
            int v;
            char* inner = (char*)*(void**)ntFriendInfo::getInstance();
            u32 total = *(u32*)(inner + 8);
            u32 c5 = *(u32*)((char*)*(void**)ntFriendInfo::getInstance() + 0xc);
            if (total != 0) {
                float q = (float)c5 / (float)total;
                v = (int)(lbl_805A2B68 * q);
                if (v > 0x64) {
                    v = 0x64;
                }
            } else {
                v = 0x32;
            }
            netReport(base + 0x81c, v, c5, total);
            *(int*)(lbl_805A0434 + 0x3498) = v;
            int u1 = fn_8033C180(0x65, lbl_8059E7A8, (int*)(lbl_805A0434 + 0x3498));
            netReport(base + 0x8c8, u1, *(int*)(lbl_805A0434 + 0x3498));
            int u2 = fn_8033C180(0x66, lbl_8059E7B0, (int*)(lbl_805A0434 + 0x348c));
            netReport(base + 0x8fc, u2, *(int*)(lbl_805A0434 + 0x348c));
        }
        lbl_805A0434[0x3413] = 1;
        netReport(base + 0x940, lbl_805A0404);
        {
            int rr = fn_80339F84((unsigned char)lbl_805A0404, buf,
                                 (void*)fn_80148A94, 0, 0, 0,
                                 (void*)fn_8014B874, 0, 0, 0, 0);
            if (rr == 0) {
                netReport(base + 0x960);
            }
            r = rr;
        }
        break;
    case 2:
        netReport(base + 0x9a8);
        if (fn_8033CAC0(0x5265C00, 0x5265C00) == 1) {
            fn_8033CAE0(&a20, &a24);
            netReport(base + 0x7e0, a24);
        } else {
            netReport(base + 0x7f0);
        }
        netReport(base + 0x9c0, lbl_805A0404);
        r = fn_8033A0C8((unsigned char)lbl_805A0404,
                        (void*)fn_80148A94, 0,
                        (void*)fn_80148A74, 0, 0, 0, 0);
        if (r == 0) {
            netReport(base + 0x9e0);
        }
        break;
    case 3:
        netReport(base + 0xa1c, lbl_805A0400);
        if (fn_8033CAC0(0x5265C00, 0x5265C00) == 1) {
            fn_8033CAE0(&a18, &a1c);
            netReport(base + 0x7e0, a1c);
        } else {
            netReport(base + 0x7f0);
        }
        lbl_805A0434[0x3413] = 1;
        r = fn_8033A200(lbl_805A0400, (void*)fn_80148A94, 0,
                        (void*)fn_80148A74, 0, 0,
                        lbl_8059E770, 0);
        if (r == 0) {
            netReport(base + 0xa3c);
        }
        break;
    default:
        r = 0;
        break;
    }
    return (r - 1) == 0;
}
}

namespace jj_80148A94 {

extern "C" {
void fn_80146FCC(int);
void fn_800CFEB8(void*);
void fn_800D53D4(void*, unsigned char);
void fn_800D54DC(void*);
void fn_80143D80(int, unsigned char);
void fn_80145B78(int);
void fn_80145B88(int);
int fn_8033A77C(void*);
int fn_8033A798(void*);
int fn_8033A7DC(void);
unsigned char fn_8033AA14(void);
unsigned char fn_800C851C(void);
void fn_8034E50C(int, void*, int);
void* memcpy(void*, const void*, unsigned long);
int DWC_GetLastErrorEx(int*, int*);
void DWC_ClearError(void);

extern int gUnk805a03d8;
extern int g_NtAIDBitmap;
extern int g_NtMatchingStatus;
extern int lbl_805A03A8;
extern char lbl_8045F7A8[];
extern OSMutex lbl_8049EC30;
extern unsigned char lbl_805A03D0;
extern unsigned char lbl_805A0414;
extern unsigned char lbl_805A0410;
extern void* lbl_805A0434;
extern unsigned char lbl_8059E742;
extern unsigned char lbl_8059E743;
extern unsigned char lbl_8059E744;
extern unsigned char lbl_8059E745;
}

struct Bits7C {
    unsigned char hi : 2;
    unsigned char mid : 4;
    unsigned char lo : 2;
};

struct BitsFCC {
    unsigned int high : 8;
    unsigned int low : 24;
};

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_80148A94(int arg0, int arg1, int arg2, int arg3) {
    unsigned char lowBits;
    char* base = lbl_8045F7A8;
    int sp10, spC;
    void* buf;

    while (lbl_805A03D0 != 0 &&
           *(unsigned char*)((char*)lbl_805A0434 + 0x33CC) == 0) {
        OSSleepTicks((s64)((*(unsigned int*)0x800000F8 >> 2) / 1000));
    }

    if (gUnk805a03d8 != 7) {
        netReport(base + 0x494);
        *(unsigned char*)((char*)lbl_805A0434 + 0x3413) = 0;
        OSLockMutex(&lbl_8049EC30);

        if (arg0 == 0) {
            int len = fn_8033A798(&buf);
            unsigned char oldFlag = lbl_805A0414;
            lbl_805A0414 = (unsigned char)len;
            memcpy(&lbl_805A0410, buf, (unsigned long)len);

            int bitmap = fn_8033A7DC();
            g_NtAIDBitmap = bitmap;
            unsigned char* initialTask = (unsigned char*)g_muWifiInterfaceTask;
            lowBits = bitmap;
            unsigned char idx = fn_800C851C();
            if (idx == 0xFF) {
                idx = 4;
            }
            unsigned char* obj = initialTask + idx * 0x218;
            ((Bits7C*)(obj + 0xC7))->mid = lowBits;
            lbl_8059E742 = fn_8033A77C(obj);

            netReport(base + 0x2E0, lbl_805A0414, g_NtAIDBitmap);
            if (oldFlag > lbl_805A0414 && lbl_805A0414 == 1) {
                netReport(base + 0x304);
            }

            if (arg1 == 1) {
                if (arg2 == 1) {
                    if (arg3 == 1) {
                        netReport(base + 0x4B0);
                    } else {
                        netReport(base + 0x4E8);
                    }
                    fn_80146FCC(5);
                } else if (arg3 == 1) {
                    netReport(base + 0x52C);
                } else {
                    netReport(base + 0x568);
                }
                OSUnlockMutex(&lbl_8049EC30);
                return;
            }

            unsigned char* ptr;
            int offset;
            int i;
            i = 0;
            ptr = &lbl_805A0410;
            offset = 0;
            for (; i < (int)lbl_805A0414; i++) {
                unsigned char val = *ptr;
                if (val >= 4) {
                    netReport(base + 0x598, 4);
                    OSUnlockMutex(&lbl_8049EC30);
                    return;
                }
                if (val != lbl_8059E742) {
                    fn_8034E50C(
                        val,
                        (void*)(*(int*)((char*)lbl_805A0434 + 0x33C8) + offset),
                        0x2000);
                }
                ptr++;
                offset += 0x2000;
            }

            int diff;
            int newVal = fn_8033A7DC();
            int oldVal = *(int*)((char*)lbl_805A0434 + 0x3404);
            diff = newVal & (oldVal ^ newVal);
            int mask = 1 << lbl_8059E742;
            netReport(base + 0x5C8, newVal, diff, mask);
            int remaining = diff & ~(1 << lbl_8059E742);
            *(int*)((char*)lbl_805A0434 + 0x3404) = newVal;
            unsigned char* task = (unsigned char*)g_muWifiInterfaceTask;
            fn_800CFEB8(task);

            for (unsigned int j = 0; j < 4; j++) {
                if ((1 << (unsigned char)j) & remaining) {
                    netReport(base + 0x5E4, (unsigned char)j);
                    fn_80143D80(lbl_805A03A8, (unsigned char)j);

                    if (arg2 != 0) {
                        unsigned char result = fn_8033AA14();
                        lbl_8059E744 = result;
                        if (result == 0xFF) {
                            lbl_8059E744 = 0xFF;
                            netReport(base + 0x600, lbl_8059E745);
                        } else {
                            netReport(base + 0x630, result);
                        }
                        lbl_8059E745 = lbl_8059E744;
                        lbl_8059E743 = lbl_8059E744;
                        netReport(base + 0x644);
                        if ((task[0xFC9] >> 7) & 1) {
                            fn_80145B88(lbl_805A03A8);
                            fn_80145B78(lbl_805A03A8);
                        }
                        fn_800D54DC(task);
                    } else {
                        if (lbl_805A0414 == 2) {
                            unsigned char result = fn_8033AA14();
                            lbl_8059E744 = result;
                            if (result == 0xFF) {
                                lbl_8059E744 = 0xFF;
                                lbl_8059E743 = 0xFF;
                                netReport(base + 0x600, lbl_8059E745);
                            } else {
                                lbl_8059E745 = result;
                                lbl_8059E743 = result;
                                netReport(base + 0x67C, result);
                                if (!((task[0xFC9] >> 7) & 1)) {
                                    ((BitsFCC*)(task + 0xFCC))->high = 4;
                                }
                            }
                        }
                        if ((task[0xFC9] >> 7) & 1) {
                            netReport(base + 0x6A4);
                            fn_80145B88(lbl_805A03A8);
                            fn_80145B78(lbl_805A03A8);
                        } else {
                            fn_800D53D4(task, (unsigned char)j);
                        }
                    }
                }
            }
            if (g_NtMatchingStatus == 7) {
                fn_80146FCC(8);
            }
        } else if (arg0 == 18) {
            int err = DWC_GetLastErrorEx(&sp10, &spC);
            netReport(base + 0x6D8);
            netReport(base + 0x700, err, sp10, spC);
            netReport(base + 0x73C);
            DWC_ClearError();
            fn_80146FCC(5);
        } else if (arg0 == 11) {
            netReport(base + 0x758);
        } else {
            netReport(base + 0x780, arg0);
        }
        OSUnlockMutex(&lbl_8049EC30);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_801475A4 {
typedef unsigned long long u64;
typedef unsigned int u32;
typedef int s32;

struct NetworkState {
    char pad[0x3440];
    u64 id;
};

struct FriendData {
    u32 data[3];
};

extern "C" {
void* getInstance__12ntFriendInfoFv();
int getUserData__12ntFriendInfoFv(int*);
int DWC_CheckFriendKey(void*, u64);
void DWC_CreateFriendKeyToken(FriendData*, u64);
char* getFriendInfo__12ntFriendInfoFv(void*);
s32 getAvailableFriendListIndex__10NtMatchingFUx(u64);
void netReport__FPCce(const char*, ...);
void setDirtyFlag__12ntFriendInfoFb(void*, int);
void DWC_UpdateServersAsync(int, void*, int, void*, int, void*, int);
void* memset(void*, int, unsigned long);
void updateServerCallback__10NtMatchingFiiPv();
void changetFriendStatusCallback__10NtMatchingFiUcPCcPv();
void deleteFriendListCallback__10NtMatchingFiiPv();
}

extern char lbl_8045F7A8[];

extern "C" s32 fn_801475A4(NetworkState* self, s32 index, u64 id, s32 flag) {
    const char* base = lbl_8045F7A8;
    s32 idx = flag;
    netReport__FPCce(base + 0x70);

    if (id == self->id) {
        netReport__FPCce(base + 0x88);
        return -3;
    }

    int found = DWC_CheckFriendKey(
        (void*)getUserData__12ntFriendInfoFv((int*)getInstance__12ntFriendInfoFv()), id);
    if (found == 0) {
        netReport__FPCce(base + 0xa8);
        return -2;
    }

    flag = index;
    if (index < 0)
        flag = getAvailableFriendListIndex__10NtMatchingFUx(id);

    if (flag < 0) {
        netReport__FPCce(base + 0xc8);
        return -1;
    }

    if (idx)
        return 0;

    DWC_CreateFriendKeyToken(
        (FriendData*)(getFriendInfo__12ntFriendInfoFv(
            getInstance__12ntFriendInfoFv()) + flag * 0xc + 0x50),
        id);
    DWC_UpdateServersAsync(0, (void*)updateServerCallback__10NtMatchingFiiPv, 0, (void*)changetFriendStatusCallback__10NtMatchingFiUcPCcPv,
                0, (void*)deleteFriendListCallback__10NtMatchingFiiPv, 0);

    if (index < 0) {
        char* b = (char*)*(int*)getInstance__12ntFriendInfoFv() + flag * 4;
        b[0x350] = 0;
        b[0x351] = 0;
        b[0x352] = 0;
        b[0x353] = 0;

        char* entry = (char*)*(int*)getInstance__12ntFriendInfoFv()
                      + flag * 0x89;
        char* p = entry + 0x581;
        memset(p, 0, 0xa);
        memset(p + 0xa, 0, 0xa);
        memset(p + 0x3b, 0, 0x4c);
        memset(p + 0x32, 0, 8);
        p[0x3a] = 0;
        p[0x87] = 0;
        p[0x88] = 0;
    }

    setDirtyFlag__12ntFriendInfoFb(getInstance__12ntFriendInfoFv(), 1);
    return flag;
}
}

// TODO: clean up, give variables real names
namespace jj_80147C9C {

extern "C" void lock__12ntFriendInfoFv(ntFriendInfo*);
extern "C" void unlock__12ntFriendInfoFv(ntFriendInfo*);
extern "C" void setDirtyFlag__12ntFriendInfoFb(ntFriendInfo*, int);
extern "C" int DWC_GetNumFriend(char*, int);
extern char lbl_8045F7A8[];
extern void* lbl_805A0434;

extern "C" unsigned int fn_80147C9C(unsigned char* self, int arg) {
    char* base = lbl_8045F7A8;
    lock__12ntFriendInfoFv(ntFriendInfo::getInstance());
    char* info = (char*)ntFriendInfo::getInstance()->getFriendInfo();
    unsigned char* order = (unsigned char*)info + 0x27c1;
    int count = DWC_GetNumFriend(info + 0x50, 0x40);
    *(int*)((char*)lbl_805A0434 + 0x3408) = count;
    unsigned int result = 0;
    int i;
    for (i = 0; i < 64; i++) {
        if (order[i] == 0) {
            result = 1;
            order[i] = arg + 1;
            break;
        }
    }
    if (result == 1)
        setDirtyFlag__12ntFriendInfoFb(ntFriendInfo::getInstance(), 1);
    else
        netReport(base + 0x100);
    unlock__12ntFriendInfoFv(ntFriendInfo::getInstance());
    netReport(base + 0x130, i, count);
    for (int k = 0; k < 64; k++)
        netReport0(base + 0x168, k, order[k] - 1);
    unsigned char* slots;
    int j;
    for (j = 0; j < 64; j++) {
        if ((self + j)[0x12] == 0)
            break;
    }
    if (j == 64) {
        netReport(base + 0x180);
        return 0;
    }
    slots = self + 0x52;
    (self + j)[0x12] = 3;
    for (i = 0; i < 64; i++) {
        if (slots[i] == 0) {
            slots[i] = j + 1;
            break;
        }
    }
    return result;
}
}

// TODO: clean up, give variables real names
namespace jj_80147F10 {

extern "C" void lock__12ntFriendInfoFv(ntFriendInfo*);
extern "C" void unlock__12ntFriendInfoFv(ntFriendInfo*);
extern "C" void setDirtyFlag__12ntFriendInfoFb(ntFriendInfo*, int);
extern "C" int DWC_GetNumFriend(char*, int);
extern char lbl_8045F7A8[];
extern void* lbl_805A0434;

extern "C" unsigned int deleteDisplayOrder__10NtMatchingFib(unsigned char* self, int arg, int flag) {
    char* base = lbl_8045F7A8;
    char* info = (char*)ntFriendInfo::getInstance()->getFriendInfo();
    unsigned char* order = (unsigned char*)info + 0x27c1;
    int count = DWC_GetNumFriend(info + 0x50, 0x40);
    *(int*)((char*)lbl_805A0434 + 0x3408) = count;
    unsigned int result = 0;
    lock__12ntFriendInfoFv(ntFriendInfo::getInstance());
    int i;
    for (i = 0; i < 64; i++) {
        if (arg == order[i] - 1) {
            netReport(base + 0x1a4, arg, i);
            for (; i < 63; i++)
                order[i] = order[i + 1];
            result = 1;
            order[63] = 0;
            break;
        }
    }
    if (result == 1)
        setDirtyFlag__12ntFriendInfoFb(ntFriendInfo::getInstance(), 1);
    else if (flag)
        netReport(base + 0x1cc, arg);
    unlock__12ntFriendInfoFv(ntFriendInfo::getInstance());
    netReport(base + 0x210, count);
    for (int k = 0; k < 64; k++)
        netReport0(base + 0x168, k, order[k] - 1);
    if (flag) {
        unsigned char* slots = self + 0x52;
        (self + arg)[0x12] = 0;
        for (int j = 0; j < 64; j++) {
            if (arg == slots[j] - 1) {
                for (; j < 63; j++)
                    slots[j] = slots[j + 1];
                slots[63] = 0;
                break;
            }
        }
    }
    return result;
}
}

// TODO: clean up, give variables real names
namespace jj_8014B130 {
extern "C" {
void fn_8013F658(int);
void netReport__FPCce(const char*, ...);
void netMinReport__FPCce(const char*, ...);
void setMatchingStatus__6NtBaseFi(int);
int getErrorStr__6NtBaseFl(int, int);
void clearGlobalAIDStatusLive__19muWifiInterfaceTaskFv(int);
void fn_8014403C(int);
extern char lbl_8045F7A8[];
extern int g_muWifiInterfaceTask;
extern int lbl_805A0390;
extern int lbl_805A0434;
extern int lbl_805A03A8;
extern int lbl_805A03E0;
extern int lbl_805A03E8;
extern int lbl_805A03F4;
extern int g_NtMatchingStatus;

void setErrorStatus__10NtMatchingFl(int arg0) {
    char* base = lbl_8045F7A8;
    int taskPtr = g_muWifiInterfaceTask;
    int sel;
    fn_8013F658(lbl_805A0390);
    char* self = (char*)lbl_805A0434;
    if (*(int*)(self + 0x34a4) != 0) {
        *(int*)(self + 0x34a4) = 0;
        netReport__FPCce(base + 0x7ac);
    }
    *(int*)(self + 0x3430) = 0;
    ((unsigned char*)lbl_805A0434)[0x34ac] = (unsigned char)(((unsigned char*)lbl_805A0434)[0x34ac] & 0xFFFFFF00);
    switch (lbl_805A03E8) {
    case 7: {
        int v;
        switch (lbl_805A03F4) {
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            v = 1;
            break;
        default:
            v = 0;
            break;
        }
        if (v == 0) goto L;
        sel = 1;
        break;
    }
    case 2:
    case 3:
    case 6:
    case 14:
        sel = 1;
        break;
    default:
    L:
        sel = 0;
        break;
    }
    if (sel != 0) {
        int r = getErrorStr__6NtBaseFl((int)lbl_805A0434, arg0);
        netReport__FPCce(base + 0x10c8, r);
        netMinReport__FPCce(base + 0x10e8, getErrorStr__6NtBaseFl((int)lbl_805A0434, arg0));
    } else {
        int r = getErrorStr__6NtBaseFl((int)lbl_805A0434, arg0);
        netReport__FPCce(base + 0x10fc, r);
        netMinReport__FPCce(base + 0x1110, getErrorStr__6NtBaseFl((int)lbl_805A0434, arg0));
        if (g_NtMatchingStatus != 0xc) {
            lbl_805A03E0 = g_NtMatchingStatus;
        }
        lbl_805A03E8 = arg0;
        setMatchingStatus__6NtBaseFi(0xc);
    }
    clearGlobalAIDStatusLive__19muWifiInterfaceTaskFv(taskPtr);
    fn_8014403C(lbl_805A03A8);
}
}
}

