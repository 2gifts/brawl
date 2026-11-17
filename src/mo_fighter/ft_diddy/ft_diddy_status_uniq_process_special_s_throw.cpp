#define SO_CATCH_MODULE_CLING_SLOTS
#include <ft/fighter.h>
#include <so/catch/so_catch_module.h>
#include <gf/gf_task_scheduler.h>
#include <so/link/so_link_event_throw.h>
#include <so/link/so_link_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <ft/diddy/ft_diddy_status_uniq_process_special_s_cling.h>
#include <types.h>

// The throw derives from the cling status, as its RTTI base list and its vtable's checkDamage slot show.
class ftDiddyStatusUniqProcessSpecialSThrow : public ftDiddyStatusUniqProcessSpecialSCling {
public:
    virtual ~ftDiddyStatusUniqProcessSpecialSThrow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStop(soModuleAccesser* moduleAccesser);
};

// Side special throw: in statuses 0x11f/0x121 the grabbed fighter (link node 0) is thrown with event 0x22 and a throw
// motion picked by the throw direction and the victim's kind (kinds 5, 0x12 and 0x25 get their own pair); its task id
// goes to work int 0x20000001 (and 0x20000000). In 0x120/0x122 the stored victim id moves from 0x20000000 to 0x20000001.
void ftDiddyStatusUniqProcessSpecialSThrow::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x11f:
    case 0x121: {
        soLinkEventThrow event(0x22, -1, true);
        int taskId = moduleAccesser->getLinkModule().getNodeTaskId(0);
        Fighter* victim = dynamic_cast<Fighter*>(
            gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Fighter, taskId));
        if (victim->getFtKind() == 5 || victim->getFtKind() == 0x12 || victim->getFtKind() == 0x25) {
            if (moduleAccesser->getStatusModule().getStatusKind() == 0x11f) {
                event.unk8 = 0x1a2;
            } else {
                event.unk8 = 0x1a3;
            }
        } else {
            if (moduleAccesser->getStatusModule().getStatusKind() == 0x11f) {
                event.unk8 = 0x19f;
            } else {
                event.unk8 = 0x1a0;
            }
        }
        moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
        moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getLinkModule().getNodeTaskId(0), 0x20000001);
        moduleAccesser->getWorkManageModule().setInt(0, 0x20000002);
        moduleAccesser->getWorkManageModule().setInt(0, 0x20000003);
        moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getLinkModule().getNodeTaskId(0), 0x20000000);
        break;
    }
    case 0x120:
    case 0x122: {
        int victimTaskId = moduleAccesser->getWorkManageModule().getInt(0x20000000);
        moduleAccesser->getWorkManageModule().setInt(victimTaskId, 0x20000001);
        moduleAccesser->getWorkManageModule().setInt(0, 0x20000002);
        moduleAccesser->getWorkManageModule().setInt(0, 0x20000003);
        break;
    }
    }
}
// BYTES: defined here, between its first caller and the rest of the status, where the original places it.
soLinkEventThrow::soLinkEventThrow(int eventKind, int value, bool enabled)
    : soLinkEventArgs(eventKind), unk8(value), unkC(-1), unk10(-1), unk14(-1),
      unk18(1.0f), unk1C(enabled) {}

void ftDiddyStatusUniqProcessSpecialSThrow::execStatus(soModuleAccesser* moduleAccesser) {}
void ftDiddyStatusUniqProcessSpecialSThrow::execFixPos(soModuleAccesser* moduleAccesser) {}
// Leaving the throw into 0x120/0x122 throws the victim (event 0x23); otherwise a victim still linked to node 0 is
// released from the cling (catch module clingCut).
void ftDiddyStatusUniqProcessSpecialSThrow::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus == 0x120 || nextStatus == 0x122) {
        soLinkEventThrow event(0x23, -1, true);
        moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
    } else if (moduleAccesser->getLinkModule().isLinked(0) == true) {
        static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->clingCut(false);
    }
}
void ftDiddyStatusUniqProcessSpecialSThrow::execStop(soModuleAccesser* moduleAccesser) {}

ftDiddyStatusUniqProcessSpecialSThrow g_ftDiddyStatusUniqProcessSpecialSThrow;
