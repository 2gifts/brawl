#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <ft/ft_status_uniq_process_shoot.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftSnakeStatusUniqProcessGrenade : public soStatusUniqProcess {
public:
    virtual ~ftSnakeStatusUniqProcessGrenade() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

ftSnakeStatusUniqProcessGrenade g_ftSnakeStatusUniqProcessGrenade;

// Translate status and motion IDs for Kirby's copied grenade ability.

void ftSnakeStatusUniqProcessGrenade::initStatus(soModuleAccesser* acc) {
    int v;
    v = g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(0, 0x122, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x20000001);
    v = g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(0, 0x123, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x20000002);
    v = g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(0, 0x125, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x20000003);
    v = g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(0, 0x124, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x20000004);
    v = g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(0, 0x126, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x20000005);
    v = g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(0, 0x127, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x20000006);
    v = g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(0, 0x128, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x20000007);
    v = g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(0, 0x12b, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x20000008);
    v = g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(0, 0x12a, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x20000009);
    v = g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(0, 0x129, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x2000000a);
    v = g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(0, 0x12c, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x2000000b);
    int a = g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(3, 0x1dc, acc, 0);
    int b = g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(3, 0x1e2, acc, 0);
    ftStatusUniqProcessShoot::initShoot_Common(acc, a, b);
}
