#include <gf/gf_task_scheduler.h>
#include <st/loader/st_loader_manager.h>
#include <st/loader/st_loader_sand_bag.h>
#include <types.h>
#include <memory.h>

extern const void* lbl_27_data_4CCC0[2];

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
