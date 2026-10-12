#include <ft/ft_status_uniq_process_damage.h>
#include <types.h>

// Pit (fall into a pit) status process; derives from the shared damage process.
class ftStatusUniqProcessPitFall : public ftStatusUniqProcessDamage {
public:
    virtual ~ftStatusUniqProcessPitFall() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};

ftStatusUniqProcessPitFall g_ftStatusUniqProcessPitFall;

void ftStatusUniqProcessPitFall::initStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessPitFall::execStop(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessPitFall::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    // HYPOTHESIS: the pit effect handle is kept in work int 0x20000001 and killed on exit.
    int effectId = moduleAccesser->getWorkManageModule().getInt(0x20000001);
    moduleAccesser->getEffectModule().kill(effectId, true, true);
    ftStatusUniqProcessDamage::exitStatus(moduleAccesser, nextStatusKind);
}
