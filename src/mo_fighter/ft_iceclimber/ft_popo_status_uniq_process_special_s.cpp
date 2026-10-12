#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_value_accesser.h>
#include <so/controller/so_controller_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <types.h>

// Link event carrying only its kind (HYPOTHESIS: kind 0x458 is the popo's release event).
struct ftPopoSpecialSLinkEvent : soLinkEventArgs {
    ftPopoSpecialSLinkEvent(int kind) : soLinkEventArgs(kind) { }
};

class ftPopoStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftPopoStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftPopoStatusUniqProcessSpecialS::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftPopoStatusUniqProcessSpecialS::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
        return;
    }
    if (moduleAccesser->getSituationModule().getKind() != 0) {
        return;
    }
    int threshold = soValueAccesser::getConstantInt(moduleAccesser, 0x5dc0, 0);
    if (moduleAccesser->getWorkManageModule().getInt(0x20000002) < threshold) {
        return;
    }
    u32 mask = soController::getButtonMask(soController::PadButton(1));
    soControllerModule& controller = moduleAccesser->getControllerModule();
    if ((controller.getTrigger() & mask) == 0) {
        return;
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010) == true) {
        moduleAccesser->getKineticModule().changeKinetic(0x68, moduleAccesser);
    } else {
        moduleAccesser->getKineticModule().changeKinetic(0x67, moduleAccesser);
    }
    moduleAccesser->getSituationModule().setKind(SituationKind(2), false);
    moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::CorrectKind(5), 0);
}

void ftPopoStatusUniqProcessSpecialS::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010) == true) {
        if (moduleAccesser->getLinkModule().isLink(6) == true) {
            if (nextStatus != 0 && nextStatus != 0x10) {
                ftPopoSpecialSLinkEvent event(0x458);
                moduleAccesser->getLinkModule().sendEventNodes(6, event, 0);
            }
            moduleAccesser->getLinkModule().setAttribute(6, soLinkConnection::Attribute(0), false);
        }
    }
}

ftPopoStatusUniqProcessSpecialS g_ftPopoStatusUniqProcessSpecialS;
