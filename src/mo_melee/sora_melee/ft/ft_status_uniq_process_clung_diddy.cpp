#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// Use the native Fighter observer override before transitive Fighter includes;
// this preserves the verified primary vtable slot of the stage object calls.
#define FT_MODULE_BUILDER
#include <ft/ft_owner.h>
#include <ft/fighter.h>
#undef FT_MODULE_BUILDER
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_status_uniq_process_views.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/stageobject.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: capture module (soModuleEnumeration::m_captureModule, +0x40)
class ftCaptureModuleView {
public:
    virtual ~ftCaptureModuleView() { }
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual void unk24();
    virtual void unk28();
    virtual void unk2C();
    virtual void unk30(int, int);
    virtual void unk34();
    virtual bool unk38(soModuleAccesser* moduleAccesser, void* hit);
    virtual void unk3C();
    virtual void unk40();
    virtual void unk44(soModuleAccesser* moduleAccesser);
    virtual void unk48();
    virtual void unk4C(int);
};

class ftStatusUniqProcessClungDiddy : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessClungDiddy() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int status);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* hit);
};

ftStatusUniqProcessClungDiddy g_ftStatusUniqProcessClungDiddy;

// HYPOTHESIS: Diddy Kong clinging onto a fighter: the victim turns away from the link, certain victims (fighter kinds 5, 0x12
// and 0x25) get work flag 0x22000010, the shake-off strength (work floats 0x21000004/5) grows with the victim's damage and a
// controller clatter of that strength starts (the second strength is used while airborne).
void ftStatusUniqProcessClungDiddy::initStatus(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).setLr(-reinterpret_cast<ftLinkModuleSlotView*>(&moduleAccesser->getLinkModule())->slotA8(0));
    moduleAccesser->getPostureModule().updateRotYLr();
    moduleAccesser->m_stageObject->updateNodeSRT();
    Fighter& victim = dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject);
    if (victim.getFtKind() == 5 || victim.getFtKind() == 0x12 || victim.getFtKind() == 0x25) {
        moduleAccesser->getWorkManageModule().onFlag(0x22000010);
    }
    float base = soValueAccesser::getConstantFloat(moduleAccesser, 0xdb2, 0);
    float strength = moduleAccesser->getDamageModule().getDamage(0);
    strength = strength * soValueAccesser::getConstantFloat(moduleAccesser, 0xd60, 0);
    strength = strength + soValueAccesser::getConstantFloat(moduleAccesser, 0xd5f, 0);
    float shakeStrength = strength + base * soValueAccesser::getConstantFloat(moduleAccesser, 0xd61, 0);
    moduleAccesser->getWorkManageModule().setFloat(shakeStrength, 0x21000004);
    float airStrength = shakeStrength * soValueAccesser::getConstantFloat(moduleAccesser, 0xd62, 0);
    moduleAccesser->getWorkManageModule().setFloat(airStrength, 0x21000005);
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        shakeStrength = airStrength;
    }
    float clatterP2 = 1.0f;
    reinterpret_cast<ftControllerClatterView*>(moduleAccesser->m_enumerationStart->m_controllerModule)->startClatter(shakeStrength, clatterP2, soValueAccesser::getConstantFloat(moduleAccesser, 0xd63, 0), 0, -1, 0, false);
}

// HYPOTHESIS: while clung to the victim a flick on the control stick (stronger than the constant 0x5a04 and with a stick
// travel above value 0xc32) is a shake: the victim's stop energy is pushed to the side by the constant 0xd64.
void ftStatusUniqProcessClungDiddy::execStatus(soModuleAccesser* moduleAccesser) {
    soControllerModule* controller = &moduleAccesser->getControllerModule();
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    Vec2f push;
    push.m_x = 0.0f;
    push.m_y = 0.0f;
    bool shaken = true;
    if (soValueAccesser::getValueFloat(moduleAccesser, 0x3f5, 0) >= soValueAccesser::getValueFloat(moduleAccesser, 0xc32, 0)
        && (u8)moduleAccesser->getControllerModule().getFlickNoResetX() < soValueAccesser::getConstantInt(moduleAccesser, 0x5a04, 0)
        && (s8)controller->getFlickXDir() == -1) {
        push.m_x = -soValueAccesser::getConstantFloat(moduleAccesser, 0xd64, 0);
        push.m_y = 0.0f;
        controller->resetFlickX();
    } else if (soValueAccesser::getValueFloat(moduleAccesser, 0x3f5, 0) >= soValueAccesser::getValueFloat(moduleAccesser, 0xc32, 0)
        && (u8)moduleAccesser->getControllerModule().getFlickNoResetX() < soValueAccesser::getConstantInt(moduleAccesser, 0x5a04, 0)
        && (s8)controller->getFlickXDir() == 1) {
        push.m_x = soValueAccesser::getConstantFloat(moduleAccesser, 0xd64, 0);
        push.m_y = 0.0f;
        controller->resetFlickX();
    } else {
        shaken = false;
    }
    if (shaken) {
        if (moduleAccesser->getSituationModule().getKind() == 2) {
            stop.resetEnergy(6, &push, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        } else {
            stop.resetEnergy(0, &push, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        }
    }
}

// HYPOTHESIS: the clatter time (work floats 0x21000004/5 minus the elapsed time, by situation) runs out when the victim
// shakes the clinging fighter off: status 0x40.
void ftStatusUniqProcessClungDiddy::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getControllerModule().getClatterTime(0) <= 0.0f) {
        moduleAccesser->getStatusModule().changeStatusRequest(0x40, moduleAccesser);
    } else if (moduleAccesser->getSituationModule().isSituationChanged()) {
        float clatterTime = moduleAccesser->getControllerModule().getClatterTime(0);
        float remaining;
        if (moduleAccesser->getSituationModule().getKind() == 0) {
            float elapsed = moduleAccesser->getWorkManageModule().getFloat(0x21000005) - clatterTime;
            remaining = moduleAccesser->getWorkManageModule().getFloat(0x21000004) - elapsed;
        } else {
            float elapsed = moduleAccesser->getWorkManageModule().getFloat(0x21000004) - clatterTime;
            remaining = moduleAccesser->getWorkManageModule().getFloat(0x21000005) - elapsed;
        }
        if (remaining <= 0.0f) {
            moduleAccesser->getStatusModule().changeStatusRequest(0x40, moduleAccesser);
        } else {
            moduleAccesser->getControllerModule().setClatterTime(remaining, 0);
        }
    }
}

bool ftStatusUniqProcessClungDiddy::checkDamage(soModuleAccesser* moduleAccesser, void* hit) {
    return false;
}

void ftStatusUniqProcessClungDiddy::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status != 0xd7) {
        reinterpret_cast<ftCaptureModuleView*>(moduleAccesser->m_enumerationStart->m_captureModule)->unk30(0, 0);
    }
}
