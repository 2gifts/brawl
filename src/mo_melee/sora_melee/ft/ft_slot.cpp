#include <ft/ft_manager.h>
#include <ft/ft_slot.h>
#include <ft/ft_slot_manager.h>
#include <so/so_array.h>
#include <types.h>

// TODO: the loading, removal and Kirby copy resource members.

extern "C" {
// HYPOTHESIS: ftSlot::process (unnamed in sora_melee).
void fn_27_120304(ftSlot* slot);
}

// HYPOTHESIS: a view of the slot layout: the request flag at +0x4, the per-instance records (0x54 bytes from +0x8) and
// the eight Kirby copy resource records (0x30 bytes from +0x310).
struct ftSlotKirbyResourceView {
    struct {
        u32 m_resId;
        u32 unk4;
    } m_res[4];
    int m_kind; // +0x20
    int unk24;
    int m_state; // +0x28, -1 when unused
    bool m_isCopySetup; // +0x2C
    char _2d[3];
};

// HYPOTHESIS: the requested resources (an array at +0x178 of records with their flags at +0x4) and the instance count at
// +0x158.
struct ftSlotResourceView {
    u32 m_resId;
    u32 m_flags;
};

struct ftSlotView {
    void* m_vtable;
    int m_isRequested; // +0x4
    struct {
        ftKind m_kind; // +0x8
        u32 m_flags; // +0xC
        char _8[0x4C];
    } m_instances[9];
    char _2fc[0x14];
    ftSlotKirbyResourceView m_kirbyResources[8]; // +0x310
};

struct ftSlotResourcesView {
    char _0[0x178];
    soArrayVector<ftSlotResourceView, 1> m_resources; // +0x178 (HYPOTHESIS: the capacity is not known)
};

static inline ftSlotView* getView(ftSlot* slot) {
    return reinterpret_cast<ftSlotView*>(slot);
}

// The first loaded resource id of a Kirby copy record (0xFFFF if none).
static inline u32 getKirbyResId(ftSlotView* view, u8 index) {
    for (int i = 0; i < 4; i++) {
        if (view->m_kirbyResources[index].m_res[i].m_resId != 0xFFFF) {
            return view->m_kirbyResources[index].m_res[i].m_resId;
        }
    }
    return 0xFFFF;
}

ftSlot::~ftSlot() { }

void ftSlot::pushItem(int index) {
    g_ftDataProvider->reqItem(getView(this)->m_instances[index].m_kind);
    getView(this)->m_isRequested = 1;
}

int ftSlot::getKirbyResourceCount() {
    int count = 0;
    for (u32 i = 0; i < 8; i++) {
        if (getView(this)->m_kirbyResources[(u8)i].m_state != -1 && getKirbyResId(getView(this), i) != 0xFFFF) {
            count++;
        }
    }
    return count;
}

int ftSlot::getKirbyResourceKind(int index) {
    int count = 0;
    for (u32 i = 0; i < 8; i++) {
        if (getView(this)->m_kirbyResources[(u8)i].m_state != -1 && getKirbyResId(getView(this), i) != 0xFFFF) {
            if (index == count) {
                return getView(this)->m_kirbyResources[(u8)i].m_kind;
            }
            count++;
        }
    }
    return -1;
}

void ftSlot::notifyKirbyCopySetup(int kind) {
    for (u32 i = 0; i < 8; i++) {
        if (getView(this)->m_kirbyResources[(u8)i].m_state != -1 && kind == getView(this)->m_kirbyResources[(u8)i].m_kind) {
            getView(this)->m_kirbyResources[(u8)i].m_isCopySetup = true;
        }
    }
}

void ftSlot::notifyKirbyCopyCancel(int kind) {
    for (u32 i = 0; i < 8; i++) {
        if (getView(this)->m_kirbyResources[(u8)i].m_state != -1 && kind == getView(this)->m_kirbyResources[(u8)i].m_kind) {
            getView(this)->m_kirbyResources[(u8)i].m_isCopySetup = false;
        }
    }
}

