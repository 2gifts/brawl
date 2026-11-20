#include <gf/gf_heap_manager.h>
#include <gm/gm_global.h>
#include <if/if_snap_save.h>
#include <nt/nt_etc_file.h>
#include <nt/nt_etc_global.h>
#include <sr/sr_getappname.h>
#include <string.h>
#include <types.h>

extern "C" u32 NETCalcCRC32(const void* data, u32 len);

u8 nteSection::getDataType() {
    nteSectionData* data = m_data;
    if (data == NULL) {
        return 4;
    }
    return data->m_dataType;
}

u8 nteSection::getStateType() {
    nteSectionData* data = m_data;
    if (data == NULL) {
        return 4;
    }
    return data->m_stateType;
}

s64 nteSection::getCreated() {
    nteSectionData* data = m_data;
    if (data == NULL) {
        return 0;
    }
    return data->m_created;
}

bool nteSection::setOwnState(s64 created, u32 state) {
    GameGlobal* global;
    nteSectionData* data = m_data;
    if (data == NULL) {
        return false;
    }
    if (state >= 4) {
        return false;
    }
    data->m_wiiId = nteGlobal::getWiiID();
    global = g_GameGlobal;
    data->m_region = (global->getDistributionType() << 5) | global->getLanguage();
    data->m_stateType = state;
    data->m_created = created;
    data->m_crc = 0xDEADBEEF;
    data->m_crc = NETCalcCRC32(data, data->m_dataSize + 0x20);
    return true;
}

nteFile::nteFile(Heaps::HeapType heap) {
    m_heap = heap;
    m_count = 0;
}

nteFile::~nteFile() {
    for (u32 i = 0; i < m_count; i++) {
        if (m_entries[i].m_data != NULL) {
            gfHeapManager::free(m_entries[i].m_data);
            m_entries[i].m_data = NULL;
        }
    }
}

u32 nteFile::getDataFileSize() {
    u32 size = m_count * 12 + 8;
    for (u32 i = 0; i < m_count; i++) {
        size += m_entries[i].m_dataSize;
        size += 0x20;
    }
    u32 rem = size & 0xF;
    if (rem != 0) {
        size += 0x10 - rem;
    }
    return size;
}

nteFileDataPhoto::~nteFileDataPhoto() {
    if (unkC_ptr != NULL) {
        gfHeapManager::free(unkC_ptr);
    }
}

void nteFileDataPhoto::setData(void* data, u32 size) {
    if (data == NULL || size == 0) {
        return;
    }
    if (unk10 != size) {
        u8* buffer = static_cast<u8*>(gfHeapManager::alloc(unk4, size + 0x34, 32));
        if (buffer != NULL) {
            if (unkC_ptr != NULL) {
                memcpy(buffer, unkC_ptr, 0x34);
            } else {
                memset(buffer, 0, 0x34);
            }
            memcpy(buffer + 0x34, data, size);
        }
        if (unkC_ptr != NULL) {
            gfHeapManager::free(unkC_ptr);
        }
        unkC_ptr = buffer;
        if (buffer != NULL) {
            unk10 = size;
        } else {
            unk10 = 0;
        }
    } else if (unkC_ptr != NULL) {
        memcpy(unkC_ptr + 0x34, data, size);
    }
}

nteFileDataStage::nteFileDataStage(Heaps::HeapType heap) {
    unkC = static_cast<u8*>(gfHeapManager::alloc(heap, 0x34, 32));
    if (unkC != NULL) {
        memset(unkC, 0, 0x34);
    }
    unk10 = 0;
    unk0 = 3;
    unk4 = heap;
}

nteFileDataStage::~nteFileDataStage() {
    if (unkC != NULL) {
        gfHeapManager::free(unkC);
    }
}

