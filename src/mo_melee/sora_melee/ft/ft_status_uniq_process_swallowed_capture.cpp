#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_views.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/visibility/so_visibility_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);

// Link event record (0x28 bytes) that tells the swallower about the victim's stick flicks. HYPOTHESIS: kind 0x18 is the
// "struggle" event; the two floats carry the flick direction on x and y (-1, 0 or 1).
struct ftSwallowedStruggleEvent {
    int m_kind;
    u8 m_flag;
    int m_unk08;
    int m_unk0C;
    float m_flickX;
    float m_flickY;
    int m_unk18;
    u8 m_unk1C;
};

class ftStatusUniqProcessSwallowedCapture : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessSwallowedCapture() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* hit);
};

ftStatusUniqProcessSwallowedCapture g_ftStatusUniqProcessSwallowedCapture;

// Being swallowed: the victim's kinetic energies are cleared, it is hidden (visibility and effects off) and the
// controller starts to clatter with the constants of the swallowed kind (0xd51..0xd53 for status 0x20 or flag 0x22000013,
// 0xd3a..0xd3c for kind 5).
void ftStatusUniqProcessSwallowedCapture::initStatus(soModuleAccesser* moduleAccesser) {
    ftKineticEnergyDisableAndClear(3, moduleAccesser);
    ftKineticEnergyDisableAndClear(1, moduleAccesser);
    ftKineticEnergyDisableAndClear(2, moduleAccesser);
    ftKineticEnergyDisableAndClear(4, moduleAccesser);
    moduleAccesser->getVisibilityModule().setWhole(0);
    moduleAccesser->getEffectModule().setWhole(false);
    float clatterTime;
    float clatterP2;
    float clatterP3;
    int kind = moduleAccesser->getWorkManageModule().getInt(0x20000001);
    if (kind == 0x20 || moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
        clatterTime = soValueAccesser::getConstantFloat(moduleAccesser, 0xd53, 0);
        clatterP2 = soValueAccesser::getConstantFloat(moduleAccesser, 0xd52, 0);
        clatterP3 = soValueAccesser::getConstantFloat(moduleAccesser, 0xd51, 0);
    } else if (moduleAccesser->getWorkManageModule().getInt(0x20000001) == 5) {
        clatterTime = soValueAccesser::getConstantFloat(moduleAccesser, 0xd3c, 0);
        clatterP2 = soValueAccesser::getConstantFloat(moduleAccesser, 0xd3b, 0);
        clatterP3 = soValueAccesser::getConstantFloat(moduleAccesser, 0xd3a, 0);
    }
    reinterpret_cast<ftControllerClatterView*>(moduleAccesser->m_enumerationStart->m_controllerModule)->startClatter(clatterTime, clatterP2, clatterP3, 0, -1, 0, false);
    moduleAccesser->getWorkManageModule().onFlag(0x12000001);
    moduleAccesser->getWorkManageModule().onFlag(0x12000029);
}

// While swallowed the victim faces the swallower (negated link value) and reports its stick flicks to it: a flick up or
// down (y) or left or right (x) that is recent enough sends a struggle event with the direction and consumes the flick.
void ftStatusUniqProcessSwallowedCapture::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000011) == false) {
        reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).setLr(-reinterpret_cast<ftLinkModuleSlotView*>(&moduleAccesser->getLinkModule())->slotA8(0));
        moduleAccesser->getPostureModule().updateRotYLr();
        ftSwallowedStruggleEvent event;
        event.m_kind = 0x18;
        event.m_flag = 0;
        event.m_unk08 = -1;
        event.m_unk0C = 0xd2;
        event.m_flickX = 0.0f;
        event.m_flickY = 0.0f;
        event.m_unk18 = 0;
        event.m_unk1C = 1;
        soControllerModule* controller = &moduleAccesser->getControllerModule();
        if (soValueAccesser::getValueFloat(moduleAccesser, 0x3fb, 0) >= soValueAccesser::getValueFloat(moduleAccesser, 0xc56, 0)
            && moduleAccesser->getControllerModule().getFlickNoResetY() < soValueAccesser::getConstantInt(moduleAccesser, 0x59fb, 0)
            && controller->getFlickYDir() == 1) {
            event.m_flickY = 1.0f;
            reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slot44(0, &event);
            controller->resetFlickY();
        } else if (soValueAccesser::getValueFloat(moduleAccesser, 0x3fb, 0) >= soValueAccesser::getValueFloat(moduleAccesser, 0xc56, 0)
            && moduleAccesser->getControllerModule().getFlickNoResetY() < soValueAccesser::getConstantInt(moduleAccesser, 0x59fb, 0)
            && controller->getFlickYDir() == -1) {
            event.m_flickY = -1.0f;
            reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slot44(0, &event);
            controller->resetFlickY();
        }
        if (soValueAccesser::getValueFloat(moduleAccesser, 0x3f5, 0) >= soValueAccesser::getValueFloat(moduleAccesser, 0xc32, 0)
            && moduleAccesser->getControllerModule().getFlickNoResetX() < soValueAccesser::getConstantInt(moduleAccesser, 0x5a04, 0)
            && controller->getFlickXDir() == -1) {
            event.m_flickX = -1.0f;
            reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slot44(0, &event);
            controller->resetFlickX();
        } else if (soValueAccesser::getValueFloat(moduleAccesser, 0x3f5, 0) >= soValueAccesser::getValueFloat(moduleAccesser, 0xc32, 0)
            && moduleAccesser->getControllerModule().getFlickNoResetX() < soValueAccesser::getConstantInt(moduleAccesser, 0x5a04, 0)
            && controller->getFlickXDir() == 1) {
            event.m_flickX = 1.0f;
            reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slot44(0, &event);
            controller->resetFlickX();
        }
    }
}

// Once the controller's clatter timer has run out the victim is released: into the thrown status (0xd5) for the
// swallowed-kind 0x20 or the flagged case, otherwise status 0x40.
void ftStatusUniqProcessSwallowedCapture::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot44();
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000011) == false) {
        if (moduleAccesser->getControllerModule().getClatterTime(0) <= 0.0f) {
            moduleAccesser->getCollisionHitModule().cancelXluGlobal(0);
            if (moduleAccesser->getWorkManageModule().getInt(0x20000001) == 0x20 || moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
                moduleAccesser->getStatusModule().changeStatusRequest(0xd5, moduleAccesser);
            } else {
                moduleAccesser->getStatusModule().changeStatusRequest(0x40, moduleAccesser);
            }
        }
    }
}

// Leaving: the victim is shown again; when it is thrown (0xd3) the swallower's value is kept for the throw.
void ftStatusUniqProcessSwallowedCapture::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    switch (nextStatusKind) {
    case 0xd3: {
        ftLinkModuleSlotView* link = reinterpret_cast<ftLinkModuleSlotView*>(&moduleAccesser->getLinkModule());
        soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
        float value = link->slotA8(0);
        work->setFloat(value, 0x21000009);
        reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot30(1, 0);
        break;
    }
    case 0xd5:
        return;
    case 0x40:
    default:
        reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot30(1, 0);
        moduleAccesser->getCollisionHitModule().cancelXluGlobal(0);
        moduleAccesser->getVisibilityModule().setWhole(1);
        moduleAccesser->getEffectModule().setWhole(true);
        break;
    }
}

bool ftStatusUniqProcessSwallowedCapture::checkDamage(soModuleAccesser* moduleAccesser, void* hit) {
    return false;
}