// HYPOTHESIS: the fighter manager flag bytes at +0x6C (bit 0x40: game started) and +0x6E (bit 0x04).
struct ftSlotManagerFlagsView {
    u8 unk80 : 1;
    u8 m_isGameStarted : 1;
    u8 unk20 : 6;
    u8 unk6d;
    u8 unk6e_f8 : 5;
    u8 unk6e_04 : 1;
    u8 unk6e_03 : 2;
};

static inline ftSlotManagerFlagsView* getManagerFlags() {
    return reinterpret_cast<ftSlotManagerFlagsView*>(reinterpret_cast<u8*>(g_ftManager) + 0x6C);
}

// Every loaded record of the kind (and of the state, unless -1) has its resources in place.
// HYPOTHESIS: +0x15E is a sound flag of the slot (ignored once the game runs) and +0x174 the slot's resource variation.
bool ftSlot::isReadyKirbyCopyResource(int kind, int state) {
    for (u32 i = 0; i < 8; i++) {
        ftSlotView* view = getView(this);
        if (view->m_kirbyResources[(u8)i].m_state != -1 && kind == view->m_kirbyResources[(u8)i].m_kind &&
            (state == -1 || state == view->m_kirbyResources[(u8)i].m_state)) {
            u8 checkSound = *(reinterpret_cast<u8*>(this) + 0x15E);
            ftSlotManagerFlagsView* flags = getManagerFlags();
            if ((bool)flags->m_isGameStarted == true && !flags->unk6e_04) {
                checkSound = 0;
            }
            if (!g_ftDataProvider->isFinishMask((ftKind)view->m_kirbyResources[(u8)i].m_kind,
                                                *reinterpret_cast<int*>(reinterpret_cast<u8*>(this) + 0x174),
                                                view->m_kirbyResources[(u8)i].m_state, view->m_kirbyResources[(u8)i].unk24,
                                                m_costumeId, checkSound)) {
                return false;
            }
        }
    }
    return true;
}

static inline const soArray<ftSlotResourceView>& getResources(ftSlot* slot) {
    return reinterpret_cast<ftSlotResourcesView*>(slot)->m_resources;
}

bool ftSlot::isLoaded(u32 flags) {
    for (int i = 0; (u8)i < *(reinterpret_cast<u8*>(this) + 0x158); i++) {
        if (flags & getView(this)->m_instances[(u8)i].m_flags) {
            return true;
        }
    }
    for (int i = 0; i < getResources(this).size(); i++) {
        if (flags & getResources(this).at(i).m_flags) {
            return true;
        }
    }
    return false;
}

ftSlotManager::~ftSlotManager() {
    delete[] m_slots;
    m_slots = NULL;
    m_slotCount = 0;
    g_ftSlotManager = NULL;
}

void ftSlotManager::process() {
    for (int i = 0; i < (int)m_slotCount; i++) {
        fn_27_120304(&m_slots[i]);
    }
}

