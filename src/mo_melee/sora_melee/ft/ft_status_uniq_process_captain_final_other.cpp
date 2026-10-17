#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/fighter.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: the Falcon Punch final smash sends fighters caught in it a link message; the message id sits right after
// the common link event header.
struct ftLinkEventCaptainFinalMessage : soLinkEventArgs {
    int m_message; // +8
    ftLinkEventCaptainFinalMessage() : soLinkEventArgs(0), m_message(0) { }
};

class ftStatusUniqProcessCaptainFinalOther : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptainFinalOther() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    static void linkMessageProcedure(soLinkEventArgs* args, soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessCaptainFinalOther g_ftStatusUniqProcessCaptainFinalOther;

void ftStatusUniqProcessCaptainFinalOther::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* workManage = moduleAccesser->m_enumerationStart->m_workManageModule;
    workManage->onFlag(0x22000011);
    workManage->onFlag(0x22000012);
    moduleAccesser->getKineticModule().clearSpeedAll();
}
void ftStatusUniqProcessCaptainFinalOther::exitStatus(soModuleAccesser* moduleAccesser, int) { }

// HYPOTHESIS: messages 0xA (start the hold), 0xB/0xC (hide / show the fighter and its effects), 0xD (clear flag
// 0x22000010) and 0xE (resume the paused fighter task).
void ftStatusUniqProcessCaptainFinalOther::linkMessageProcedure(soLinkEventArgs* args, soModuleAccesser* moduleAccesser) {
    soSituationModule& situation = moduleAccesser->getSituationModule();
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soStatusModule& status = moduleAccesser->getStatusModule();
    int situationKind = situation.getKind();
    soVisibilityModule& visibility = moduleAccesser->getVisibilityModule();
    Fighter& fighter = dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject);
    switch (static_cast<ftLinkEventCaptainFinalMessage*>(args)->m_message) {
    case 0xA:
        if (situationKind == 5) {
            fighter.setPaused(true);
        } else {
            switch (status.getStatusKind()) {
            case 0xD2:
            case 0xD3: {
                soMotionChangeParam param;
                param.m_kind = 0xA1;
                param.m_frame = 0.0f;
                param.m_rate = 0.0f;
                param._12 = 0;
                param._13 = 0;
                param._14 = 0;
                param._15 = 0;
                moduleAccesser->getMotionModule().changeMotionRequest(&param);
                break;
            }
            }
            status.changeStatusRequest(0xFB, moduleAccesser);
        }
        break;
    case 0xB:
        if (status.getStatusKind() == 0xFB) {
            moduleAccesser->getEffectModule().setWhole(false);
            visibility.setWhole(0);
            workManage.offFlag(0x12000019);
            workManage.offFlag(0x22000011);
        }
        break;
    case 0xC:
        if (status.getStatusKind() == 0xFB) {
            moduleAccesser->getEffectModule().setWhole(true);
            visibility.setWhole(1);
            workManage.onFlag(0x12000019);
            workManage.onFlag(0x22000011);
        }
        break;
    case 0xD:
        workManage.offFlag(0x22000010);
        break;
    case 0xE:
        fighter.setPaused(false);
        break;
    }
}
