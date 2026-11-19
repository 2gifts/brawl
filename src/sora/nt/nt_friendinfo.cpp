#include <gf/gf_heap_manager.h>
#include <gf/gf_sys_recorder2.h>
#include <nt/nt_offline.h>
#include <nt/nt_report.h>
#include <sr/sr_revision.h>
#include <revolution/NET/crc.h>
#include <revolution/OS/OSMutex.h>
#include <string.h>
#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
// gfMutex is identified by the global object's registered destructor.
class gfMutex {
public:
    gfMutex() { OSInitMutex(&m_mutex); }
    ~gfMutex();
    OSMutex m_mutex;
};
gfMutex lbl_8049ECA0 __attribute__((aligned(8)));
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
ntFriendInfo* lbl_805A044C __attribute__((aligned(4))) = 0;


extern "C" int DWC_CheckUserData(const DWCAccUserData* userdata);
extern "C" void DWC_CreateUserData(DWCAccUserData* userdata);
extern "C" int DWC_CheckDirtyFlag(const DWCAccUserData* userdata);
// HYPOTHESIS: clears the dirty flag of the account data.
extern "C" void DWC_ClearDirtyFlag(DWCAccUserData* userdata);

// TODO: clean up, give variables real names
static inline bool getSaveHeaderInline(void** data) {
    bool result;
    if (data == 0) {
        return false;
    }
    ntFriendInfo* info = ntFriendInfo::getInstance();
    int modified;
    if (info->unk0 == 0) {
        modified = 0;
    } else {
        modified = info->unk8;
        if (modified == 0 && DWC_CheckUserData(info->getUserData()) != 0 && DWC_CheckDirtyFlag(info->getUserData()) != 0) {
            modified = 1;
        }
    }
    if (modified == 0) {
        result = false;
    } else {
        DWC_ClearDirtyFlag(info->getUserData());
        info->unk8 = false;
        info->unk0->unk0_0 = 0x2804;
        u16 rev = srSystemGetRevisionNumber();
        UnkFriendInfo* header = info->unk0;
        header->unk4 = rev;
        info->unk0->unk6 = 0;
        info->unk0->unk2 = 0;
        u16 crc = NETCalcCRC16((const char*)info->unk0, 0x2804);
        result = true;
        info->unk0->unk2 = crc;
        *data = info->unk0;
    }
    return result;
}

static inline u8 isModifiedInline(ntFriendInfo* info) {
    u8 modified;
    if (info->unk0 == 0) {
        modified = 0;
    } else {
        modified = info->unk8;
        if (modified == 0 && DWC_CheckUserData(info->getUserData()) != 0 && DWC_CheckDirtyFlag(info->getUserData()) != 0) {
            modified = 1;
        }
    }
    return modified;
}

static inline u8 isReadyInline() {
    return isModifiedInline(ntFriendInfo::getInstance());
}

ntFriendInfoSave::ntFriendInfoSave()
    : gfTask("ntFriendInfoSave", Category_None, 14, 6, true), unk40(0), unk44(false) {
    unk2C_b1 = false;
}

ntFriendInfoSave::~ntFriendInfoSave() {
    if (unk40) {
        gfSysRecorder2::dropInstance();
        unk40 = 0;
    }
}

// HYPOTHESIS: copies the Wi-Fi friend data to the buffer of a new gfSysRecorder2 and starts to save it.
// TODO: clean up, give variables real names
bool ntFriendInfoSave::startSave(Heaps::HeapType heap, bool exitWhenDone) {
    if (unk40 != 0) {
        return false;
    }
    ++s_savingCount;
    unk44 = exitWhenDone;
    int ok;
    if (!gfSysRecorder2::isEnable(2)) {
        ok = 0;
    } else if (lbl_805A044C == 0) {
        ok = 0;
    } else if (!NtBase::isMatchStatusOnline()) {
        ok = 0;
    } else {
        ok = isReadyInline() != 0;
    }
    if (ok) {
        unk40 = gfSysRecorder2::createInstance(heap);
        if (unk40 != 0) {
            void* buffer = unk40->getBuffer(2);
            if (buffer != 0) {
                void* data = 0;
                if (getSaveHeaderInline(&data)) {
                    memcpy(buffer, data, 0x2804);
                    if (unk40->saveWifi()) {
                        return true;
                    }
                }
            }
        }
    }
    if (unk40 != 0) {
        gfSysRecorder2::dropInstance();
        unk40 = 0;
    }
    if (unk44) {
        exit();
    }
    --s_savingCount;
    return false;
}

void ntFriendInfoSave::processDefault() {
    if (unk40 && !unk40->m_busy) {
        gfSysRecorder2::dropInstance();
        unk40 = 0;
        if (unk44) exit();
        --s_savingCount;
    }
}