void nteFileDataStage::setData(void* data, u32 size) {
    if (data == NULL || size == 0) {
        return;
    }
    if (unk10 != size) {
        u8* buffer = static_cast<u8*>(gfHeapManager::alloc(unk4, size + 0x34, 32));
        if (buffer != NULL) {
            if (unkC != NULL) {
                memcpy(buffer, unkC, 0x34);
            } else {
                memset(buffer, 0, 0x34);
            }
            memcpy(buffer + 0x34, data, size);
        }
        if (unkC != NULL) {
            gfHeapManager::free(unkC);
        }
        unkC = buffer;
        if (buffer != NULL) {
            unk10 = size;
        } else {
            unk10 = 0;
        }
    } else if (unkC != NULL) {
        memcpy(unkC + 0x34, data, size);
    }
}

nteFileDataSetting::~nteFileDataSetting() {
    if (unkC != NULL) {
        gfHeapManager::free(unkC);
    }
}

u64 nteSection::getWiiId() {
    nteSectionData* data = m_data;
    if (data == NULL) {
        return 0;
    }
    return data->m_wiiId;
}

u32 nteFile::getSections_Direct(const u8* buffer) {
    u32 count = buffer[4];
    if (count > 5) {
        count = 0;
    }
    return count;
}

nteSectionData* nteFile::getSectionData_Direct(u8* buffer, u32 index) {
    u32 count = getSections_Direct(buffer);
    if (count < (u8)index) {
        return NULL;
    }
    const u32* entry = (const u32*)buffer + index * 3;
    if (NETCalcCRC32(buffer + entry[2], entry[3]) != entry[4]) {
        return NULL;
    }
    return (nteSectionData*)(buffer + entry[2]);
}

bool nteFile::addSection(nteSectionData* section) {
    if (section == NULL) {
        return false;
    }
    bool success = false;
    if ((s32)m_count < 5) {
        u32 size = section->m_dataSize;
        if (size == 0) {
            return false;
        }
        nteFileEntry* entry = &m_entries[m_count];
        entry->m_data = gfHeapManager::alloc(m_heap, size, 32);
        if (entry->m_data == NULL) {
            return false;
        }
        memcpy(entry, section, sizeof(nteSectionData));
        memcpy(entry->m_data, section + 1, size);
        m_count++;
        success = true;
    }
    return success;
}

u32 nteSection::getSize() {
    nteSectionData* data = m_data;
    if (data == NULL) {
        return 0;
    }
    return data->m_dataSize + 0x20;
}

nteFileDataPhoto::nteFileDataPhoto(Heaps::HeapType heap) {
    unkC_ptr = static_cast<u8*>(gfHeapManager::alloc(heap, 0x34, 32));
    if (unkC_ptr != NULL) {
        memset(unkC_ptr, 0, 0x34);
    }
    unk10 = 0;
    unk0_word = 1;
    unk4 = heap;
}

nteFileDataReplay::nteFileDataReplay(Heaps::HeapType heap) {
    unkC = static_cast<u8*>(gfHeapManager::alloc(heap, 0x34, 32));
    if (unkC != NULL) {
        memset(unkC, 0, 0x34);
    }
    unk10 = 0;
    unk0 = 2;
    unk4 = heap;
}

void nteFileDataReplay::setData(void* data, u32 size) {
    if (data == NULL || size == 0) {
        return;
    }
    if (unk10 != size) {
        u8* buffer = static_cast<u8*>(gfHeapManager::alloc(unk4, size + 0x34, 32));
        if (buffer != NULL) {
            if (unkC != NULL) {
                memcpy(buffer, unkC, 0x34);
            } else {
                memset(buffer, 0, 0x34);
            }
            memcpy(buffer + 0x34, data, size);
        }
        if (unkC != NULL) {
            gfHeapManager::free(unkC);
        }
        unkC = buffer;
        if (buffer != NULL) {
            unk10 = size;
        } else {
            unk10 = 0;
        }
    } else if (unkC != NULL) {
        memcpy(unkC + 0x34, data, size);
    }
}


// TODO: clean up, give variables real names
extern "C" u32 fn_80153810(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x0);
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" u32 fn_80154278(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x10);
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" u32 fn_80154280(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0xc);
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" u32 fn_80154294(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0xc);
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" u32 fn_801542A8(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0xc);
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" u32 fn_801542BC(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0xc);
}

