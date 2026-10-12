#include <it/it_gen_archive.h>
#include <gf/gf_task_scheduler.h>
#include <so/stageobject.h>
#include <so/team/so_team.h>
#include <wn/dedede/wn_dedede_follower.h>
#include <math.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

float ftDededeWaddledeeThrowABS(float value);

// HYPOTHESIS: these local event views extend the public event header with the
// two float payload words carried by Dedede's Waddle Dee throw link event.
struct ftDededeWaddledeeThrowEventArgs : public soLinkEventArgs {
    float unk8;
    float unkC;

    ftDededeWaddledeeThrowEventArgs()
        : soLinkEventArgs(0x83A), unk8(0.0f), unkC(0.0f) { }
};

// HYPOTHESIS: the stop event has no payload beyond the common event header.
struct ftDededeWaddledeeThrowStopEventArgs : public soLinkEventArgs {
    ftDededeWaddledeeThrowStopEventArgs() : soLinkEventArgs(0x83B) { }
};

// Native per-unit item sheets are initialized even in the article path.
static itGenSheetKind g_unkItemSheet0(Stages::srStageKind(0xff), 0);
static itGenSheetKind g_unkItemSheet1(Stages::srStageKind(0xff), 1);

class ftDededeStatusUniqProcessSpecialWaddledeeThrowPickup : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialWaddledeeThrowPickup() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDededeStatusUniqProcessSpecialWaddledeeThrowPickup::initStatus(soModuleAccesser* moduleAccesser) {
    int kind = moduleAccesser->getWorkManageModule().getInt(0x10000043);
    unsigned long taskId = moduleAccesser->getWorkManageModule().getInt(0x10000044);
    gfTask* task = gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Weapon, taskId);
    if (task != NULL && kind == 0x33) {
        // MATCH-ONLY: preserve the owner argument's lifetime before the receiver.
        int ownerTaskId;
        wnDededeFollower* follower = dynamic_cast<wnDededeFollower*>(task);
        ownerTaskId = moduleAccesser->m_stageObject->m_taskId;
        int teamNo = moduleAccesser->getTeamModule().getTeam()->getNo();
        follower->had(ownerTaskId, 54, teamNo);
        moduleAccesser->getWorkManageModule().onFlag(0x22000017);
    }
}

void ftDededeStatusUniqProcessSpecialWaddledeeThrowPickup::execStatus(soModuleAccesser* moduleAccesser) {
    float angle = soValueAccesser::getConstantFloat(moduleAccesser, 0xFAC, 0);
    float stickY = moduleAccesser->getControllerModule().getStickY();

    float delta;
    if (stickY < 0.0f) {
        if (soValueAccesser::getConstantFloat(moduleAccesser, 0xFAD, 0) < 0.0f) {
            delta = angle + ftDededeWaddledeeThrowABS(
                soValueAccesser::getConstantFloat(moduleAccesser, 0xFAD, 0));
        } else {
            delta = angle - soValueAccesser::getConstantFloat(moduleAccesser, 0xFAD, 0);
        }
        angle = angle - delta * ftDededeWaddledeeThrowABS(stickY);
    } else if (stickY > 0.0f) {
        delta = soValueAccesser::getConstantFloat(moduleAccesser, 0xFAE, 0) - angle;
        angle = angle + delta * ftDededeWaddledeeThrowABS(stickY);
    }

    if (angle < 0.0f) {
        angle += 360.0f;
    }

    moduleAccesser->getWorkManageModule().setFloat(angle, 0x21000004);
}

void ftDededeStatusUniqProcessSpecialWaddledeeThrowPickup::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
        if (moduleAccesser->getLinkModule().isLinked(3) == true) {
            ftDededeWaddledeeThrowEventArgs event;
            event.unk8 = soValueAccesser::getConstantFloat(moduleAccesser, 0xFAA, 0);
            event.unkC = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
            moduleAccesser->getLinkModule().sendEventNodes(3, event, 0);
        }
        moduleAccesser->getWorkManageModule().offFlag(0x22000013);
        moduleAccesser->getWorkManageModule().offFlag(0x22000017);
    }
}

void ftDededeStatusUniqProcessSpecialWaddledeeThrowPickup::execStop(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialWaddledeeThrowPickup::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000017)) {
        if (moduleAccesser->getLinkModule().isLinked(3) == true) {
            ftDededeWaddledeeThrowStopEventArgs event;
            moduleAccesser->getLinkModule().sendEventNodes(3, event, 0);
        }
        moduleAccesser->getWorkManageModule().offFlag(0x22000017);
    }
}

ftDededeStatusUniqProcessSpecialWaddledeeThrowPickup g_ftDededeStatusUniqProcessSpecialWaddledeeThrowPickup;