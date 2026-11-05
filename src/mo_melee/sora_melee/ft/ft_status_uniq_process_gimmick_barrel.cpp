#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_gimmick.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/stop/so_stop_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <types.h>
#include <so/so_value_accesser.h>
#include <so/so_kinetic_energy_normal.h>

void ftStatusUniqProcessGimmickBarrel::initStatus(soModuleAccesser* moduleAccesser) {
    ftStatusUniqProcessGimmick::initStatus(moduleAccesser);
    unk14 = -1;
    switch (moduleAccesser->getWorkManageModule().getInt(0x10000017)) {
    case 0x1F:
        moduleAccesser->getWorkManageModule().setInt(0, 0x20000000);
        moduleAccesser->getWorkManageModule().onFlag(0x22000010);
        break;
    case 0x20:
        moduleAccesser->getWorkManageModule().setInt(0, 0x20000000);
        moduleAccesser->getWorkManageModule().offFlag(0x22000010);
        break;
    case 0x21:
        moduleAccesser->getWorkManageModule().setInt(1, 0x20000000);
        moduleAccesser->getWorkManageModule().onFlag(0x22000010);
        break;
    case 0x22:
        moduleAccesser->getWorkManageModule().setInt(1, 0x20000000);
        moduleAccesser->getWorkManageModule().offFlag(0x22000010);
        break;
    }
    Vec3f pos = moduleAccesser->getPostureModule().getPos();
    soGimmickBarrelEventArgs_In args(pos);
    presentEvent(&args, moduleAccesser);
    moduleAccesser->getWorkManageModule().setInt(args.m_breakHitstopFrame, 0x20000002);
    pos.set(args.m_pos.m_x, args.m_pos.m_y, pos.m_z);
    moduleAccesser->getPostureModule().setPos(&pos);
    moduleAccesser->getWorkManageModule().onFlag(0x1200003C);
    float value = soValueAccesser::getConstantFloat(moduleAccesser, 0xD2D, 0);
    moduleAccesser->getWorkManageModule().setFloat(value, 0x11000012);
}

void ftStatusUniqProcessGimmickBarrel::execStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getWorkManageModule().getInt(0x20000000)) {
    case 0: {
        int trigger = moduleAccesser->getControllerModule().getTrigger();
        if ((trigger & soController::getButtonMask(soController::Pad_Button_Attack)) ||
            (trigger & soController::getButtonMask(soController::Pad_Button_Special))) {
            moduleAccesser->getWorkManageModule().setInt(1, 0x20000000);
            soGimmickBarrelEventArgs_ShootTrigger args(Vec3f(0.0f, 0.0f, 0.0f));
            presentEvent(&args, moduleAccesser);
        }
        break;
    }
    case 1:
        break;
    case 2: {
        soKineticEnergyNormal& energy = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3));
        float magnitude = energy.getSpeed().length();
        if (unk14 != -1 && magnitude < soValueAccesser::getConstantFloat(moduleAccesser, 0xCD1, 0)) {
            moduleAccesser->getEffectModule().removeContinual(unk14);
            unk14 = -1;
        }
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000010) &&
            magnitude < moduleAccesser->getWorkManageModule().getFloat(0x21000004)) {
            moduleAccesser->getStatusModule().changeStatusRequest(0x49, moduleAccesser);
        }
        moduleAccesser->getWorkManageModule().offFlag(0x1200003C);
        break;
    }
    }
}

void ftStatusUniqProcessGimmickBarrel::exitStatus(soModuleAccesser* moduleAccesser, int next) {
    ftStatusUniqProcessGimmick::exitStatus(moduleAccesser, next);
    moduleAccesser->getVisibilityModule().setWhole(true);
}

void ftStatusUniqProcessGimmickBarrel::execStop(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().getInt(0x20000000);
}

void ftStatusUniqProcessGimmickBarrel::execFixPos(soModuleAccesser* moduleAccesser) {
    bool breakA;
    bool breakB;
    bool breakC;
    bool breakD;
    moduleAccesser->getGroundModule().getAttackPointBreak(&breakA, &breakB, &breakC, &breakD, 0);
    if (breakA == true || breakB == true || breakC == true || breakD == true) {
        int hitStopFrame = moduleAccesser->getWorkManageModule().getInt(0x20000002);
        moduleAccesser->getStopModule().setHitStopFrame(hitStopFrame, 0);
    }
}
