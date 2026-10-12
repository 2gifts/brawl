#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// MATCH-ONLY: keep native out-of-line Vec3f construction for resetEnergy inputs.
#define MT_VEC3F_CTOR_NOINLINE
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Verified native constants used by the ascent unit: zero, facing-left LR, and node yaw.
static const float s_superJumpJumpConst[3] = { 0.0f, -1.0f, 180.0f };

class ftDededeStatusUniqProcessSpecialSuperJumpJump : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialSuperJumpJump() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int status);
};

void ftDededeStatusUniqProcessSpecialSuperJumpJump::initStatus(soModuleAccesser* moduleAccesser) {
    const float* k = s_superJumpJumpConst;
    float stickX = moduleAccesser->getControllerModule().getStickX();
    float horizontalSpeed = stickX * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb7, 0);
    float verticalSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb8, 0);
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        verticalSpeed *= soValueAccesser::getConstantFloat(moduleAccesser, 0xfba, 0);
    }
    verticalSpeed -= __fabsf(stickX) * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb9, 0);

    ftKineticEnergyGravity& vertical = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    Vec2f verticalInitial(0.0f, verticalSpeed);
    vertical.resetEnergy(0, &verticalInitial, &Vec3f(k[0], k[0], k[0]), moduleAccesser);
    vertical.m_gravity = -soValueAccesser::getConstantFloat(moduleAccesser, 0xfbb, 0);
    vertical.enable();

    ftKineticEnergyStop& horizontal = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    Vec2f horizontalInitial(horizontalSpeed, 0.0f);
    horizontal.resetEnergy(0x16, &horizontalInitial, &Vec3f(k[0], k[0], k[0]), moduleAccesser);
    horizontal.m_accel = Vec2f(0.0f, 0.0f);
    horizontal.m_speedTarget = Vec2f(horizontalSpeed, 0.0f);
    horizontal.m_brake = Vec2f(0.0f, 0.0f);
    horizontal.m_speedLimit = Vec2f(horizontalSpeed, 0.0f);
    horizontal.enable();
    moduleAccesser->getKineticModule().getEnergy(2)->disable();

    moduleAccesser->getWorkManageModule().onFlag(0x22000011);
    if (moduleAccesser->getPostureModule().getLr() == -1.0f) {
        Vec3f original = moduleAccesser->getModelModule().getNodeRotate(7);
        Vec3f rotate;
        rotate.m_x = original.m_x;
        rotate.m_y = 180.0f;
        rotate.m_z = original.m_z;
        moduleAccesser->getModelModule().setNodeRotate(7, &rotate);
    }
    moduleAccesser->getWorkManageModule().offFlag(0x22000015);
}

void ftDededeStatusUniqProcessSpecialSuperJumpJump::execStatus(soModuleAccesser* moduleAccesser) {
    const float* k = s_superJumpJumpConst;
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (work.isFlag(0x22000011)) {
        work.addInt(1, 0x20000001);
        if (work.getInt(0x20000001) > soValueAccesser::getConstantInt(moduleAccesser, 0x5dc4, 0)) {
            soKineticEnergy::AttributeFlag flag(1);
            Vec2f sumSpeed = moduleAccesser->getKineticModule().getSumSpeed(flag);
            ftKineticEnergyGravity& vertical = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
            Vec2f speed(0.0f, sumSpeed.m_y);
            vertical.resetEnergy(0, &speed, &Vec3f(k[0], k[0], k[0]), moduleAccesser);
            vertical.enable();
            work.offFlag(0x22000011);
            work.onFlag(0x22000015);
        }
    }
    if (moduleAccesser->getPostureModule().getLr() == -1.0f) {
        Vec3f original = moduleAccesser->getModelModule().getNodeRotate(7);
        Vec3f rotate;
        rotate.m_x = original.m_x;
        rotate.m_y = 180.0f;
        rotate.m_z = original.m_z;
        moduleAccesser->getModelModule().setNodeRotate(7, &rotate);
    }
}

void ftDededeStatusUniqProcessSpecialSuperJumpJump::execStop(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getPostureModule().getLr() == -1.0f) {
        Vec3f original = moduleAccesser->getModelModule().getNodeRotate(7);
        Vec3f rotate;
        rotate.m_x = original.m_x;
        rotate.m_y = 180.0f;
        rotate.m_z = original.m_z;
        moduleAccesser->getModelModule().setNodeRotate(7, &rotate);
    }
}

void ftDededeStatusUniqProcessSpecialSuperJumpJump::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status == 0x12b || status == 0x12e || status == 0x12c) {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        if (work.isFlag(0x22000010)) {
            moduleAccesser->getPostureModule().setLr(-1.0f);
        } else {
            moduleAccesser->getPostureModule().setLr(1.0f);
        }
    }
    moduleAccesser->getModelModule().clearNodeSRT(7);
}

ftDededeStatusUniqProcessSpecialSuperJumpJump g_ftDededeStatusUniqProcessSpecialSuperJumpJump;