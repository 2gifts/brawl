#include <ft/ft_common_data_accesser.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: extendParam group 4 of the GameWatch data (ftData::extendParam[3]); the lower bound sits at +0x10.
struct ftGameWatchSpecialLwShootGroup {
    u8 unk0[0x10];
    float m_minPower;
};

class ftGamewatchStatusUniqProcessSpecialLwShoot : public soStatusUniqProcess {
public:
    virtual ~ftGamewatchStatusUniqProcessSpecialLwShoot() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

ftGamewatchStatusUniqProcessSpecialLwShoot g_ftGamewatchStatusUniqProcessSpecialLwShoot;

void ftGamewatchStatusUniqProcessSpecialLwShoot::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    ftGameWatchSpecialLwShootGroup* group = static_cast<ftGameWatchSpecialLwShootGroup*>(
        g_ftCommonDataAccesser.getData(Fighter_GameWatch)->extendParam[3]);
    float power = workManage.getFloat(0x11000014);
    if (power < group->m_minPower) {
        power = group->m_minPower;
    }
    workManage.setFloat(power, 0x21000006);
}

void ftGamewatchStatusUniqProcessSpecialLwShoot::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    soCollisionAttackModule& attack = moduleAccesser->getCollisionAttackModule();
    float power = moduleAccesser->getWorkManageModule().getFloat(0x21000006);
    for (int i = 0; i < 4; i++) {
        if (attack.isAttack(i, false)) {
            attack.setPower(i, (int)power, false);
        }
    }
}
