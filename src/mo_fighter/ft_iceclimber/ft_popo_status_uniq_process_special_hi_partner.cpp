#include <ft/fighter.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/link/so_link_module_impl.h>
#include <so/motion/so_motion_change_param.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Helpers of the iceclimber REL that are not reconstructed yet.
extern "C" void fn_105_BD50(soMotionChangeParam* param, int kind, float frame, float rate, u8 a, u8 b, u8 c, u8 d);

// Partner-side motion change: the partner switches to the new kind when its motion is the expected kind.
static void changeMotionOnPartner(soModuleAccesser* moduleAccesser, int expectKind, int newKind) {
    // HYPOTHESIS: the default frame and rate are the constants lbl_105_rodata_58 and lbl_105_rodata_5C (unverified)
    float frame = 0.0f;
    u8 haveFrame = 0;
    soMotionModule& motion = moduleAccesser->getMotionModule();
    if (motion.getKind() == expectKind) {
        frame = motion.getFrame();
        haveFrame = 1;
    }
    soMotionChangeParam param;
    fn_105_BD50(&param, newKind, frame, 1.0f, 0, 0, haveFrame, 0);
    motion.changeMotionRequest(&param);
}

class ftPopoStatusUniqProcessSpecialHiPartner : public soStatusUniqProcess {
public:
    virtual ~ftPopoStatusUniqProcessSpecialHiPartner() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftPopoStatusUniqProcessSpecialHiPartner::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().isLink(6) == 1) {
        moduleAccesser->getLinkModule().setAttribute(6, soLinkConnection::Attribute(0xc), true);
    }
}

void ftPopoStatusUniqProcessSpecialHiPartner::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000014) == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000014);
        changeMotionOnPartner(moduleAccesser, 0x1d7, 0x1de);
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000015) == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000015);
        changeMotionOnPartner(moduleAccesser, 0x1d6, 0x1df);
    }
}

// HYPOTHESIS: this is the partner's exit path. Only the partner-facing tail (direction sync and link attribute
// reset) is named here; the link and model queries before it are not reconstructed yet.
void ftPopoStatusUniqProcessSpecialHiPartner::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    Fighter* self = dynamic_cast<Fighter*>(&moduleAccesser->getStageObject());
    if (self == 0) {
        return;
    }
    Fighter* partner = self->getPartner();
    if (partner == 0) {
        return;
    }
    float partnerLr = soExternalValueAccesser::getLr(partner);
    moduleAccesser->getPostureModule().setLr(-partnerLr);
    moduleAccesser->getPostureModule().updateRotYLr();
    moduleAccesser->getLinkModule().setAttribute(6, soLinkConnection::Attribute(0xc), false);
}

ftPopoStatusUniqProcessSpecialHiPartner g_ftPopoStatusUniqProcessSpecialHiPartner;
