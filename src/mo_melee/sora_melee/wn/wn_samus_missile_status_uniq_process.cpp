#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_manager.h>
#include <em/em_manager.h>
#include <em/enemy.h>
#include <ft/ft_external_value_accesser.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/so_external_value_accesser.h>
#include <st/st_enemy_id_manager.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <wn/samus/wn_samus_missile.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSamusMissileStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnSamusMissileStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnSamusMissileStatusUniqProcess g_wnSamusMissileStatusUniqProcess;

// The missile starts flying at its parameter speed in the facing direction.
void wnSamusMissileStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0: {
        const wnSamusMissileParam* param = dynamic_cast<wnSamusMissile&>(moduleAccesser->getStageObject()).getData()->m_param;
        float lr = moduleAccesser->getPostureModule().getLr();
        int life = param->unk4;
        wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
        energy.resetEnergy(0, &Vec2f(param->unkC * lr, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
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
// Homing: while the homing life (work int 0x10000006) lasts, the flight angle (work float 0x11000001) turns towards the
// nearest fighter or enemy; afterwards the missile speeds up once it is fast enough.
void wnSamusMissileStatusUniqProcess::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    int status = moduleAccesser->getStatusModule().getStatusKind();
    const wnSamusMissileParam* param = dynamic_cast<wnSamusMissile&>(moduleAccesser->getStageObject()).getData()->m_param;
    switch (status) {
    case 0:
        if (moduleAccesser->getWorkManageModule().getInt(0x10000006) <= param->unk8) {
            float angle = moduleAccesser->getWorkManageModule().getFloat(0x11000001);
            float lr = moduleAccesser->getPostureModule().getLr();
            Vec2f targetPos;
            Vec2f direction;
            direction = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
            direction.normalize();
            ftManager* fighters = g_ftManager;
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            Fighter* fighter = fighters->searchNearFighter(&pos, lr, -1.0f, soExternalValueAccesser::getTeamNo(moduleAccesser->m_stageObject), false);
            if (fighter != NULL) {
                Vec3f hip = ftExternalValueAccesser::getHipPos(fighter);
                targetPos.m_x = hip.m_x;
                targetPos.m_y = hip.m_y;
            } else {
                int index = -1;
                if (g_stEnemyIdManager != NULL) {
                    index = g_stEnemyIdManager->searchNearEnemy(&pos, lr, -1.0f);
                }
                if (index >= 0) {
                    u32 enemyId = g_stEnemyIdManager->getEnemyId(index);
                    Vec3f center = emManager::getInstance()->getEnemyRefFromId(enemyId)->getCenterPos();
                    targetPos.m_x = center.m_x;
                    targetPos.m_y = center.m_y;
                } else {
                    targetPos.m_x = pos.m_x + direction.m_x;
                    targetPos.m_y = pos.m_y + direction.m_y;
                }
            }
            Vec3f ownPos = moduleAccesser->getPostureModule().getPos();
            Vec2f toTarget;
            toTarget.m_x = targetPos.m_x - ownPos.m_x;
            toTarget.m_y = targetPos.m_y - ownPos.m_y;
            toTarget.normalize();
            if (!(direction.angle(&toTarget) < param->unk20)) {
                Vec2f difference = toTarget - direction;
                if (difference.m_y > 0.001f) {
                    angle -= param->unk18 * lr;
                } else if (difference.m_y < 0.001f) {
                    angle += param->unk18 * lr;
                }
                if (lr > 0.0f) {
                    if (fabs(angle) > param->unk1C) {
                        angle = angle > 0.0f ? param->unk1C : -param->unk1C;
                    }
                } else {
                    float fromBack = angle - M_PI;
                    double absBack = fabs(fromBack);
                    float limit = param->unk1C;
                    if (absBack > limit) {
                        angle = M_PI + (fromBack > 0.0f ? limit : -limit);
                    }
                }
                moduleAccesser->getWorkManageModule().setFloat(angle, 0x11000001);
            }
            Vec2f speed(param->unkC, 0.0f);
            speed.rot(-angle, &speed);
            dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0)).m_speed = speed;
            moduleAccesser->getWorkManageModule().incInt(0x10000006);
        } else {
            Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
            if (speed.length() > param->unk14) {
                speed *= param->unk10;
                dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0)).m_speed = speed;
            }
        }
        break;
    }
}
