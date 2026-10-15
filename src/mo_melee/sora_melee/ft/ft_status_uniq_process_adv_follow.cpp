#include <so/effect/so_effect_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftStatusUniqProcessAdvFollow : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessAdvFollow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};

ftStatusUniqProcessAdvFollow g_ftStatusUniqProcessAdvFollow;

// HYPOTHESIS: the follower status (adventure mode) starts its looping effect 0x1030016 at the fighter and a status sound
// (0x2287); the handles are kept in work ints 0x20000000/1.
void ftStatusUniqProcessAdvFollow::initStatus(soModuleAccesser* moduleAccesser) {
    float scale = moduleAccesser->getPostureModule().getScale();
    Vec3f pos = moduleAccesser->getPostureModule().getPos();
    int effectHandle = moduleAccesser->getEffectModule().req(static_cast<EfID>(0x1030016), &Vec3f(pos.m_x, pos.m_y, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), scale, 0, -1);
    moduleAccesser->getWorkManageModule().setInt(effectHandle, 0x20000000);
    int soundHandle = moduleAccesser->getSoundModule().playStatusSE(static_cast<SndID>(0x2287), false, 0);
    moduleAccesser->getWorkManageModule().setInt(soundHandle, 0x20000001);
}

void ftStatusUniqProcessAdvFollow::execStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessAdvFollow::execFixPos(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessAdvFollow::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) { }
