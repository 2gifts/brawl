#include <gf/gf_task_scheduler.h>
#include <st/loader/st_loader_manager.h>
#include <st/loader/st_loader_sand_bag.h>
#include <types.h>

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
