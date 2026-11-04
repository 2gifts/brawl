#include <mu/wifi/mu_wifi_interface.h>
#include <nt/nt_net_report.h>
#include <nt/nt_offline.h>
#include <string.h>
#include <types.h>

class DWCAllocType {
public:
    static void* ntAlloc(int, u32, u32);
    static void ntFree(int, void*);
};

class gfMemoryPool {
public:
    static void* alloc(void* heapAddr, size_t size, int align);
};

struct OSMutex;
extern "C" void OSLockMutex(OSMutex* mutex);
extern "C" void OSUnlockMutex(OSMutex* mutex);
extern OSMutex lbl_8049EC78;

extern "C" void fn_80025F2C(void* heap, void* ptr);

struct unk805A0434 {
    u8 unk0[0x353C];
    void* unk353C;
};
extern unk805A0434* lbl_805A0434;

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
        fn_80025F2C(lbl_805A0434->unk353C, arg1);
        OSUnlockMutex(&lbl_8049EC78);
    }
}


void netReport0(const char* fmt, ...) {
}

class NtMatching {
public:
    // HYPOTHESIS: the matching manager NtTr resets (only the methods its callers use are declared).
    void watchResetNodeMissMatch();
    // HYPOTHESIS: starts the save of the user data (the name of the Wi-Fi user) in a thread, when the network state
    // allows it.
    static void thSaveUserDataNow();
    bool deleteFriend(int index);
    void freeDisplayOrder();
    static void updateServerCallback(int error, int isChanged, void* param);
    static void changetFriendStatusCallback(int index, u8 status, const char* statusString, void* param);
    // HYPOTHESIS: the final argument selects updates to the local display order.
    bool deleteDisplayOrder(int index, bool updateLocal);

    u8 unk0[0x12];
    u8 unk12[0x40];
    u8 unk52[0x33BF];
    bool unk3411;
    bool unk3412;
    u8 unk3413[0xE];
    s8 unk3421;
};

extern s32 gUnk805a03d8;
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
