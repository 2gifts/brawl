#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <wn/toon_link/wn_toon_link_bow_arrow.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <math.h>
#include <mt/mt_prng.h>

// The status unique process of Toon Link's arrow: status 0 = nocked on the bow, 1 = flying, 2 = stuck in the ground or a wall.
class wnToonLinkBowArrowStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnToonLinkBowArrowStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnToonLinkBowArrowStatusUniqProcess g_wnToonLinkBowArrowStatusUniqProcess;

void wnToonLinkBowArrowStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) {
    wnToonLinkBowArrow& arrow = dynamic_cast<wnToonLinkBowArrow&>(moduleAccesser->getStageObject());
    int status = moduleAccesser->getStatusModule().getStatusKind();
    if (status == 0) {
        const wnToonLinkBowArrowParam* param = arrow.getData()->m_param;
        int life = param->m_holdLife;
        moduleAccesser->getWorkManageModule().setInt(life, 0x10000003);
        moduleAccesser->getWorkManageModule().setInt(life, 0x10000004);
        arrow.setFlyPose();
    } else if (status == 1) {
        const wnToonLinkBowArrowParam* param = arrow.getData()->m_param;
        float lr = moduleAccesser->getPostureModule().getLr();
        float charge = moduleAccesser->getWorkManageModule().getFloat(0x11000001);
        float angle = moduleAccesser->getWorkManageModule().getFloat(0x11000002);
        moduleAccesser->getWorkManageModule().setInt(
            static_cast<int>(param->m_flyLifeMin + charge * (param->m_flyLifeMax - param->m_flyLifeMin)), 0x20000000);
        float speed = param->m_speedMin + charge * (param->m_speedMax - param->m_speedMin);
        Vec2f velocity;
        velocity.m_x = lr * (speed * static_cast<float>(cos(angle)));
        velocity.m_y = speed * static_cast<float>(sin(angle));
        wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
        energy.m_speed = velocity;
        Vec2f stable(3.4028235e38f, 3.4028235e38f);
        energy.m_speedTarget = stable;
        Vec2f accel(0.0f, -param->m_gravity);
        energy.m_accel = accel;
    } else if (status == 2) {
        const wnToonLinkBowArrowParam* param = arrow.getData()->m_param;
        int life = param->m_stuckLife;
        moduleAccesser->getWorkManageModule().setInt(life, 0x10000003);
        moduleAccesser->getWorkManageModule().setInt(life, 0x10000004);
        int shakeCount = randi(4) + 3;
        moduleAccesser->getWorkManageModule().setInt(shakeCount, 0x20000000);
    }
}

void wnToonLinkBowArrowStatusUniqProcess::execStatus(soModuleAccesser* moduleAccesser) { }