void ntFriendInfo::setSaveData(const void* data, u32 size) {
    unk8 = true;
    if (size >= 0x2804) {
        memcpy(unk0, data, 0x2804);
        if (unk0->unk0_0 == 0x2804) {
            u16 crc = unk0->unk2;
            unk0->unk2 = 0;
            if (crc == NETCalcCRC16((const char*)unk0, 0x2804)) unk8 = false;
        }
    }
    if (unk8) memset(unk0, 0, 0x2804);
    unk9 = true;
}

bool ntFriendInfo::isValid(const DWCAccUserData* data) {
    return DWC_CheckUserData(data) == 1;
}


// HYPOTHESIS: the save data is aligned to 32 bytes.
static inline void alignSize32(u32* size) {
    u32 rem = *size & 31;
    if (rem) {
        *size += 32 - rem;
    }
}

// HYPOTHESIS: true when the save of the Wi-Fi friend data has to be started.
// TODO: clean up, give variables real names
bool ntFriendInfo::isSaveNeeded() {
    if (!gfSysRecorder2::isEnable(2)) {
        return false;
    }
    if (lbl_805A044C == 0) {
        return false;
    }
    if (!NtBase::isMatchStatusOnline()) {
        return false;
    }
    if (!isReadyInline()) {
        return false;
    }
    return true;
}

u32 ntFriendInfo::getSaveSize() {
    return 0x2804;
}

// TODO: clean up, give variables real names
extern "C" void fn_8014C960() {
}

void ntFriendInfo::lock() {
    OSLockMutex(&lbl_8049ECA0.m_mutex);
}

// TODO: clean up, give variables real names
void ntFriendInfo::unlock() {
    OSUnlockMutex(&lbl_8049ECA0.m_mutex);
}

// TODO: clean up, give variables real names
ntFriendInfo* ntFriendInfo::getInstance() {
    if (lbl_805A044C == 0) {
        ntFriendInfo* info = new ((Heaps::HeapType)0x2D) ntFriendInfo;
        if (info != 0) {
            info->unk4 = 0x2804;
            alignSize32(&info->unk4);
            info->unk0 = (UnkFriendInfo*)gfHeapManager::alloc((Heaps::HeapType)0x2D, info->unk4);
            memset(info->unk0, 0, info->unk4);
            info->unk8 = false;
            info->unk9 = false;
        }
        lbl_805A044C = info;
    }
    return lbl_805A044C;
}

// TODO: clean up, give variables real names
bool ntFriendInfo::getSaveHeader(void** data) {
    bool result;
    if (data == 0) {
        return false;
    }
    ntFriendInfo* info = ntFriendInfo::getInstance();
    int modified;
    if (info->unk0 == 0) {
        modified = 0;
    } else {
        modified = info->unk8;
        if (modified == 0 && DWC_CheckUserData(info->getUserData()) != 0 && DWC_CheckDirtyFlag(info->getUserData()) != 0) {
            modified = 1;
        }
    }
    if (modified == 0) {
        result = false;
    } else {
        DWC_ClearDirtyFlag(info->getUserData());
        info->unk8 = false;
        info->unk0->unk0_0 = 0x2804;
        u16 rev = srSystemGetRevisionNumber();
        UnkFriendInfo* header = info->unk0;
        header->unk4 = rev;
        info->unk0->unk6 = 0;
        info->unk0->unk2 = 0;
        u16 crc = NETCalcCRC16((const char*)info->unk0, 0x2804);
        result = true;
        info->unk0->unk2 = crc;
        *data = info->unk0;
    }
    return result;
}

// HYPOTHESIS: true when the data was changed since it was saved.
bool ntFriendInfo::isModified() {
    bool modified;
    if (unk0 == 0) {
        modified = false;
    } else {
        modified = unk8;
        if (!modified && DWC_CheckUserData(getUserData()) != 0 && DWC_CheckDirtyFlag(getUserData()) != 0) {
            modified = true;
        }
    }
    return modified;
}

// TODO: clean up, give variables real names
bool ntFriendInfo::getSaveData(void** data, int* size) {
    if (!isModified()) {
        return false;
    }
    DWC_ClearDirtyFlag(getUserData());
    unk8 = false;
    unk0->unk0_0 = 0x2804;
    u16 rev = srSystemGetRevisionNumber();
    UnkFriendInfo* header = unk0;
    header->unk4 = rev;
    unk0->unk6 = 0;
    unk0->unk2 = 0;
    unk0->unk2 = NETCalcCRC16((const char*)unk0, 0x2804);
    *data = unk0;
    *size = 0x2804;
    return true;
}

void ntFriendInfo::setDirtyFlag(bool dirty) {
    unk8 = dirty;
}

UnkFriendInfo* ntFriendInfo::getFriendInfo() {
    if (unk9 == 0) {
        return 0;
    }
    return unk0;
}

DWCAccUserData* ntFriendInfo::getUserData() {
    return (DWCAccUserData*)unk0->unk10;
}

bool ntFriendInfo::resetData() {
    lock();
    lock();
    memset(unk0, 0, 0x2804);
    unk8 = true;
    unlock();
    DWC_CreateUserData(getUserData());
    unlock();
    return true;
}
