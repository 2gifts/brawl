#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/effect/so_effect_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSamusCShotStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnSamusCShotStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnSamusCShotStatusUniqProcess g_wnSamusCShotStatusUniqProcess;

void wnSamusCShotStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) { }

// Kills the effect whose id is held in work int 0x10000006, unless the next status is in 1..8.
void wnSamusCShotStatusUniqProcess::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus >= 9 || nextStatus < 1) {
        int effectId = moduleAccesser->getWorkManageModule().getInt(0x10000006);
        moduleAccesser->getEffectModule().kill(effectId, true, true);
    }
}

void wnSamusCShotStatusUniqProcess::execFixPos(soModuleAccesser* moduleAccesser) { }
