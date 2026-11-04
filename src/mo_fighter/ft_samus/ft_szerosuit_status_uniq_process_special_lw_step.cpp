#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <gf/gf_task_scheduler.h>
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_external_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/stageobject.h>

class ftSZerosuitStatusUniqProcessSpecialLwStep : public soStatusUniqProcess {
public:
    virtual ~ftSZerosuitStatusUniqProcessSpecialLwStep() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

// HYPOTHESIS: link event 6 asks the stepped-on object for the constraint node and the position to stand at.
struct ftSZerosuitSpecialLwStepLinkEvent : soLinkEventArgs {
    bool m_constrain;
    int m_node;
    float m_x;
    float m_y;
    float m_scale;
    ftSZerosuitSpecialLwStepLinkEvent() : soLinkEventArgs(6), m_scale(1.0f) {
        m_constrain = false;
        m_node = 0;
    }
};

void ftSZerosuitStatusUniqProcessSpecialLwStep::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soLinkModule& link = moduleAccesser->getLinkModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    int taskId = workManage.getInt(0x20000003);
    if (taskId != -1) {
        gfTask* task = gfTaskScheduler::getInstance()->getTask((u32)taskId);
        if (task != NULL) {
            if (soExternalValueAccesser::isTreadPassive(&dynamic_cast<StageObject&>(*task))) {
                if (link.link(3, taskId)) {
                    Vec3f pos = posture.getPos();
                    ftSZerosuitSpecialLwStepLinkEvent event;
                    event.m_x = pos.m_x;
                    event.m_y = pos.m_y;
                    link.sendEventParents(3, event);
                    if (event.m_constrain == true) {
                        link.setModelConstraintPosOrt(3, 0, event.m_node, 5, true);
                    } else {
                        workManage.onFlag(0x22000010);
                        pos.m_x = event.m_x;
                        pos.m_y = event.m_y;
                        posture.setPos(&pos);
                    }
                }
            }
        }
    }
}

void ftSZerosuitStatusUniqProcessSpecialLwStep::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    soLinkModule& link = moduleAccesser->getLinkModule();
    if (link.isLink(3) == 1) {
        if (link.isModelConstraint() == 1) {
            link.adjustModelConstraintPosture(&Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
            link.removeModelConstraint(true);
        }
        link.unlink(3);
    }
    if (nextStatus == 0x115 || static_cast<u32>(nextStatus - 0x11b) <= 4u) {
        return;
    }
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    if (workManage.isFlag(0x22000017)) {
        posture.reverseLr();
        posture.updateRotYLr();
    }
}

ftSZerosuitStatusUniqProcessSpecialLwStep g_ftSZerosuitStatusUniqProcessSpecialLwStep;
