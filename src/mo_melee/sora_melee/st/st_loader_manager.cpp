#include <gf/gf_task_scheduler.h>
#include <st/loader/st_loader_manager.h>
#include <st/loader/st_loader_sand_bag.h>
#include <st/loader/st_loader_stage.h>
#include <st/loader/st_loader_stage_star_fox.h>
#include <st/loader/st_loader_stage_se.h>
#include <st/loader/st_loader_info.h>
#include <st/loader/st_loader_stage_adventure.h>
#include <st/loader/st_loader_info_wifi.h>
#include <st/loader/st_loader_info_adventure.h>
#include <st/loader/st_loader_local_adventure.h>
#include <st/loader/st_loader_info_stage_edit.h>
#include <types.h>
#include <memory.h>

extern const void* lbl_27_data_4CCC0[2];

// HYPOTHESIS: the loader class whose constructor sits at 0x23AC74 (a stLoaderStage subclass of size 0xE4).
extern "C" stLoader* fn_27_23AC74(void* self);

// TODO: clean up - the kind numbers are bare; name the loader kinds (players 0..6, then stage, star fox, SE, info, ...).
stLoader* stLoaderManager::createLoader(u32 kind) {
    switch (kind) {
    case 0:
        return new (Heaps::System) stLoaderPlayer(0);
    case 1:
        return new (Heaps::System) stLoaderPlayer(1);
    case 2:
        return new (Heaps::System) stLoaderPlayer(2);
    case 3:
        return new (Heaps::System) stLoaderPlayer(3);
    case 4:
        return new (Heaps::System) stLoaderPlayer(4);
    case 5:
        return new (Heaps::System) stLoaderPlayer(5);
    case 6:
        return new (Heaps::System) stLoaderPlayer(6);
    case 7:
        return new (Heaps::System) stLoaderStage;
    case 8:
        return new (Heaps::System) stLoaderStageStarFox;
    case 9:
        return new (Heaps::System) stLoaderStageSe;
    case 10:
        return new (Heaps::System) stLoaderPokemonSe;
    case 11:
        return new (Heaps::System) stLoaderInfo;
    case 12:
        return new (Heaps::System) stLoaderSandBag(2);
    case 13: {
        void* mem = operator new(0xE4, Heaps::System);
        return mem != NULL ? fn_27_23AC74(mem) : NULL;
    }
    case 14:
        return new (Heaps::System) stLoaderStageAdventure;
    case 15:
        return new (Heaps::System) stLoaderCommonSeAdventure;
    case 16:
        return new (Heaps::System) stLoaderStageSeAdventure;
    case 17:
        return new (Heaps::System) stLoaderInfoWifi;
    case 18:
        return new (Heaps::System) stLoaderInfoAdventure;
    case 19:
        return new (Heaps::System) stLoaderLocalAdventure;
    case 20:
        return new (Heaps::System) stLoaderInfoStageEdit;
    default:
        return NULL;
    }
}

inline stLoaderManager::stLoaderManager()
    : unk38(0), unk3C(0), m_isRequested(false), m_isSetupDone(false),
      unk4C(lbl_27_data_4CCC0[0]), unk50(0) {
    unk40 = new (Heaps::System) u32(0);
    m_request = new (Heaps::System) u32(0);
    for (int player = 0; player < MAX_PLAYERS; ++player) {
        m_loaderPlayers[player] = NULL;
    }
    m_loaderStage = NULL;
    m_loaderStageAlt = NULL;
    m_loaderStageSe = NULL;
    m_loaderPokemonSe = NULL;
    m_loaderInfo = NULL;
    m_loaderSandbag = NULL;
    m_loaderStageAdvCommon = NULL;
}

void stLoaderManager::create() {
    if (g_stLoaderManager == NULL) {
        g_stLoaderManager = new (Heaps::System) stLoaderManager;
    }
}

// HYPOTHESIS: stLoaderManager::sqSetupSyncDo (map); it stays a stand-in until its sequence data is recovered.
extern "C" bool fn_27_23A580(stLoaderManager* manager);

static stLoaderSandBagMarker sMarkerA(0xFF, 0);
static stLoaderSandBagMarker sMarkerB(0xFF, 1);

void stLoaderManager::sqSetupRequest(const u32* request) {
    *m_request = *request;
    m_isSetupDone = false;
    m_isRequested = true;
}

bool stLoaderManager::sqSetupSync(bool wait) {
    bool isDone;
    while (!(isDone = fn_27_23A580(this)) && wait) {
        gfTaskScheduler::getInstance()->updateStatus();
    }
    return isDone;
}

// TODO: clean up, give variables real names
namespace jj_23A92C {
struct Pair { int a; int b; };

extern "C" {
void* __nw__FUlQ25Heaps8HeapType(unsigned long size, int heap);
void* __ct__9clManagerFv(void* p);
void init__9clManagerFPi(void* a, void* b);
void fn_27_3AD0(int a, int b, int c, int d);
void create__13gfAreaManagerFQ25Heaps8HeapType(int a);
void __ct__18grCollisionManagerFv(void* p);
void create__6IfMngrFv();
void create__12IfWifiprMngrFv();
void fn_8010F310();
void create__9IfAdvMngrFii(int a, int b);
void fn_27_2A1EC0();
void create__9ftManagerFv();
extern void* lbl_27_bss_8;
extern Pair lbl_27_rodata_3970;
}

extern "C" void fn_27_23A92C(char* self, u32 flags) {
    if (flags & 0x20) {
        Pair data = lbl_27_rodata_3970;
        if (lbl_27_bss_8 == 0) {
            void* p = __nw__FUlQ25Heaps8HeapType(0x28, 2);
            if (p != 0) {
                p = __ct__9clManagerFv(p);
            }
            lbl_27_bss_8 = p;
        }
        init__9clManagerFPi(lbl_27_bss_8, &data);
        fn_27_3AD0(200, 10, 1, 0);
        create__13gfAreaManagerFQ25Heaps8HeapType(1);
        void* q = __nw__FUlQ25Heaps8HeapType(0x4F8, 2);
        if (q != 0) {
            __ct__18grCollisionManagerFv(q);
        }
        *(u32*)(self + 0x38) |= 0x20;
    }
    if (flags & 1) {
        create__6IfMngrFv();
        *(u32*)(self + 0x38) |= 1;
    }
    if (flags & 2) {
        create__12IfWifiprMngrFv();
        *(u32*)(self + 0x38) |= 2;
    }
    if (flags & 0x40) {
        fn_8010F310();
        *(u32*)(self + 0x38) |= 0x40;
    }
    if (flags & 4) {
        create__9IfAdvMngrFii(1, 1);
        *(u32*)(self + 0x38) |= 4;
    }
    if (flags & 8) {
        fn_27_2A1EC0();
        *(u32*)(self + 0x38) |= 8;
    }
    if (flags & 0x10) {
        create__9ftManagerFv();
        *(u32*)(self + 0x38) |= 0x10;
    }
}
}

