#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/fighter.h>
#include <so/catch/so_catch_module.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: ft_status_uniq_process_catch_pull.h declares only initStatus and checkDamage, so the
// full override set (including exitStatus) is declared here for this translation unit.
class ftStatusUniqProcessCatchPull : public soStatusUniqProcess {
public:
    virtual void initStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
    // Original CatchPull vtable imports this override, rather than the generic false callback.
    virtual bool checkDamage(soModuleAccesser*, void*);
};

// HYPOTHESIS: catch module slots after catchCut (0x2c) are not in so_catch_module.h; the checkDamage slot is 0x3c.
class ftCatchModuleCatchPullView : public soCatchModule {
public:
    virtual void unk30();
    virtual void unk34();
    virtual void unk38();
    virtual bool checkDamageCatch(soModuleAccesser* moduleAccesser, void* arg);
};

void ftStatusUniqProcessCatchPull::initStatus(soModuleAccesser* moduleAccesser) {
    static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->setCatch(-1);
}

void ftStatusUniqProcessCatchPull::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != Fighter::Status::Catch_Wait) {
        static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->catchCut(false, false);
    }
    if (!moduleAccesser->getWorkManageModule().isFlag(0x22000010)) {
        // HYPOTHESIS: clears the SRT of the node that the catch uses (node id 0x130 in the model module).
        soModelModule& model = moduleAccesser->getModelModule();
        model.clearNodeSRT(model.getCorrectNodeId(0x130));
    }
}

bool ftStatusUniqProcessCatchPull::checkDamage(soModuleAccesser* moduleAccesser, void* arg) {
    return static_cast<ftCatchModuleCatchPullView*>(moduleAccesser->m_enumerationStart->m_catchModule)->checkDamageCatch(moduleAccesser, arg);
}

ftStatusUniqProcessCatchPull g_ftStatusUniqProcessCatchPull;
