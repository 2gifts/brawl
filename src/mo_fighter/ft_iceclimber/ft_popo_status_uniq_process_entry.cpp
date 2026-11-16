#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Base entry process of the fighters' entry statuses (sora_melee, no header in this tree).
class ftStatusUniqProcessEntry : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessEntry() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execFixPos(soModuleAccesser* moduleAccesser); // the popo's vtable points at this base override
};

// Link event sent when the entry leaves to the next status (HYPOTHESIS names).
struct ftPopoEntryLinkEvent : soLinkEventArgs {
    s32 m_nextStatus;
    ftPopoEntryLinkEvent(int kind, int nextStatus) : soLinkEventArgs(kind), m_nextStatus(nextStatus) { }
};

class ftPopoStatusUniqProcessEntry : public ftStatusUniqProcessEntry {
public:
    virtual ~ftPopoStatusUniqProcessEntry() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

// HYPOTHESIS: work flag 0x1200003e marks the popo's entry mode (the cliff-pull partner path when set).
void ftPopoStatusUniqProcessEntry::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x1200003e) == false) {
        ftStatusUniqProcessEntry::initStatus(moduleAccesser);
    } else {
        if (moduleAccesser->getLinkModule().isLink(6) == true) {
            moduleAccesser->getGroundModule().setShapeKind(soGroundShapeImpl::ShapeKind(1), 0);
            moduleAccesser->getGroundModule().modifyRhombus(2.0f, 0.0f, 2.0f, 0);
            moduleAccesser->getLinkModule().setModelConstraintPosOrt(6, 0, 0, 3, true);
        }
    }
}

void ftPopoStatusUniqProcessEntry::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (!moduleAccesser->getWorkManageModule().isFlag(0x1200003e)) {
        ftStatusUniqProcessEntry::exitStatus(moduleAccesser, nextStatus);
        if (moduleAccesser->getLinkModule().isLink(6) == true) {
            ftPopoEntryLinkEvent event(0x455, nextStatus);
            moduleAccesser->getLinkModule().sendEventNodes(6, event, 0);
        }
    } else {
        if (moduleAccesser->getLinkModule().isModelConstraint() == true) {
            moduleAccesser->getLinkModule().removeModelConstraint(true);
            moduleAccesser->getLinkModule().setAttribute(6, soLinkConnection::Attribute(0xd), true);
        }
    }
}

ftPopoStatusUniqProcessEntry g_ftPopoStatusUniqProcessEntry;
