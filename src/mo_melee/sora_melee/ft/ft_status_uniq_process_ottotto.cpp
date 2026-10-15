#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <gr/collision/gr_collision_status.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/stageobject.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// grCollStatus::getOttottoRecoveryPos (main DOL, fn_801348D8): writes the position the fighter is put back at when it
// starts teetering on a ledge. The BrawlHeaders class cannot be extended, so it is called through its symbol.
extern "C" void fn_801348D8(grCollStatus* collStatus, Vec3f* out, float rate);

// Teetering on the edge of a platform (statuses 0x7C start and 0x7D wait).
class ftStatusUniqProcessOttotto : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessOttotto() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};

ftStatusUniqProcessOttotto g_ftStatusUniqProcessOttotto;

// HYPOTHESIS: when the start status is entered where the cliff stop does not apply, the fighter is moved to the ground
// shape's recovery position.
void ftStatusUniqProcessOttotto::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x7C) {
        if (!moduleAccesser->getGroundModule().getConditionChecker()->isConditionNoCliffStop(moduleAccesser)) {
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            Vec3f recovery;
            fn_801348D8(moduleAccesser->getGroundModule().getCollStatus(0), &recovery, 1.6f);
            pos.m_x = recovery.m_x;
            pos.m_y = recovery.m_y;
            moduleAccesser->getPostureModule().setPos(&pos);
            moduleAccesser->m_stageObject->updateNodeSRT();
        }
    }
}

void ftStatusUniqProcessOttotto::execStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: work int 0x20000000 counts the frames (or hits) of the ottotto wobble.
    int count = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    moduleAccesser->getWorkManageModule().setInt(count + 1, 0x20000000);
}

void ftStatusUniqProcessOttotto::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x7D) {
        if (moduleAccesser->getSituationModule().getKind() == 0) {
            if (!moduleAccesser->getGroundModule().isOttotto(1.61f, 0)) {
                if (!moduleAccesser->getGroundModule().isOttotto(3.5f, 0) ||
                    moduleAccesser->getWorkManageModule().getInt(0x20000000) > 0x78) {
                    moduleAccesser->getStatusModule().changeStatusRequest(0, moduleAccesser);
                }
            }
        }
    }
}

void ftStatusUniqProcessOttotto::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) { }
