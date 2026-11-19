#include <mt/mt_vector.h>
#include <so/so_module_accesser.h>
#include <so/so_slow.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/stop/so_stop_module_impl.h>
#include <wn/fox/wn_fox_illusion_util.h>

// TODO: clean up - this is a local view of the trail query event (link event 0x455) that the Fox and Falco fighters answer; the
// fighters keep the same view in their own units (ftFoxLinkEventView), so it belongs in a shared header.
struct wnFoxIllusionTrailEvent {
    int kind;
    u8 flag;
    u8 pad[3];
    u32 value;
    Vec3f pos;
    float rotX;
};

// Moves the afterimage to the sample of the parent's posture trail that the event answers.
static inline void applyTrailSample(soModuleAccesser* moduleAccesser, wnFoxIllusionTrailEvent* event) {
    moduleAccesser->getLinkModule().sendEventParents(3, *reinterpret_cast<soLinkEventArgs*>(event));
    Vec3f pos = event->pos;
    Vec3f rot = moduleAccesser->getPostureModule().getRot(0);
    rot.m_x = event->rotX;
    moduleAccesser->getPostureModule().setPos(&pos);
    moduleAccesser->getPostureModule().setRot(&rot, 0);
}

// TODO: clean up - activate differs from the original only in where the status module pointer is loaded (the original loads it after
// the status argument is computed, this version hoists it above the compare); find the source form that keeps it late.
void wnFoxIllusionUtil::activate(soModuleAccesser* moduleAccesser, u8 situationKind, s32 followUp) {
    u32 isFollowUp = followUp;
    int status = isFollowUp == 1 ? 2 : situationKind == 2;
    soStatusModule& statusModule = moduleAccesser->getStatusModule();
    statusModule.changeStatusForce(status, moduleAccesser);
    if (isFollowUp == 1) {
        wnFoxIllusionTrailEvent event;
        event.kind = 0x455;
        event.flag = 0;
        event.value = 3;
        applyTrailSample(moduleAccesser, &event);
    } else {
        wnFoxIllusionTrailEvent event;
        event.kind = 0x455;
        event.flag = 0;
        event.value = 1;
        applyTrailSample(moduleAccesser, &event);
    }
    moduleAccesser->getStageObject().updateNodeSRT();
}

void wnFoxIllusionUtil::processUpdate(soModuleAccesser* moduleAccesser) {
    if (soSlow::getInstance()->isEstimate() == true && moduleAccesser->getStopModule().isStop() == 0) {
        u32 index = 0;
        if (moduleAccesser->getWorkManageModule().isFlag(0x12000003)) {
            index = 2;
        }
        wnFoxIllusionTrailEvent event;
        event.kind = 0x455;
        event.flag = 0;
        event.value = index;
        applyTrailSample(moduleAccesser, &event);
    }
}
