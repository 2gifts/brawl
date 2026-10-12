#include <ft/ft_common_data_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

extern "C" {
// HYPOTHESIS: status-entry helper called with (this, moduleAccesser) at the end of initStatus (REL-local, unnamed)
void fn_107_141A0(void* self, soModuleAccesser* moduleAccesser);
// HYPOTHESIS: status-end helper called with (this, moduleAccesser) at the end of execStatus (REL-local, unnamed)
void fn_107_14328(void* self, soModuleAccesser* moduleAccesser);
// HYPOTHESIS: float constants from lbl_107_rodata_A0 (values not recovered; read through the symbol)
extern float lbl_107_rodata_A0[];
}

class ftGameWatchStatusUniqProcessFinalTurnRun : public soStatusUniqProcess {
public:
    virtual ~ftGameWatchStatusUniqProcessFinalTurnRun() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftGameWatchStatusUniqProcessFinalTurnRun::initStatus(soModuleAccesser* moduleAccesser) {
    u8* commonData = reinterpret_cast<u8*>(g_ftCommonDataAccesser.getData(static_cast<ftKind>(0x12)));
    u8* table = *reinterpret_cast<u8**>(commonData + 0xb8);
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    workManage.setFloat(moduleAccesser->getPostureModule().getLr(), 0x21000005);
    workManage.offFlag(0x22000011);
    workManage.setFloat(soValueAccesser::getConstantFloat(moduleAccesser, 0xc3b, 0), 0x21000004);
    workManage.offFlag(0x22000010);
    workManage.setInt(*reinterpret_cast<int*>(table + 0x28), 0x20000000);
    fn_107_141A0(this, moduleAccesser);
}

void ftGameWatchStatusUniqProcessFinalTurnRun::execStatus(soModuleAccesser* moduleAccesser) {}

ftGameWatchStatusUniqProcessFinalTurnRun g_ftGameWatchStatusUniqProcessFinalTurnRun;
