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
