#include <mu/wifi/mu_wifi_interface.h>
#include <nt/network.h>
#include <nt/nt_match.h>
#include <nt/nt_net_report.h>
#include <nt/nt_offline.h>
#include <nt/nt_report.h>
#include <revolution/OS/OSTime.h>
#include <string.h>
#include <types.h>

class gfMemoryPool {
public:
    static void* alloc(void* heapAddr, size_t size, int align);
};

struct OSMutex;
extern "C" void OSLockMutex(OSMutex* mutex);
extern "C" void OSUnlockMutex(OSMutex* mutex);
extern OSMutex lbl_8049EC78;

extern "C" void fn_80025F2C(void* heap, void* ptr);

extern NtMatching* lbl_805A0434;

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

extern "C" void DWC_CreateFriendKeyToken(UnkFriendInfo::FriendData* token, u64 friendKey);
extern "C" int DWC_IsEqualFriendData(const UnkFriendInfo::FriendData* data1, const UnkFriendInfo::FriendData* data2);
extern "C" int DWC_IsValidFriendData(const UnkFriendInfo::FriendData* data);
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
