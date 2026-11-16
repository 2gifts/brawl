#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <ec/ec_mgr.h>
#include <wn/samus/wn_samus_cshot.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSamusCShotStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnSamusCShotStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnSamusCShotStatusUniqProcess g_wnSamusCShotStatusUniqProcess;

// Held (status 0): the charge effect starts at node 0 (kind 3 is Kirby's copy). Shot (1..8): the speed grows with the
// charge (work float 0x11000001) and the shot flies at the parameter angle in the facing direction.
void wnSamusCShotStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0: {
        const wnSamusCShotParam* param = dynamic_cast<wnSamusCShot&>(moduleAccesser->getStageObject()).getData()->m_param;
        moduleAccesser->getWorkManageModule().setInt(param->unk0, 0x10000005);
        Vec3f offset(0.0f, 0.0f, 0.0f);
        // HYPOTHESIS: the fighter kind is read here and not used.
        moduleAccesser->getStageObject().soGetSubKind();
        u32 effectId;
        if (moduleAccesser->getWorkManageModule().getInt(0x10000007) == 3) {
            effectId = moduleAccesser->getEffectModule().req(static_cast<EfID>(0x40003), &moduleAccesser->getModelModule().getNodeGlobalPosition(0, &offset, false, false), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
        } else {
            effectId = moduleAccesser->getEffectModule().req(static_cast<EfID>(0x1080001), &moduleAccesser->getModelModule().getNodeGlobalPosition(0, &offset, false, false), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
        }
        moduleAccesser->getWorkManageModule().setInt(effectId, 0x10000006);
        break;
    }
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8: {
        const wnSamusCShotParam* param = dynamic_cast<wnSamusCShot&>(moduleAccesser->getStageObject()).getData()->m_param;
        float lr = moduleAccesser->getPostureModule().getLr();
        float charge = moduleAccesser->getWorkManageModule().getFloat(0x11000001);
        float angle = param->unk4;
        float speed = param->unk8 + charge * (param->unkC - param->unk8);
        Vec2f velocity;
        velocity.m_x = lr * (speed * (float)cos(angle));
        velocity.m_y = speed * (float)sin(angle);
        wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
        energy.resetEnergy(0, &velocity, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        energy.m_speedTarget = Vec2f(3.4028235e38f, 3.4028235e38f);
        energy.enable();
        moduleAccesser->getCollisionAttackModule().setSpeed(&energy.getSpeed());
        break;
    }
    }
}

// Kills the effect whose id is held in work int 0x10000006, unless the next status is in 1..8.
void wnSamusCShotStatusUniqProcess::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus >= 9 || nextStatus < 1) {
        int effectId = moduleAccesser->getWorkManageModule().getInt(0x10000006);
        moduleAccesser->getEffectModule().kill(effectId, true, true);
    }
}

void wnSamusCShotStatusUniqProcess::execFixPos(soModuleAccesser* moduleAccesser) {
    int status = moduleAccesser->getStatusModule().getStatusKind();
    ecMgr* effectManager;
    u32 effectId = moduleAccesser->getWorkManageModule().getInt(0x10000006);
    int kind = moduleAccesser->getWorkManageModule().getInt(0x10000007);
    Vec3f offset;
    if (kind == 3) {
        offset = Vec3f(status == 0 ? -8.0f : 0.0f, 0.0f, 0.0f);
    } else {
        offset = Vec3f(status == 0 ? 0.0f : 0.0f, 0.0f, 0.0f);
    }
    effectManager = g_ecMgr;
    effectManager->setPos(effectId, &moduleAccesser->getModelModule().getNodeGlobalPosition(0, &offset, false, false));
    const wnSamusCShotParam* param = dynamic_cast<wnSamusCShot&>(moduleAccesser->getStageObject()).getData()->m_param;
    float charge = moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    float scale = param->unk18 + charge * (param->unk1C - param->unk18);
    scale *= moduleAccesser->getPostureModule().getScale();
    g_ecMgr->setScl(effectId, &Vec3f(scale, scale, scale));
    switch (status) {
    case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:
        moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
    }
}
