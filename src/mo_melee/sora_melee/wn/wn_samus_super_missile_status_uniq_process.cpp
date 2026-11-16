#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <so/so_module_accesser.h>
#include <wn/samus/wn_samus_supermissile.h>
#include <wn/wn_kinetic_energy_normal.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSamusSuperMissileStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnSamusSuperMissileStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnSamusSuperMissileStatusUniqProcess g_wnSamusSuperMissileStatusUniqProcess;

// The super missile starts at its parameter speed in the facing direction; its stable speed (unk14) points the same way.
void wnSamusSuperMissileStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0: {
        const wnSamusSuperMissileParam* param = dynamic_cast<wnSamusSuperMissile&>(moduleAccesser->getStageObject()).getData()->m_param;
        float lr = moduleAccesser->getPostureModule().getLr();
        int life = param->unk4;
        wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
        energy.resetEnergy(0, &Vec2f(param->unkC * lr, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        energy.m_speedTarget = Vec2f(param->unk14 * lr, 0.0f);
        energy.enable();
        moduleAccesser->getCollisionAttackModule().setSpeed(&energy.getSpeed());
        moduleAccesser->getWorkManageModule().setInt(0, 0x10000006);
        moduleAccesser->getWorkManageModule().setInt(life, 0x10000005);
        break;
    }
    case 1:
        break;
    }
}
void wnSamusSuperMissileStatusUniqProcess::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    int status = moduleAccesser->getStatusModule().getStatusKind();
    const wnSamusSuperMissileParam* param = dynamic_cast<wnSamusSuperMissile&>(moduleAccesser->getStageObject()).getData()->m_param;
    switch (status) {
    case 0: {
        int count = moduleAccesser->getWorkManageModule().getInt(0x10000006);
        moduleAccesser->getPostureModule().getLr();
        if (count <= param->unk8) {
            if (count == param->unk8) {
                wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
                Vec2f speed = energy.getSpeed();
                if (speed.length() > 0.00001f) {
                    speed.normalize();
                    energy.m_accel = speed * param->unk10;
                }
            }
            moduleAccesser->getWorkManageModule().incInt(0x10000006);
        }
        break;
    }
    }
}
