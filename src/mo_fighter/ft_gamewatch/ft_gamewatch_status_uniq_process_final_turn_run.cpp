#include <ft/ft_common_data_accesser.h>
#include <ft/gamewatch/ft_gamewatch_status_uniq_process_final_swim.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <types.h>

extern "C" {
// HYPOTHESIS: float constants from lbl_107_rodata_A0 (values not recovered; read through the symbol)
extern float lbl_107_rodata_A0[];
}

class ftGameWatchStatusUniqProcessFinalTurnRun : public ftGameWatchStatusUniqProcessFinalSwim {
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
    ftGameWatchStatusUniqProcessFinalSwim::initStatus(moduleAccesser);
}

void ftGameWatchStatusUniqProcessFinalTurnRun::execStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: float literals 1.0f, 0.0f, 0.5f stand in for unrecovered lbl_107_rodata_A0 values.
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    if (workManage.isFlag(0x22000010) == 1) {
        soValueAccesser::getConstantFloat(moduleAccesser, 0xc39, 0);
        if (!workManage.isFlag(0x22000011)) {
            moduleAccesser->getMotionModule().setRate(1.0f);
            workManage.onFlag(0x22000011);
        } else {
            float facing = workManage.getFloat(0x21000005);
            float speedX = moduleAccesser->getKineticModule()
                               .getSumSpeed(soKineticEnergy::AttributeFlag(1)).m_x;
            bool reachedLimit = facing * speedX <= 0.0f;
            if (reachedLimit) {
                moduleAccesser->getMotionModule().setRate(0.5f);
                workManage.offFlag(0x22000010);
            }
        }
    }
    ftGameWatchStatusUniqProcessFinalSwim::execStatus(moduleAccesser);
}

ftGameWatchStatusUniqProcessFinalTurnRun g_ftGameWatchStatusUniqProcessFinalTurnRun;