// TODO: clean up, give variables real names
namespace jj_11F698 {
struct Entry { s32 a; s32 b; s32 c; char pad[0x24]; };
extern "C" {
s32 reqMask__14ftDataProviderF6ftKindiiUlUlQ25Heaps8HeapTypebb(s32, s32, s32, s32, s32, u8, s32, s32, s32);
extern s32 g_ftDataProvider;
void fn_27_11F698(u8* self, s32 a1, s32 a2, void* a3, s32 a4) {
    u8 i;
    s32 res;
    s32 off;
    u8* slot;
    s32 mask;
    s32 t;
    for (i = 0; i < 8; i++) {
        off = i * 0x30;
        slot = self + off;
        if (a2 == *(s32*)(slot + 0x330) && a1 == *(s32*)(slot + 0x338)) {
            if (self[0x15F] != 1) {
                res = 0;
                mask = ~*(s32*)(slot + 0x334) & 0xE00;
                if (mask != 0) {
                    t = mask & 0x600;
                    if (t != 0) {
                        res = reqMask__14ftDataProviderF6ftKindiiUlUlQ25Heaps8HeapTypebb(g_ftDataProvider, a2, *(s32*)(self + 0x174), *(s32*)(self + 0x174), t, self[0x15B], (s32)a3 + 0x20, *(s32*)(self + 0x168), 0);
                    }
                    if (mask & 0x800) {
                        res |= reqMask__14ftDataProviderF6ftKindiiUlUlQ25Heaps8HeapTypebb(g_ftDataProvider, a2, *(s32*)(self + 0x174), a1, 0x800, self[0x15B], *(s32*)(self + a4 * 0x14 + 0x490), *(s32*)(self + 0x168), 1);
                    }
                    (*(Entry*)(self + 0x330 + off)).b |= res;
                }
            } else {
                res = 0;
            }

            if (res != 0) {
                *(s32*)(self + 4) = 1;
                return;
            }
            if ((*(Entry*)(self + 0x330 + off)).c == -1) {
                *(s32*)(slot + 0x338) = a1;
            }
            return;
        }
    }
    for (i = 0; i < 8; i++) {
        u8* slot;
        s32 off;
        s32 res;
        off = i * 0x30;
        slot = self + off;
        if (*(s32*)(slot + 0x338) == -1) {
            if (self[0x15F] != 1) {
                res = 0;
                mask = ~*(s32*)(slot + 0x334) & 0xE00;
                if (mask != 0) {
                    t = mask & 0x600;
                    if (t != 0) {
                        res = reqMask__14ftDataProviderF6ftKindiiUlUlQ25Heaps8HeapTypebb(g_ftDataProvider, a2, *(s32*)(self + 0x174), *(s32*)(self + 0x174), t, self[0x15B], (s32)a3 + 0x20, *(s32*)(self + 0x168), 0);
                    }
                    if (mask & 0x800) {
                        res |= reqMask__14ftDataProviderF6ftKindiiUlUlQ25Heaps8HeapTypebb(g_ftDataProvider, a2, *(s32*)(self + 0x174), a1, 0x800, self[0x15B], *(s32*)(self + a4 * 0x14 + 0x490), *(s32*)(self + 0x168), 1);
                    }
                    (*(Entry*)(self + 0x330 + off)).b |= res;
                }
            } else {
                res = 0;
            }

            if (res != 0) {
                (*(Entry*)(self + 0x330 + off)).a = a2;
                *(s32*)(slot + 0x338) = a1;
                *(s32*)(self + 4) = 1;
            }
            return;
        }
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_11FD4C {
extern "C" void onKirbyResourceLoaded__9ftManagerFii(void*, int, int);
extern "C" void onKirbyResourceUnLoaded__9ftManagerFii(void*, int, int);
extern void* g_ftManager;

struct EntryPair {
    unsigned int value;
    int arg;
};

struct EntryBlock {
    char padding[0x310];
    EntryPair entries[4];
};

static inline u8 findEntry(char* self, int id) {
    for (u8 i = 0; i < *(u8*)(self + 0x158); ++i) {
        if (id == *(int*)(self + i * 0x54 + 8)) {
            return i;
        }
    }
    return 0xFF;
}

static inline unsigned int firstValue(EntryBlock* block) {
    for (int j = 0; j < 4; ++j) {
        if (block->entries[j].value != 0xFFFF) {
            return block->entries[j].value;
        }
    }
    return 0xFFFF;
}

static inline void removeValue(EntryBlock* block, unsigned int value, int arg) {
    for (unsigned int j = 0; j < 4; ++j) {
        if (arg == block->entries[j].arg) {
            block->entries[j].value = value;
            block->entries[j].arg = -1;
            return;
        }
    }
}

static inline void addValue(EntryBlock* block, unsigned int value, int arg) {
    for (int j = 0; j < 4; ++j) {
        if (block->entries[j].value == 0xFFFF) {
            block->entries[j].value = value;
            block->entries[j].arg = arg;
            return;
        }
    }
}

extern "C" void fn_27_11FD4C(
    char* self, int kind, int id, unsigned int field,
    unsigned int value, int arg
) {
    switch (kind) {
    case 8: {
        char* entry = self + findEntry(self, id) * 0x54 + 8;
        if (field == 0xC) {
            *(unsigned int*)(entry + 0x48) = value;
        } else if (field == 0xD) {
            *(unsigned int*)(entry + 0x44) = value;
        } else if (field & 0x80) {
            *(unsigned int*)(entry + 0x4C) = value;
        } else {
            *(unsigned int*)(entry + (u8)field * 4 + 0x14) = value;
        }
        break;
    }
    case 6:
        *(unsigned int*)(self + findEntry(self, id) * 0x54 + 0x10) = value;
        break;
    case 5:
        *(unsigned int*)(self + findEntry(self, id) * 0x54 + 0x14) = value;
        break;
    case 7:
        *(unsigned int*)(self + findEntry(self, id) * 0x54 + 0x18) = value;
        break;
    case 11: {
        char* p = self;
        int found = 0;
        for (unsigned int i = 0; i < 8; ++i, p += 0x30) {
            if (id == *(int*)(p + 0x330)) {
                if (arg == *(int*)(p + 0x338)) {
                    if (value == 0xFFFF) {
                        removeValue((EntryBlock*)p, value, arg);
                    } else {
                        addValue((EntryBlock*)p, value, arg);
                    }

                    unsigned int result = firstValue((EntryBlock*)p);
                    if (result != 0xFFFF) {
                        onKirbyResourceLoaded__9ftManagerFii(g_ftManager, *(int*)(self + 0x174), id);
                        goto mark_found;
                    }
                } else {
                mark_found:
                    found = 1;
                }
            }
        }
        if (!found) {
            onKirbyResourceUnLoaded__9ftManagerFii(g_ftManager, *(int*)(self + 0x174), id);
        }
        break;
    }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_12006C {
static inline u8 findEntry(char* self, int id) {
    for (u8 i = 0; i < *(u8*)(self + 0x158); ++i) {
        if (id == *(int*)(self + i * 0x54 + 8)) {
            return i;
        }
    }
    return 0xFF;
}

extern "C" int fn_27_12006C(char* self, int kind, int id, unsigned int field) {
    switch (kind) {
    case 8: {
        char* entry = self + findEntry(self, id) * 0x54 + 8;
        if (field == 0xC) return *(int*)(entry + 0x48);
        if (field == 0xD) return *(int*)(entry + 0x44);
        if (field & 0x80) return *(int*)(entry + 0x4C);
        return *(int*)(entry + (u8)field * 4 + 0x14);
    }
    case 6:
        return *(int*)(self + findEntry(self, id) * 0x54 + 0x10);
    case 5:
        return *(int*)(self + findEntry(self, id) * 0x54 + 0x14);
    case 7:
        return *(int*)(self + findEntry(self, id) * 0x54 + 0x18);
    default:
        return 0xFFFF;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_120218 {
extern "C" u32 fn_27_120218(u8* self, s32 arg1, s32 arg2) {
    s32 i;

    for (i = 0; i < 8; i++) {
        if (arg1 == *(s32*)(self + 0x330) && arg2 == *(s32*)(self + 0x338)) {
            u32 v;
            if (*(u32*)(self + 0x310) != 0xFFFF && (arg2 == -1 || arg2 == *(s32*)(self + 0x314))) {
                v = *(s32*)(self + 0x310);
            } else if (*(u32*)(self + 0x318) != 0xFFFF && (arg2 == -1 || arg2 == *(s32*)(self + 0x31C))) {
                v = *(s32*)(self + 0x318);
            } else if (*(u32*)(self + 0x320) != 0xFFFF && (arg2 == -1 || arg2 == *(s32*)(self + 0x324))) {
                v = *(s32*)(self + 0x320);
            } else if (*(u32*)(self + 0x328) != 0xFFFF && (arg2 == -1 || arg2 == *(s32*)(self + 0x32C))) {
                v = *(s32*)(self + 0x328);
            } else {
                v = 0xFFFF;
            }
            if (v != 0xFFFF) {
                return v;
            }
        }
        self += 0x30;
    }
    return 0xFFFF;
}
}

// TODO: clean up, give variables real names
namespace jj_120818 {
struct CheckObject { virtual void f0(); virtual void f1(); virtual void f2(); virtual s32 check(); };
extern "C" s32 isFinishMask__14ftDataProviderF6ftKindiiUlUlUl(s32, s32, s32, s32, s32, u8, u8);
extern "C" s32 isReady__14ftDataProviderFv(s32);
extern int g_ftDataProvider;
static inline bool checkSlots(s32 entry, s32 owner, char* obj, u8& k);
static inline bool checkOthers(s32 entry, s32 owner, s32* sm, s32& j, s32& off, char*& obj, u8& k);
extern "C" s32 isReadyRemove__6ftSlotFv(char* p) {
    if (*(s32*)(p + 4) != 0) return 0;
    if (((CheckObject*)(p + 0x178))->check() > 0 || ((CheckObject*)(p + 0x204))->check() > 0) return 0;
    if (*(s32*)(p + 0x160) != 6) {
        s32 entry; u8 k; s32 j; s32 owner; s32* sm; s32 off; char* obj; u8 i;
        for (i = 0; i < *(u8*)(p + 0x158); ++i) {
            owner = *(s32*)(p + 0x174);
            sm = (s32*)g_ftSlotManager;
            j = 0;
            off = 0;
            entry = *(s32*)(p + i * 0x54 + 8);
            if (!checkOthers(entry, owner, sm, j, off, obj, k)) return 0;
        }
    }
    if (isReady__14ftDataProviderFv(g_ftDataProvider) == 0) return 0;
    return 1;
}
static inline bool checkSlots(s32 entry, s32 owner, char* obj, u8& k) {
    for (k = 0; k < 8; ++k) {
        char* slot = obj + k * 0x30;
        s32 slotOwner = *(s32*)(slot + 0x338);
        if (slotOwner == -1) continue;
        if (entry != *(s32*)(slot + 0x330)) continue;
        if (owner != -1 && owner != slotOwner) continue;
        u8 flag = *(u8*)(obj + 0x15E);
        
        if (((u32)(-((((char*)g_ftManager)[0x6C] >> 6) & 1) | ((((char*)g_ftManager)[0x6C] >> 6) & 1)) >> 31) == 1 && ((((char*)g_ftManager)[0x6E] >> 2) & 1) == 0) flag = 0;
        if (isFinishMask__14ftDataProviderF6ftKindiiUlUlUl(g_ftDataProvider, *(s32*)(slot + 0x330), *(s32*)(obj + 0x174), slotOwner, *(s32*)(slot + 0x334), *(u8*)(obj + 0x15B), flag) == 0) return false;
    }
    return true;
}
static inline bool checkOthers(s32 entry, s32 owner, s32* sm, s32& j, s32& off, char*& obj, u8& k) {
    while (j < sm[1]) {
        if (j != owner) {
            obj = (char*)sm[0] + off;
            if (*(u8*)(obj + 0x15D) == 0 && *(s32*)(obj + 0x160) == 6) {
                if (!checkSlots(entry, owner, obj, k)) return 0;
            }
        }
        off += 0x4D4;
        ++j;
    }
    return true;
}
}

// TODO: clean up, give variables real names
namespace jj_120A10 {

struct CheckObject { virtual void f0(); virtual void f1(); virtual void f2(); virtual s32 check(); };

extern "C" s32 isFinishMask__14ftDataProviderF6ftKindiiUlUlUl(s32, s32, s32, s32, s32, u8, u8);
extern "C" s32 isReady__14ftDataProviderFv(s32);
extern "C" u32 isExistEffect__14ftDataProviderFUlUl(s32, void*, s32);
extern int g_ftDataProvider;

static inline bool checkSlots(s32 entry, s32 owner, char* obj, u8& k);
static inline bool checkOthers(s32 entry, s32 owner, s32* sm, s32& j, s32& off, char*& obj, u8& k);

static inline bool checkReady(char* p) {
    char* obj; s32 off; s32* sm; s32 owner; u8 i; s32 entry; u8 k; s32 j;
    if (*(s32*)(p + 4) != 0) return 0;
    if (((CheckObject*)(p + 0x178))->check() > 0 || ((CheckObject*)(p + 0x204))->check() > 0) return 0;
    if (*(s32*)(p + 0x160) != 6) {
        for (i = 0; i < *(u8*)(p + 0x158); ++i) {
            owner = *(s32*)(p + 0x174);
            sm = (s32*)g_ftSlotManager;
            j = 0;
            off = 0;
            entry = *(s32*)(p + i * 0x54 + 8);
            if (!checkOthers(entry, owner, sm, j, off, obj, k)) return 0;
        }
    }
    return isReady__14ftDataProviderFv(g_ftDataProvider) != 0;
}

extern "C" s32 isReady__6ftSlotFv(char* p) {
    if (!checkReady(p)) return 0;
    char* e;
    u8 i;
    {
        for (i = 0; i < *(u8*)(p + 0x158); ++i) {
            e = p + i * 0x54;
            if ((*(u32*)(e + 0xC) & 2) && isExistEffect__14ftDataProviderFUlUl(g_ftDataProvider, *(void**)((char*)g_ftSlotManager + *(s32*)(e + 8) * 4 + 0x78), 1) == 1) return 0;
            if ((*(u32*)(e + 0xC) & 0x10) && isExistEffect__14ftDataProviderFUlUl(g_ftDataProvider, *(void**)((char*)g_ftSlotManager + 0x53C), 1) == 1) return 0;
            if ((*(u32*)(e + 0xC) & 0x1000) && isExistEffect__14ftDataProviderFUlUl(g_ftDataProvider, *(void**)((char*)g_ftSlotManager + *(s32*)(e + 8) * 4 + 0x154), 1) == 1) return 0;
            if (!(*(u16*)(e + 0x58) & 0x10) && isExistEffect__14ftDataProviderFUlUl(g_ftDataProvider, *(void**)((char*)g_ftSlotManager + *(s32*)(e + 8) * 4 + 0x460), 1) == 1) return 0;
        }
        if (*(s32*)(p + 0x160) == 6) {
            char* slot;
            u8 k;
            for (k = 0; k < 8; ++k) {
                slot = p + k * 0x30;
                if (*(s32*)(slot + 0x338) != -1) {
                    if (isFinishMask__14ftDataProviderF6ftKindiiUlUlUl(g_ftDataProvider, *(s32*)(slot + 0x330), *(s32*)(p + 0x174), *(s32*)(slot + 0x338), *(s32*)(slot + 0x334), *(u8*)(p + 0x15B), *(u8*)(p + 0x15E)) == 0) return 0;
                }
                if (*(u32*)(slot + 0x334) & 0x400) {
                    if (isExistEffect__14ftDataProviderFUlUl(g_ftDataProvider, *(void**)((char*)g_ftSlotManager + *(s32*)(slot + 0x330) * 4 + 0x460), 1) == 1) return 0;
                }
            }
        }
    }
    return 1;
}

static inline bool checkSlots(s32 entry, s32 owner, char* obj, u8& k) {
    for (k = 0; k < 8; ++k) {
        char* slot = obj + k * 0x30;
        s32 slotOwner = *(s32*)(slot + 0x338);
        if (slotOwner == -1) continue;
        if (entry != *(s32*)(slot + 0x330)) continue;
        if (owner != -1 && owner != slotOwner) continue;
        u8 flag = *(u8*)(obj + 0x15E);

        if (((u32)(-((((char*)g_ftManager)[0x6C] >> 6) & 1) | ((((char*)g_ftManager)[0x6C] >> 6) & 1)) >> 31) == 1 && ((((char*)g_ftManager)[0x6E] >> 2) & 1) == 0) flag = 0;
        if (isFinishMask__14ftDataProviderF6ftKindiiUlUlUl(g_ftDataProvider, *(s32*)(slot + 0x330), *(s32*)(obj + 0x174), slotOwner, *(s32*)(slot + 0x334), *(u8*)(obj + 0x15B), flag) == 0) return false;
    }
    return true;
}

static inline bool checkOthers(s32 entry, s32 owner, s32* sm, s32& j, s32& off, char*& obj, u8& k) {
    while (j < sm[1]) {
        if (j != owner) {
            obj = (char*)sm[0] + off;
            if (*(u8*)(obj + 0x15D) == 0 && *(s32*)(obj + 0x160) == 6) {
                if (!checkSlots(entry, owner, obj, k)) return 0;
            }
        }
        off += 0x4D4;
        ++j;
    }
    return true;
}
}

// TODO: clean up, give variables real names
namespace jj_120DCC {
typedef unsigned char u8;
typedef unsigned int u32;

extern void* g_ftSlotManager;
extern u32 g_ftDataProvider;
extern "C" int isExistEffect__14ftDataProviderFUlUl(u32, void*, int);

extern "C" int fn_27_120DCC(char* self) {
    char* e;
    u8 count = *(u8*)(self + 0x158);
    u8 i = 0;
    for (; i < count; i++) {
        e = self + i * 0x54;
        if (*(u32*)(e + 0xC) & 0x1000) {
            u32 idx = *(u32*)(e + 0x8);
            void* slot = *(void**)((char*)g_ftSlotManager + 0x154 + idx * 4);
            return isExistEffect__14ftDataProviderFUlUl(g_ftDataProvider, slot, 0);
        }
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_12131C {
extern "C" int fn_27_12131C(char* self, int type, unsigned int sub, int sub2, unsigned int mask, int pref, int start) {
    char* base;
    int offset;
    char* e;
    int i;
    int count;
    int inRange;

    if (pref != -1) {
        inRange = 0;
        if (type >= 0x2E && type <= 0x31) inRange = 1;
        if (inRange == 0) {
            char* p = *(char**)self + pref * 0x4D4;
            if ((unsigned char)p[0x159] == 0 && type == *(int*)(p + 0x160) && sub == (unsigned char)p[0x15B]) {
                if (mask == 0 || mask == (mask & *(unsigned int*)(p + sub2 * 0x54 + 0xC))) return pref;
            }
        }
    }

    count = *(int*)(self + 4);
    i = start;
    for (int j = 0; j < count; j++) {
        offset = i * 0x4D4;
        base = *(char**)self;
        e = base + offset;
        if ((unsigned char)e[0x159] == 0 && type == *(int*)(e + 0x160) && sub == (unsigned char)e[0x15B]) {
            if (mask == 0 || mask == (mask & *(unsigned int*)(offset + (base + sub2 * 0x54) + 0xC))) return i;
        }
        i++;
        if (i >= count) i = 0;
    }

    inRange = 0;
    if (type >= 0x2E && type <= 0x31) inRange = 1;
    i = start;
    if (inRange != 0) {
        for (int j = 0; j < count; j++) {
            base = *(char**)self;
            e = base + i * 0x4D4;
            if ((unsigned char)e[0x15D] == 0 && type == *(int*)(e + 0x160) && sub == (unsigned char)e[0x15B]) return i;
            i++;
            if (i >= count) i = 0;
        }
    } else {
        for (int j = 0; j < count; j++) {
            base = *(char**)self;
            e = base + i * 0x4D4;
            if ((unsigned char)e[0x159] == 0 && type == *(int*)(e + 0x160) && sub == (unsigned char)e[0x15B]) return i;
            i++;
            if (i >= count) i = 0;
        }
    }
    return -1;
}
}

// TODO: clean up, give variables real names
namespace jj_1220DC {
extern "C" void fn_27_11FD4C(char*, int, int, unsigned int, unsigned int, int);
extern "C" void fn_27_1220DC(char* self, int index, int arg, unsigned int kind, int id, unsigned int field, unsigned int value) {
    switch (kind) {
    case 15: *(unsigned int*)(self + 0x74) = value; return;
    case 1: *(unsigned int*)(self + id * 4 + 0x78) = value; return;
    case 4: *(unsigned int*)(self + 0x53c) = value; return;
    case 13:
        if (id == 12) {
            if (field == 12) *(unsigned int*)(self + 0x340) = value;
            else if (field == 13) *(unsigned int*)(self + 0x33c) = value;
            else if (field & 0x80) *(unsigned int*)(self + 0x344) = value;
            else *(unsigned int*)(self + (unsigned char)field * 4 + 0x30c) = value;
        } else *(unsigned int*)(self + id * 4 + 0x230) = value;
        return;
    case 14:
        if (id == 12) {
            if (field == 12) *(unsigned int*)(self + 0x458) = value;
            else if (field == 13) *(unsigned int*)(self + 0x454) = value;
            else if (field & 0x80) *(unsigned int*)(self + 0x45c) = value;
            else *(unsigned int*)(self + (unsigned char)field * 4 + 0x424) = value;
        } else *(unsigned int*)(self + id * 4 + 0x348) = value;
        return;
    case 12: *(unsigned int*)(self + id * 4 + 0x154) = value; return;
    case 10: *(unsigned int*)(self + id * 4 + 0x460) = value; return;
    default: fn_27_11FD4C(*(char**)self + index * 0x4d4, kind, id, field, value, arg); return;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_122218 {
extern "C" u32 fn_27_122218(u8* self, s32 a, u32 b) {
    if (a == 0xc) {
        if (b == 0xc) {
            return *(u32*)(self + 0x340);
        }
        if (b == 0xd) {
            return *(u32*)(self + 0x33c);
        }
        if (b & 0x80) {
            return *(u32*)(self + 0x344);
        }
        return *(u32*)(self + ((b & 0xff) << 2) + 0x30c);
    }
    return *(u32*)(self + (a << 2) + 0x230);
}
}

// TODO: clean up, give variables real names
namespace jj_122270 {
typedef unsigned int u32;
typedef int s32;

extern "C" {
s32 fn_27_122270(char* self, s32 a, u32 b) {
    if (a == 0xc) {
        if (b == 0xc) {
            return *(s32*)(self + 0x458);
        }
        if (b == 0xd) {
            return *(s32*)(self + 0x454);
        }
        if (b & 0x80) {
            return *(s32*)(self + 0x45c);
        }
        return *(s32*)(self + ((b & 0xff) << 2) + 0x424);
    }
    return *(s32*)(self + (a << 2) + 0x348);
}
}
}

// TODO: clean up, give variables real names
namespace jj_1222C8 {
struct ftSlotManager {
    u32 pad[0x460 / 4];
    void* slots[1];
};

extern "C" void* fn_27_1222C8(ftSlotManager* self, u32 index) {
    return self->slots[index];
}
}

