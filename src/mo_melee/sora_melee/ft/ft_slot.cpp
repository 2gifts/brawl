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
