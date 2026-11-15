#include <ft/metaknight/ft_metaknight_kinetic_transactor.h>
#include <ft/ft_kinetic_energy_controller.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

// TODO: changeKinetic (modes 0x64..0x6b).

static inline void updateIfActive(soKineticEnergy* energy, soModuleAccesser* acc) {
    if (energy->isEnable() == true && energy->isSuspend() == false) {
        energy->updateEnergy(acc);
    }
}

// A wall turns the fighter around and sends it back at the same speed.
static inline void bounceOffWall(ftKineticEnergyController* energy, soModuleAccesser* acc) {
    // BYTES: declared in this order for the original registers.
    bool touch;
    bool isWall = true;
    touch = false;
    if (energy->getSpeed().m_x >= 0.0f) {
        if (acc->getGroundModule().isTouch(static_cast<grCollStatus::TouchMask>(4), 0)) {
            touch = true;
        }
    }
    if (!touch) {
        touch = false;
        if (energy->getSpeed().m_x <= 0.0f) {
            if (acc->getGroundModule().isTouch(static_cast<grCollStatus::TouchMask>(2), 0)) {
                touch = true;
            }
        }
        if (!touch) {
            isWall = false;
        }
    }
    if (isWall) {
        energy->m_speed = Vec2f(-1.0f * energy->getSpeed().m_x, 0.0f);
        soPostureModule& posture = acc->getPostureModule();
        posture.setLr(-1.0f * posture.getLr());
        acc->getPostureModule().updateRotYLr();
    }
    updateIfActive(energy, acc);
}

// After enough frames with the flag set the fall speed gets a boost once; the fall speed is capped.
void ftMetaknightKineticTransactor::updateEnergySub(ftKineticEnergyGravity* energy, bool*, soModuleAccesser* acc) {
    soWorkManageModule& work = acc->getWorkManageModule();
    int frames = acc->getConstantIntKirby(0x5dc0);
    if (work.getInt(0x20000003) >= frames && acc->getWorkManageModule().isFlag(0x22000010)) {
        energy->m_speedY = energy->getSpeed().m_y + acc->getConstantFloatKirby(0xfaa);
        acc->getWorkManageModule().offFlag(0x22000010);
        acc->getWorkManageModule().setInt(0, 0x20000003);
    }
    if (energy->getSpeed().m_y > acc->getConstantFloatKirby(0xfab)) {
        energy->m_speedY = acc->getConstantFloatKirby(0xfab);
    }
    updateIfActive(energy, acc);
}

void ftMetaknightKineticTransactor::updateEnergySub1(ftKineticEnergyController* energy, bool*, soModuleAccesser* acc) {
    bounceOffWall(energy, acc);
}

void ftMetaknightKineticTransactor::updateEnergySub2(ftKineticEnergyController* energy, bool*, soModuleAccesser* acc) {
    bounceOffWall(energy, acc);
}

// The rotation follows the facing and the model's x rotation (degrees to radians).
void ftMetaknightKineticTransactor::updateEnergySub3(ftKineticEnergyMotion* energy, bool*, soModuleAccesser* acc) {
    int lr = acc->getPostureModule().getLr() < 0.0f ? -1 : 1;
    float facing = lr;
    Vec3f rot = acc->getPostureModule().getRot(0);
    energy->m_rotation = 0.017453292f * (-rot.m_x * facing);
    updateIfActive(energy, acc);
}

// On the ground a rise takes the fighter into the air; a fall is stopped.
void ftMetaknightKineticTransactor::updateEnergySub4(ftKineticEnergyController* energy, bool*, soModuleAccesser* acc) {
    updateIfActive(energy, acc);
    if (acc->getSituationModule().getKind() == 0) {
        if (energy->getSpeed().m_y > 0.0f) {
            acc->getSituationModule().setKind(static_cast<SituationKind>(2), false);
            acc->getGroundModule().setCorrect(static_cast<soGroundShapeImpl::CorrectKind>(5), 0);
        } else if (energy->getSpeed().m_y < 0.0f) {
            energy->m_speed = Vec2f(energy->getSpeed().m_x, 0.0f);
        }
    }
}

void ftMetaknightKineticTransactor::updateEnergySub5(ftKineticEnergyMotion* energy, bool*, soModuleAccesser* acc) {
    updateIfActive(energy, acc);
    if (acc->getSituationModule().getKind() == 0) {
        bool takeOff = false;
        if (energy->getSpeed().m_y > 0.05f) {
            if (acc->getWorkManageModule().isFlag(0x22000011) == false) {
                takeOff = true;
            }
        }
        if (takeOff) {
            acc->getSituationModule().setKind(static_cast<SituationKind>(2), false);
            acc->getGroundModule().setCorrect(static_cast<soGroundShapeImpl::CorrectKind>(5), 0);
        }
    }
}
