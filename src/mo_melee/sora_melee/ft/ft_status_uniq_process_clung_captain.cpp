#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_views.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/stageobject.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessClungCaptain : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessClungCaptain() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessClungCaptain g_ftStatusUniqProcessClungCaptain;

// HYPOTHESIS: the clung fighter faces away from the link (negated link lr) and uses motion kind offset 1 when constant 0x5a98 is 1.
void ftStatusUniqProcessClungCaptain::initStatus(soModuleAccesser* moduleAccesser) {
    soPostureModule* posture = moduleAccesser->m_enumerationStart->m_postureModule;
    posture->setLr(-reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slotA8(0));
    moduleAccesser->getPostureModule().updateRotYLr();
    moduleAccesser->m_stageObject->updateNodeSRT();
    soMotionModule* motion = moduleAccesser->m_enumerationStart->m_motionModule;
    int kindOffset = 0;
    switch (soValueAccesser::getConstantInt(moduleAccesser, 0x5a98, 0)) {
    case 1:
        kindOffset = 1;
        break;
    }
    motion->setKindOffset(kindOffset);
}

bool ftStatusUniqProcessClungCaptain::checkDamage(soModuleAccesser* moduleAccesser, void* damageInfo) {
    return reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot38(moduleAccesser, damageInfo);
}

// HYPOTHESIS: leaving the cling for anything but status 0x42 releases the capture (slots 0x54 then 0x4c).
void ftStatusUniqProcessClungCaptain::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x42) {
        reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot54(1);
        reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot4c(0);
    }
}

