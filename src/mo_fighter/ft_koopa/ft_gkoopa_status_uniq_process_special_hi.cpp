#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define MT_VEC2F_ASSIGN_NOINLINE // BYTES: the original assigns the clamped target speeds through the out-of-line operator=
#define MT_VEC3F_CTOR_NOINLINE // BYTES: every zero rotation passed to resetEnergy goes through the out-of-line constructor
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_kinetic_energy_controller.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/motion/so_motion_change_param.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/slope/so_slope_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/visibility/so_visibility_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

#include <ft/koopa/ft_koopa_special_hi.h>

// HYPOTHESIS: the shared helpers of Giga Bowser's up special (emitted at the start of this unit, called only by its status
// processes): the first clears the update flag (0x80) of the four kinetic energies, the second resets the work state
// and, in the air, restarts the gravity and controller energies from the special hi parameters.
static inline void setControllerVec(Vec2f& dst, const Vec2f& src) {
    dst = src;
}

static void ftKineticEnergyUnable(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getKineticModule().getEnergy(0)->disable();
    moduleAccesser->getKineticModule().getEnergy(1)->disable();
    moduleAccesser->getKineticModule().getEnergy(2)->disable();
    moduleAccesser->getKineticModule().getEnergy(3)->disable();
}

static void initStatusCommon(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    int situation = moduleAccesser->getSituationModule().getKind();
    work.clearAll(2);
    work.offFlag(0x22000010);
    work.offFlag(0x22000011);
    work.offFlag(0x22000012);
    work.offFlag(0x22000013);
    work.offFlag(0x22000014);
    work.onFlag(0x22000015);
    work.setInt(0, 0x20000000);
    work.offFlag(0x1200003f);
    if (situation == 2) {
        ftKoopaSpecialHiParam* param = getKoopaSpecialHiParam(Fighter_GKoopa, 2);
        Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
        speed.m_y = param->m_airSpeedY;
        ftKineticEnergyUnable(moduleAccesser);
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.resetEnergy(0, &Vec2f(0.0f, speed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.m_gravity = -param->m_gravity;
        gravity.m_fallSpeedMax = param->m_fallSpeedMax;
        gravity.unk1C = param->m_fallSpeedMax;
        gravity.enable();
        ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
        controller.resetEnergy(0, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        controller.m_accelMul = param->m_airAccelMul;
        setControllerVec(controller.m_speedTarget, Vec2f(param->m_airSpeedX, controller.m_speedTarget.m_y));
        setControllerVec(controller.m_speedLimit, Vec2f(param->m_airSpeedX, controller.m_speedLimit.m_y));
        controller.enable();
    }
}

class ftGKoopaStatusUniqProcessSpecialHiG : public soStatusUniqProcess {
public:
    virtual ~ftGKoopaStatusUniqProcessSpecialHiG() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
};

class ftGKoopaStatusUniqProcessSpecialHiA : public soStatusUniqProcess {
public:
    virtual ~ftGKoopaStatusUniqProcessSpecialHiA() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
};

void ftGKoopaStatusUniqProcessSpecialHiG::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (work.isFlag(0x1200003d)) {
        initStatusCommon(moduleAccesser);
        work.offFlag(0x1200003d);
    }
    soItemManageModule& items = moduleAccesser->getItemManageModule();
    // BYTES: here the flag and the parameter pointer are plain locals (numbered below the motion module getter of the
    // resume branch); in the air version the flag goes through the inline helper.
    bool visible = !work.isFlag(0x22000013);
    items.setHaveItemVisibility(visible, 0);
    items.setAttachItemVisibility(visible, static_cast<itParam::AttachGroup>(0xff));
    Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    ftKoopaSpecialHiParam* param = static_cast<ftKoopaSpecialHiParam*>(g_ftCommonDataAccesser.getData(Fighter_GKoopa)->extendParam[2]);
    work.setInt(0, 0x20000000);
    ftKineticEnergyUnable(moduleAccesser);
    if (!work.isFlag(0x22000010)) {
        ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
        controller.resetEnergy(0, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        controller.m_accelMul = param->m_groundAccelMul;
        setControllerVec(controller.m_speedTarget, Vec2f(param->m_groundSpeedX, controller.m_speedTarget.m_y));
        setControllerVec(controller.m_speedLimit, Vec2f(param->m_groundSpeedX, controller.m_speedLimit.m_y));
        controller.enable();
    } else {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
        stop.resetEnergy(0, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop.enable();
    }
    if (work.isFlag(0x22000015)) {
        changeKoopaMotion(moduleAccesser, soMotionChangeParam(0x1d4, 0.0f, 1.0f, 0, 0, 0, 0));
        work.offFlag(0x22000015);
    } else if (work.isFlag(0x22000016)) {
        float frame;
        if (!work.isFlag(0x1200003f)) {
            work.onFlag(0x1200003f);
            soMotionModule& motion = moduleAccesser->getMotionModule();
            float delay = soValueAccesser::getConstantInt(moduleAccesser, 0x5dc8, 0);
            frame = motion.getFrame() - delay;
        } else {
            frame = moduleAccesser->getMotionModule().getFrame();
        }
        changeKoopaMotion(moduleAccesser, soMotionChangeParam(0x1d4, 1.0f, 1.0f, 0, 0, 0, 0));
        moduleAccesser->getMotionModule().setFrameSyncAnimCmd(frame);
        work.offFlag(0x22000016);
    } else {
        changeKoopaMotion(moduleAccesser, soMotionChangeParam(0x1d4, 0.0f, 1.0f, 0, 0, 0, 0));
    }
    if (work.isFlag(0x22000013)) {
        moduleAccesser->getVisibilityModule().set(1, 1);
    } else {
        moduleAccesser->getVisibilityModule().set(1, 0);
    }
}

void ftGKoopaStatusUniqProcessSpecialHiG::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (work.isFlag(0x22000010) && !work.isFlag(0x22000011)) {
        Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
        moduleAccesser->getKineticModule().clearSpeedAll();
        ftKineticEnergyUnable(moduleAccesser);
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
        stop.resetEnergy(0, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop.enable();
        work.onFlag(0x22000011);
    }
    work.incInt(0x20000000);
    soSlopeModule& slope = *static_cast<soSlopeModule*>(moduleAccesser->m_enumerationStart->m_slopeModule);
    if (work.isFlag(0x22000014)) {
        if (slope.getStatus() != soSlopeStatusParam::STATUS_MASK_TOP) {
            slope.setStatus(soSlopeStatusParam::STATUS_MASK_TOP, 6, true);
        }
    } else if (slope.getStatus() != soSlopeStatusParam::STATUS_MASK_LR) {
        slope.setStatus(soSlopeStatusParam::STATUS_MASK_LR, 3, false);
    }
    if (work.isFlag(0x22000013)) {
        moduleAccesser->getVisibilityModule().set(1, 1);
    } else {
        moduleAccesser->getVisibilityModule().set(1, 0);
    }
}

void ftGKoopaStatusUniqProcessSpecialHiA::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (work.isFlag(0x1200003d)) {
        initStatusCommon(moduleAccesser);
        work.offFlag(0x1200003d);
    }
    soItemManageModule& items = moduleAccesser->getItemManageModule();
    bool visible = isKoopaFlagOff(work, 0x22000013);
    items.setHaveItemVisibility(visible, 0);
    items.setAttachItemVisibility(visible, static_cast<itParam::AttachGroup>(0xff));
    work.setInt(0, 0x20000000);
    if (!work.isFlag(0x22000010)) {
        ftKoopaSpecialHiParam* param = getKoopaSpecialHiParam(Fighter_GKoopa, 2);
        Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
        ftKineticEnergyUnable(moduleAccesser);
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.resetEnergy(0, &Vec2f(0.0f, speed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.m_gravity = -param->m_gravity;
        gravity.m_fallSpeedMax = param->m_fallSpeedMax;
        gravity.unk1C = param->m_fallSpeedMax;
        gravity.enable();
        ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
        controller.resetEnergy(0, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        controller.m_accelMul = param->m_airAccelMul;
        setControllerVec(controller.m_speedTarget, Vec2f(param->m_airSpeedX, controller.m_speedTarget.m_y));
        setControllerVec(controller.m_speedLimit, Vec2f(param->m_airSpeedX, controller.m_speedLimit.m_y));
        controller.enable();
    } else {
        moduleAccesser->getKineticModule().changeKinetic(0, moduleAccesser);
    }
    if (work.isFlag(0x22000012)) {
        moduleAccesser->getVisibilityModule().set(1, 1);
    }
}

void ftGKoopaStatusUniqProcessSpecialHiA::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (work.isFlag(0x22000010) && !work.isFlag(0x22000011)) {
        moduleAccesser->getKineticModule().changeKinetic(0, moduleAccesser);
        work.onFlag(0x22000011);
    }
    work.incInt(0x20000000);
    if (work.isFlag(0x22000012)) {
        moduleAccesser->getVisibilityModule().set(1, 1);
    } else {
        moduleAccesser->getVisibilityModule().set(1, 0);
    }
}

void ftGKoopaStatusUniqProcessSpecialHiA::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (status == 0x117) {
        ftKoopaSpecialHiParam* param = getKoopaSpecialHiParam(Fighter_GKoopa, 2);
        moduleAccesser->getWorkManageModule().setFloat(param->unk2C, 0x11000000);
        float frame = moduleAccesser->getMotionModule().getFrame();
        if (frame > param->unk20) {
            work.onFlag(0x22000016);
            if (frame - param->unk28 < param->unk24) {
                work.onFlag(0x22000013);
            }
        }
    } else {
        ftData* data = g_ftCommonDataAccesser.getData(Fighter_GKoopa);
        work.setFloat(static_cast<ftKoopaSpecialHiParam*>(data->extendParam[2])->unk2C, 0x11000000);
    }
}

ftGKoopaStatusUniqProcessSpecialHiG g_ftGKoopaStatusUniqProcessSpecialHiG;
ftGKoopaStatusUniqProcessSpecialHiA g_ftGKoopaStatusUniqProcessSpecialHiA;