// TODO: clean up, give variables real names
namespace jj_80154288 {
extern "C" void* fn_80154288(void* arg0) {
    return *reinterpret_cast<unsigned char**>(static_cast<unsigned char*>(arg0) + 0x10) + 0x34;
}
}

namespace jj_8015429C {
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void* fn_8015429C(void* arg0) {
    unsigned char* p = *reinterpret_cast<unsigned char**>(static_cast<unsigned char*>(arg0) + 0x10);
    return p + 0x34;
}
}

namespace jj_801542B0 {
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void* fn_801542B0(void* object) {
    return *reinterpret_cast<unsigned char**>(static_cast<char*>(object) + 0x10) + 0x34;
}
}

namespace jj_80153488 {
// TODO: clean up, give variables real names
// TODO: clean up, give variables real names
extern "C" void fn_80153488(int* arg0) {
    arg0[0] = 0;
    *(unsigned char*)(arg0 + 1) = 0;
}
}

namespace jj_80153498 {

extern "C" void __dl__FPv(void*);

struct Fn80153498Obj {
    void* unk0;
    u8 unk4;
};

// TODO: clean up, give variables real names
// TODO: clean up, give variables real names
extern "C" void* fn_80153498(Fn80153498Obj* p, int flag) {
    if (p != NULL) {
        if (p->unk4 != 0) {
            void* ptr = p->unk0;
            if (ptr != NULL) {
                gfHeapManager::free(ptr);
            }
        }
        if (flag > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

namespace jj_80152BDC {
extern "C" {
extern char lbl_80461690[];
void __dl__FPv(void*);
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void* fn_80152BDC(void* self, int flag) {
    if (self) {
        void* ptr = *(void**)((char*)self + 0xC);
        *(void**)((char*)self + 0x8) = lbl_80461690;
        if (ptr) {
            gfHeapManager::free(ptr);
        }
        if (flag > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}
}

// TODO: clean up, give variables real names
namespace jj_80153504 {
extern "C" void free__13gfHeapManagerFPv(void*);
extern "C" u32 NETCalcCRC32(void* p, s32 size);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" s32 fn_80153504(u8* arg0, u8* arg1) {
    if (arg0[4] != 0) {
        void* p = *(void**)arg0;
        if (p != 0) free__13gfHeapManagerFPv(p);
    }
    arg0[4] = 0;
    *(u32*)arg0 = 0;
    s32 v = 0;
    if (arg1 == 0) {
        v = 0;
    } else {
        if (arg1[0x17] != 0xdb) {
            v = 0;
        } else if (arg1[0x15] >= 4) {
            v = 0;
        } else if (arg1[0x16] >= 4) {
            v = 0;
        } else {
            u32 size = *(u32*)(arg1 + 0x1C);
            if (size >= 0x80000) {
                v = 0;
            } else {
                u32 old = *(u32*)(arg1 + 0x10);
                *(u32*)(arg1 + 0x10) = 0xDEADBEEF;
                u32 crc = NETCalcCRC32(arg1, (s32)(size + 0x20));
                *(u32*)(arg1 + 0x10) = old;
                v = (s32)((u32)__cntlzw(crc - old) >> 5);
            }
        }
    }
    if (v == 0) return 0;
    *(void**)arg0 = arg1;
    return 1;
}
}

// TODO: clean up, give variables real names
namespace jj_80153610 {
struct Object_vptr_8_base { u32 kind; u32 pad; };
class Object_vptr_8 : public Object_vptr_8_base {
public:
virtual void slot0();
virtual void* slot1();
virtual u32 slot2();
};
struct Hdr { u64 time; u64 id; u32 crc; u8 flags; u8 kind; u8 b16; u8 b17; u32 size; u32 csize; };
struct Holder { Hdr* buf; u8 owned; };
extern "C" {
u64 getWiiID__9nteGlobalFv();
u8 getLanguage__10GameGlobalFv(GameGlobal*);
u8 getDistributionType__10GameGlobalFv(GameGlobal*);
u32 fn_80204FC8(void*, u32, void*, void*, int);
u32 NETCalcCRC32(void*, u32);
// TODO: clean up, give variables real names
// TODO: clean up, give variables real names
int fn_80153610(Holder* self, Object_vptr_8* src, u64 id, Heaps::HeapType heap, u8 b, void* work) {
Hdr* h;
GameGlobal* g;
if (self->owned) {
if (self->buf) gfHeapManager::free(self->buf);
self->owned = 0;
}
self->buf = 0;
if (!src) return 0;
void* data = src->slot1();
u32 size = src->slot2();
if (!data || !size) return 0;
self->buf = (Hdr*)gfHeapManager::alloc(heap, size + 0x20, 0x20);
if (!self->buf) return 0;
void* w = work;
if (!work) {
w = gfHeapManager::alloc(heap, 0x2400, 0x20);
if (!w) {
gfHeapManager::free(self->buf);
self->buf = 0;
return 0;
}
}
self->owned = 1;
h = self->buf;
u8* dst = (u8*)self->buf + 0x20;
h->time = getWiiID__9nteGlobalFv();
g = g_GameGlobal;
u8 lang = getLanguage__10GameGlobalFv(g);
u8 hi = getDistributionType__10GameGlobalFv(g);
h->flags = (hi << 5) | lang;
h->kind = src->kind;
h->b16 = b;
h->id = id;
h->size = size;
h->b17 = 0xdb;
u32 cs = fn_80204FC8(data, size, dst, w, 1);
if (!cs) { cs = size; memcpy(dst, data, size); }
h->csize = cs;
h->crc = 0xDEADBEEF;
h->crc = NETCalcCRC32(h, cs + 0x20);
if (!work && w) gfHeapManager::free(w);
return 1;
}
}
}

// TODO: clean up, give variables real names
namespace jj_80152FA4 {
extern "C" {
void setContribute__9nteGlobalFUc(unsigned char);
void setIsInfinityContribute__9nteGlobalFb(unsigned char);
void setContribute_Start__9nteGlobalFUsUcUc(unsigned short, unsigned char, unsigned char);
void setContribute_End__9nteGlobalFUsUcUc(unsigned short, unsigned char, unsigned char);
void setWatch_Start__9nteGlobalFUsUcUc(unsigned short, unsigned char, unsigned char);
void setWatch_End__9nteGlobalFUsUcUc(unsigned short, unsigned char, unsigned char);
void setDeliv_Start__9nteGlobalFUsUcUc(unsigned short, unsigned char, unsigned char);
void setDeliv_End__9nteGlobalFUsUcUc(unsigned short, unsigned char, unsigned char);
void setUploadSizeLimit__9nteGlobalFUc(unsigned char);
void setEnableUploadCharacter__9nteGlobalFUx(unsigned int, unsigned int);
void setEnableUploadStage__9nteGlobalFUx(unsigned int, unsigned int);
void setCollectionLifetime__9nteGlobalFUc(unsigned char);
void* getUploadSetting__9nteGlobalFv(void);
}

union Grp {
    unsigned int w;
    struct {
        unsigned short h;
        unsigned char a;
        unsigned char b;
    } p;
};

struct Src {
    unsigned char f0;
    unsigned char f1;
    unsigned char f2;
    unsigned char f3;
    Grp g4;
    Grp g8;
    Grp gC;
    Grp g10;
    Grp g14;
    Grp g18;
    unsigned int w1C;
    unsigned int w20;
    unsigned int w24;
    unsigned int w28;
    unsigned char t[0x37];
};

struct Obj {
    char pad[0xC];
    Src* src;
};

#define COPY(n) d[0x11 + n] = arg0->src->t[n];

// TODO: clean up, give variables real names
// TODO: clean up, give variables real names
extern "C" void fn_80152FA4(Obj* arg0) {
    unsigned short var_r31;
    unsigned char var_r30;
    unsigned char var_r29;
    int f;

    if (!((arg0->src->f0 >> 7) & 1)) {
        setContribute__9nteGlobalFUc(arg0->src->f0);
    }
    if (!((arg0->src->f1 >> 7) & 1)) {
        setIsInfinityContribute__9nteGlobalFb(arg0->src->f1 != 0);
    }
    if (arg0->src->g4.w >> 31) {
        f = 0;
    } else {
        var_r31 = arg0->src->g4.p.h;
        f = 1;
        var_r30 = arg0->src->g4.p.a;
        var_r29 = arg0->src->g4.p.b;
    }
    if (f != 0) {
        setContribute_Start__9nteGlobalFUsUcUc(var_r31, var_r30, var_r29);
    }
    if (arg0->src->g8.w >> 31) {
        f = 0;
    } else {
        var_r31 = arg0->src->g8.p.h;
        f = 1;
        var_r30 = arg0->src->g8.p.a;
        var_r29 = arg0->src->g8.p.b;
    }
    if (f != 0) {
        setContribute_End__9nteGlobalFUsUcUc(var_r31, var_r30, var_r29);
    }
    if (arg0->src->gC.w >> 31) {
        f = 0;
    } else {
        var_r31 = arg0->src->gC.p.h;
        f = 1;
        var_r30 = arg0->src->gC.p.a;
        var_r29 = arg0->src->gC.p.b;
    }
    if (f != 0) {
        setWatch_Start__9nteGlobalFUsUcUc(var_r31, var_r30, var_r29);
    }
    if (arg0->src->g10.w >> 31) {
        f = 0;
    } else {
        var_r31 = arg0->src->g10.p.h;
        f = 1;
        var_r30 = arg0->src->g10.p.a;
        var_r29 = arg0->src->g10.p.b;
    }
    if (f != 0) {
        setWatch_End__9nteGlobalFUsUcUc(var_r31, var_r30, var_r29);
    }
    if (arg0->src->g14.w >> 31) {
        f = 0;
    } else {
        var_r31 = arg0->src->g14.p.h;
        f = 1;
        var_r30 = arg0->src->g14.p.a;
        var_r29 = arg0->src->g14.p.b;
    }
    if (f != 0) {
        setDeliv_Start__9nteGlobalFUsUcUc(var_r31, var_r30, var_r29);
    }
    if (arg0->src->g18.w >> 31) {
        f = 0;
    } else {
        var_r31 = arg0->src->g18.p.h;
        f = 1;
        var_r30 = arg0->src->g18.p.a;
        var_r29 = arg0->src->g18.p.b;
    }
    if (f != 0) {
        setDeliv_End__9nteGlobalFUsUcUc(var_r31, var_r30, var_r29);
    }
    if (!((arg0->src->f3 >> 7) & 1)) {
        setUploadSizeLimit__9nteGlobalFUc(arg0->src->f3);
    }
    {
        unsigned int w1 = arg0->src->w1C;
        unsigned int w2 = arg0->src->w20;
        if (!(w1 >> 31)) {
            setEnableUploadCharacter__9nteGlobalFUx(w1, w2);
        }
    }
    {
        unsigned int w1 = arg0->src->w24;
        unsigned int w2 = arg0->src->w28;
        if (!(w1 >> 31)) {
            setEnableUploadStage__9nteGlobalFUx(w1, w2);
        }
    }
    if (!((arg0->src->f2 >> 7) & 1)) {
        setCollectionLifetime__9nteGlobalFUc(arg0->src->f2);
    }
    unsigned char* d = (unsigned char*)getUploadSetting__9nteGlobalFv();
    COPY(0) COPY(1) COPY(2) COPY(3) COPY(4) COPY(5) COPY(6) COPY(7)
    COPY(8) COPY(9) COPY(10) COPY(11) COPY(12) COPY(13) COPY(14) COPY(15)
    COPY(16) COPY(17) COPY(18) COPY(19) COPY(20) COPY(21) COPY(22) COPY(23)
    COPY(24) COPY(25) COPY(26) COPY(27) COPY(28) COPY(29) COPY(30) COPY(31)
    COPY(32) COPY(33) COPY(34) COPY(35) COPY(36) COPY(37) COPY(38) COPY(39)
    COPY(40) COPY(41) COPY(42) COPY(43) COPY(44) COPY(45) COPY(46) COPY(47)
    COPY(48) COPY(49) COPY(50) COPY(51) COPY(52) COPY(53) COPY(54)
}
}

