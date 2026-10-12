// Reconstructed native Cypher Hang status behavior.
// HYPOTHESIS: the accessor result has a +0x34 field pointing to
// this parameter block. The accessor identity and the seven parameter meanings
// still need source-interface review; field labels stay unknown pending that review.
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#include <ft/fighter.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftSnakeStatusUniqProcessCypherHang : public soStatusUniqProcess {
public:
    virtual ~ftSnakeStatusUniqProcessCypherHang() {}
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

struct ftSnakeCypherHangParams {
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    float unk10;
    float unk14;
    float unk18;
};

struct ftSnakeCypherHangDataView {
    u8 unk0[0x34];
    ftSnakeCypherHangParams* unk34;
};

// HYPOTHESIS: fn_27_147F88 is the native accessor call at the target callsite.
// Its callee ignores the forwarded kind and output-bool registers.
extern "C" void* fn_27_147F88(
    ftCommonDataAccesser* accesser, int kind, bool* resourceGroup);

void ftSnakeStatusUniqProcessCypherHang::initStatus(
    soModuleAccesser* moduleAccesser) {
    bool resourceGroup = false;
    int ownerKind = moduleAccesser->getStageObject().soGetSubKind();
    ftSnakeCypherHangDataView* data =
        static_cast<ftSnakeCypherHangDataView*>(fn_27_147F88(
            &g_ftCommonDataAccesser, ownerKind, &resourceGroup));
    const ftSnakeCypherHangParams* param = data->unk34;

    Vec2f speed;
    Vec2f::copy(
        speed,
        moduleAccesser->getKineticModule().getSumSpeed(
            soKineticEnergy::AttributeFlag(1)));

    ftKineticEnergyGravity& gravity =
        dynamic_cast<ftKineticEnergyGravity&>(
            *moduleAccesser->getKineticModule().getEnergy(1));
    gravity.enable();
    if (speed.m_y < -param->unk0)
        speed.m_y = -param->unk0;
    Vec2f gravitySpeed(0.0f, speed.m_y);
    Vec3f gravityAccel(0.0f, 0.0f, 0.0f);
    gravity.resetEnergy(1, &gravitySpeed, &gravityAccel, moduleAccesser);
    gravity.m_gravity = param->unk4;
    gravity.m_fallSpeedMax = param->unk8;
    gravity.unk1C = param->unkC;
    gravity.enable();

    ftKineticEnergyController& controller =
        dynamic_cast<ftKineticEnergyController&>(
            *moduleAccesser->getKineticModule().getEnergy(2));
    controller.enable();
    Vec2f controllerSpeed(speed.m_x, 0.0f);
    Vec3f controllerAccel(0.0f, 0.0f, 0.0f);
    controller.resetEnergy(0, &controllerSpeed, &controllerAccel, moduleAccesser);
    controller.m_accelMul = param->unk10;
    Vec2f speedTarget(param->unk14, 0.0f);
    controller.m_speedTarget = speedTarget;
    Vec2f speedLimit(param->unk18, 0.0f);
    controller.m_speedLimit = speedLimit;

    moduleAccesser->getKineticModule().getEnergy(0)->disable();
    moduleAccesser->getKineticModule().getEnergy(3)->disable();
}

void ftSnakeStatusUniqProcessCypherHang::execStatus(
    soModuleAccesser* moduleAccesser) {
    bool shouldClamp = false;
    if (moduleAccesser->getGroundModule().isTouch(8, 0)) {
        soKineticEnergy* energy = moduleAccesser->getKineticModule().getEnergy(1);
        if (energy->getSpeed().m_y <= 0.0f)
            shouldClamp = true;
    }

    if (shouldClamp) {
        ftKineticEnergyGravity& gravity =
            dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.m_speedY = 0.0f;
    }
}

void ftSnakeStatusUniqProcessCypherHang::exitStatus(
    soModuleAccesser*, int) {
}

ftSnakeStatusUniqProcessCypherHang g_ftSnakeStatusUniqProcessCypherHang;
