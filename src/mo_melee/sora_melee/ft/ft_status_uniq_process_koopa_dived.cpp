#include <ft/ft_status_uniq_process_koopa_dived.h>
#include <ft/ft_status_uniq_process_views.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

ftStatusUniqProcessKoopaDived g_ftStatusUniqProcessKoopaDived;

void ftStatusUniqProcessKoopaDived::initStatus(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot3c();

    soMotionModule& motion = moduleAccesser->getMotionModule();
    int dived = 0;
    motion.setShareAnimObjKind(0x1cb);
    switch (soValueAccesser::getConstantInt(moduleAccesser, 0x5a98, 0)) {
    case 1:
        dived = 1;
        break;
    }
    motion.setKindOffset(dived);

    // HYPOTHESIS: frame and rate are the float constants lbl_27_rodata_2520 and lbl_27_rodata_2524.
    soMotionChangeParam motionParam;
    motionParam.m_kind = 0x1ba;
    motionParam.m_frame = 0.0f;
    motionParam.m_rate = 1.0f;
    motionParam._12 = 0;
    motionParam._13 = 1;
    motionParam._14 = 0;
    motionParam._15 = 0;
    motion.changeMotionRequest(&motionParam);
}

void ftStatusUniqProcessKoopaDived::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot44();
}

void ftStatusUniqProcessKoopaDived::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status != 0x42) {
        reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot4c(0);
    } else {
        reinterpret_cast<ftLinkModuleSlotView*>(&moduleAccesser->getLinkModule())->unlinkSlot164(1);
    }
}

bool ftStatusUniqProcessKoopaDived::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    return false;
}
