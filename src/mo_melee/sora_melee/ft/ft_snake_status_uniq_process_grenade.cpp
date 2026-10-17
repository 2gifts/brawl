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

// HYPOTHESIS: shared grenade parameter table and its getter (no declarations in the shared headers).
extern u8 lbl_27_bss_7B0C[];
extern "C" int fn_27_30FA2C(void* table, int kind, int id, soModuleAccesser* acc, int);

void ftSnakeStatusUniqProcessGrenade::initStatus(soModuleAccesser* acc) {
    int v;
    v = fn_27_30FA2C(lbl_27_bss_7B0C, 0, 0x122, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x20000001);
    v = fn_27_30FA2C(lbl_27_bss_7B0C, 0, 0x123, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x20000002);
    v = fn_27_30FA2C(lbl_27_bss_7B0C, 0, 0x125, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x20000003);
    v = fn_27_30FA2C(lbl_27_bss_7B0C, 0, 0x124, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x20000004);
    v = fn_27_30FA2C(lbl_27_bss_7B0C, 0, 0x126, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x20000005);
    v = fn_27_30FA2C(lbl_27_bss_7B0C, 0, 0x127, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x20000006);
    v = fn_27_30FA2C(lbl_27_bss_7B0C, 0, 0x128, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x20000007);
    v = fn_27_30FA2C(lbl_27_bss_7B0C, 0, 0x12b, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x20000008);
    v = fn_27_30FA2C(lbl_27_bss_7B0C, 0, 0x12a, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x20000009);
    v = fn_27_30FA2C(lbl_27_bss_7B0C, 0, 0x129, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x2000000a);
    v = fn_27_30FA2C(lbl_27_bss_7B0C, 0, 0x12c, acc, 0);
    acc->getWorkManageModule().setInt(v, 0x2000000b);
    int a = fn_27_30FA2C(lbl_27_bss_7B0C, 3, 0x1dc, acc, 0);
    int b = fn_27_30FA2C(lbl_27_bss_7B0C, 3, 0x1e2, acc, 0);
    ftStatusUniqProcessShoot::initShoot_Common(acc, a, b);
}
