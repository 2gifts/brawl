#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_entry.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_util.h>
#include <so/motion/so_motion_change_param.h>
#include <so/model/so_model_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>


class ftDededeStatusUniqProcessSpecialSuperJumpHitCeil : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialSuperJumpHitCeil() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    void setTurnTransN(soModuleAccesser* moduleAccesser);
};

void ftDededeStatusUniqProcessSpecialSuperJumpHitCeil::initStatus(soModuleAccesser* moduleAccesser) {
    soMotionChangeParam change;
    change.m_kind = 0x1F9;
    change.m_frame = 0.0f;
    change.m_rate = 1.0f;
    change._12 = 0;
    change._13 = 0;
    change._14 = 0;
    change._15 = 0;
    moduleAccesser->getMotionModule().changeMotionRequest(&change);
    ftUtil::adjustCeil(moduleAccesser, true, moduleAccesser->getPostureModule().getLr());
    setTurnTransN(moduleAccesser);
    moduleAccesser->getKineticModule().getEnergy(0)->disable();
}

void ftDededeStatusUniqProcessSpecialSuperJumpHitCeil::execStatus(soModuleAccesser* moduleAccesser) {
    setTurnTransN(moduleAccesser);
    if (moduleAccesser->getWorkManageModule().getInt(0x20000002) <= soValueAccesser::getConstantInt(moduleAccesser, 0x5dc5, 0)) {
        moduleAccesser->getWorkManageModule().addInt(1, 0x20000002);
    }
    if (moduleAccesser->getWorkManageModule().getInt(0x20000002) == soValueAccesser::getConstantInt(moduleAccesser, 0x5dc5, 0)) {
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        Vec3f accel(0.0f, 0.0f, 0.0f);
        Vec2f speed(0.0f, 0.0f);
        gravity.resetEnergy(0, &speed, &accel, moduleAccesser);
        gravity.enable();
    }
}

void ftDededeStatusUniqProcessSpecialSuperJumpHitCeil::execStop(soModuleAccesser* moduleAccesser) {
    setTurnTransN(moduleAccesser);
}

void ftDededeStatusUniqProcessSpecialSuperJumpHitCeil::exitStatus(soModuleAccesser* moduleAccesser, int) {
    moduleAccesser->getModelModule().clearNodeSRT(5);
}

void ftDededeStatusUniqProcessSpecialSuperJumpHitCeil::setTurnTransN(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getPostureModule().getLr() == -1.0f) {
        Vec3f rotate = moduleAccesser->getModelModule().getNodeRotate(5);
        rotate.m_y = 180.0f;
        moduleAccesser->getModelModule().setNodeRotate(5, &rotate);
    }
}

ftDededeStatusUniqProcessSpecialSuperJumpHitCeil g_ftDededeStatusUniqProcessSpecialSuperJumpHitCeil;
