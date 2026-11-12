#define MT_VEC2F_CTOR_NOINLINE // BYTES: the original builds the clamped speed pair through the out-of-line constructor
#define MT_VEC2F_ASSIGN_NOINLINE // BYTES: ... and assigns it through the out-of-line operator=
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_kinetic_energy_controller.h>
#include <math.h>
#include <so/camera/so_camera_module_simple.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: the shared helpers of Giga Bowser's up special (emitted at the start of this unit, called only by its
// status processes; the module map names them soModuleAccesser::ftKineticEnergyUnable3 and initStatusCommon2): the first
// clears the update flag (0x80) of the four kinetic energies, the second resets the work state and, in the air, restarts the
// gravity and controller energies from the Giga Bowser extend parameters (index 2).
extern "C" void fn_102_156C0(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getKineticModule().getEnergy(0)->disable();
    moduleAccesser->getKineticModule().getEnergy(1)->disable();
    moduleAccesser->getKineticModule().getEnergy(2)->disable();
    moduleAccesser->getKineticModule().getEnergy(3)->disable();
}

extern "C" void fn_102_15788(soModuleAccesser* moduleAccesser) {
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
        float* param = reinterpret_cast<float*>(g_ftCommonDataAccesser.getData(Fighter_GKoopa)->extendParam[2]);
        Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
        Vec2f airSpeed = speed;
        airSpeed.m_y = param[1];
        fn_102_156C0(moduleAccesser);
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.resetEnergy(0, &Vec2f(0.0f, airSpeed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.m_gravity = -param[2];
        gravity.m_fallSpeedMax = param[3];
        gravity.unk1C = param[3];
        gravity.enable();
        ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
        controller.resetEnergy(0, &Vec2f(airSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        controller.m_accelMul = param[7];
        controller.m_speedTarget = Vec2f(param[5], controller.m_speedTarget.m_y);
        controller.m_speedLimit = Vec2f(param[5], controller.m_speedLimit.m_y);
        controller.enable();
    }
}
