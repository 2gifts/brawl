#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/collision/so_collision_shield_module_impl.h>
#include <types.h>

// HYPOTHESIS: float constants from lbl_101_rodata_90 (x and z of the start offset, y of the offset, and scale)
extern float lbl_101_rodata_90[];

// Ness's PK Thunder hold (special lw hold): spawns the hold effect and the looping sound on start.
// HYPOTHESIS: work int 0x20000003 holds an effect handle and 0x20000002 a sound handle; both are cleared on exit.
class ftNessStatusUniqProcessSpecialLwHold : public soStatusUniqProcess {
public:
    virtual ~ftNessStatusUniqProcessSpecialLwHold() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

ftNessStatusUniqProcessSpecialLwHold g_ftNessStatusUniqProcessSpecialLwHold;

void ftNessStatusUniqProcessSpecialLwHold::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: the absorber status values are (0, 1, 0) for the hold
    moduleAccesser->getCollisionAbsorberModule().setStatus(0, 1, 0);
    if (moduleAccesser->getWorkManageModule().getInt(0x20000003) < 0) {
        Vec3f rotate(lbl_101_rodata_90[0], lbl_101_rodata_90[0], lbl_101_rodata_90[0]);
        Vec3f position(lbl_101_rodata_90[0], lbl_101_rodata_90[1], lbl_101_rodata_90[0]);
        int effectId = moduleAccesser->getEffectModule().req(static_cast<EfID>(0xb000c), 4, &position, &rotate,
                                                            lbl_101_rodata_90[2], 0, 0, false, 0xffffffff);
        moduleAccesser->getWorkManageModule().setInt(effectId, 0x20000003);
    }
    if (moduleAccesser->getWorkManageModule().getInt(0x20000002) < 0) {
        int soundHandle = moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0xe7f), true, false, 0);
        moduleAccesser->getWorkManageModule().setInt(soundHandle, 0x20000002);
    }
}

void ftNessStatusUniqProcessSpecialLwHold::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x11e && nextStatus != 0x11f) {
        int effectId = moduleAccesser->getWorkManageModule().getInt(0x20000003);
        if (effectId >= 0) {
            moduleAccesser->getEffectModule().kill(effectId, true, true);
        }
        moduleAccesser->getWorkManageModule().setInt(-1, 0x20000003);
        int soundHandle = moduleAccesser->getWorkManageModule().getInt(0x20000002);
        if (soundHandle >= 0) {
            moduleAccesser->getSoundModule().stopSEHandle(soundHandle, 0);
        }
        moduleAccesser->getWorkManageModule().setInt(-1, 0x20000002);
    }
}
