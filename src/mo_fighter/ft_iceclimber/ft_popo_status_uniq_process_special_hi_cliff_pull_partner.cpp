#include <ft/fighter.h>
#include <ft/ft_status_uniq_process_cliff.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Link event sent to the cliff-pulled partner: the kind, then the status kind it is moved to (HYPOTHESIS names).
struct ftPopoCliffPullStatusEvent : soLinkEventArgs {
    s32 m_status;
    ftPopoCliffPullStatusEvent(int kind, int status) : soLinkEventArgs(kind), m_status(status) { }
};

// Link event carrying the ledge position the partner is pulled to (HYPOTHESIS names).
struct ftPopoCliffPullPosEvent : soLinkEventArgs {
    Vec3f m_pos;
    ftPopoCliffPullPosEvent(int kind, Vec3f pos) : soLinkEventArgs(kind), m_pos(pos) { }
};

class ftPopoStatusUniqProcessSpecialHiCliffPullPartner : public soStatusUniqProcess {
public:
    virtual ~ftPopoStatusUniqProcessSpecialHiCliffPullPartner() { }
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftPopoStatusUniqProcessSpecialHiCliffPullPartner::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010) == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000010);
        Fighter* partner = dynamic_cast<Fighter&>(moduleAccesser->getStageObject()).getPartner();
        if (partner != 0 && soExternalValueAccesser::getStatusKind(partner) == 0x119) {
            if (moduleAccesser->getLinkModule().isLink(6) == 1) {
                ftPopoCliffPullStatusEvent statusEvent(0x455, 0x11f);
                moduleAccesser->getLinkModule().sendEventNodes(6, statusEvent, 0);
                Vec3f pos = moduleAccesser->getGroundModule().getCliffPos(0);
                ftPopoCliffPullPosEvent posEvent(0x459, pos);
                moduleAccesser->getLinkModule().sendEventNodes(6, posEvent, 0);
            }
        }
    }
}

void ftPopoStatusUniqProcessSpecialHiCliffPullPartner::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (ftStatusUniqProcessCliff::isLeaveCliffStatus(nextStatus) == true) {
        moduleAccesser->getGroundModule().leaveCliff(0);
    }
}

ftPopoStatusUniqProcessSpecialHiCliffPullPartner g_ftPopoStatusUniqProcessSpecialHiCliffPullPartner;
