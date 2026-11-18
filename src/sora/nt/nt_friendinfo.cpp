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
extern "C" void fn_8035161C(DWCAccUserData* userdata);

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
    if (!getInstance()->isModified()) {
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

void ntFriendInfo::unlock() {
    OSUnlockMutex(&lbl_8049ECA0.m_mutex);
}

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

bool ntFriendInfo::getSaveHeader(void** data) {
    if (data == 0) {
        return false;
    }
    ntFriendInfo* info = getInstance();
    if (!info->isModified()) {
        return false;
    }
    fn_8035161C(info->getUserData());
    info->unk8 = false;
    info->unk0->unk0_0 = 0x2804;
    u16 rev = srSystemGetRevisionNumber();
    UnkFriendInfo* header = info->unk0;
    header->unk4 = rev;
    info->unk0->unk6 = 0;
    info->unk0->unk2 = 0;
    u16 crc = NETCalcCRC16((const char*)info->unk0, 0x2804);
    info->unk0->unk2 = crc;
    *data = info->unk0;
    return true;
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

bool ntFriendInfo::getSaveData(void** data, int* size) {
    if (!isModified()) {
        return false;
    }
    fn_8035161C(getUserData());
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

// TODO: clean up, give variables real names
namespace jj_8014C75C {
struct Hdr { u16 magic; u16 crc; u16 rev; u8 pad6; u8 pad7; };
struct Obj { Hdr* hdr; int size; u8 flag; u8 f9; };
extern "C" {
s16 NETCalcCRC16(void*, s32);
s32 fn_80351304(void*);
u32 fn_80351610(void*);
void fn_8035161C(void*);
s16 srSystemGetRevisionNumber__Fv(void);
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void* alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(int, unsigned long);
void* memset(void*, int, unsigned long);
}
extern Obj* lbl_805A044C;

static inline void adjust(unsigned int* field, unsigned int remainder) {
    if (remainder) *(long*)field += 32 - remainder;
}

extern "C" s32 fn_8014C75C(Hdr** out) {
    s32 r30;
    s32 r5;
    if (out == 0) return 0;
    if (lbl_805A044C == 0) {
        Obj* o = (Obj*)__nw__FUlQ25Heaps8HeapType(12, 0x2d);
        if (o) {
            unsigned int* sizeField = (unsigned int*)((char*)o + 4);
            *sizeField = 0x2804;
            adjust(sizeField, *sizeField & 31);
            o->hdr = (Hdr*)alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(0x2d, o->size);
            memset(o->hdr, 0, o->size);
            o->flag = 0;
            o->f9 = 0;
        }
        lbl_805A044C = o;
    }
    Obj* self = lbl_805A044C;
    if (self->hdr == 0) {
        r30 = 0;
    } else {
        r30 = self->flag;
        if (r30 == 0 && fn_80351304((u8*)self->hdr + 0x10) != 0 &&
            fn_80351610((u8*)self->hdr + 0x10) != 0) {
            r30 = 1;
        }
    }
    if (r30 == 0) {
        r5 = 0;
    } else {
        fn_8035161C((u8*)self->hdr + 0x10);
        self->flag = 0;
        self->hdr->magic = 0x2804;
        s16 rev = srSystemGetRevisionNumber__Fv();
        Hdr* t = self->hdr;
        t->rev = rev;
        self->hdr->pad6 = 0;
        self->hdr->crc = 0;
        s16 c = NETCalcCRC16(self->hdr, 0x2804);
        r5 = 1;
        self->hdr->crc = c;
        *out = self->hdr;
    }
    return r5;
}
}

