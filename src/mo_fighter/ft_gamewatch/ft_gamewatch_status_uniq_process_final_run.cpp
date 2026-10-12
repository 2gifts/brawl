#include <ft/ft_common_data_accesser.h>
#include <ft/ft_util.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <math.h>
#include <types.h>

// HYPOTHESIS: run parameter block pointer stored at ftData+0xB8; floats at +0x14, +0x18 and +0x1C.
struct ftGameWatchRunParam {
    u8 unk0[0x14];
    float m_14;
    float m_18;
    float m_1C;
};

// HYPOTHESIS: base status class of the final statuses (its methods are defined in the FinalSwim unit).
class ftGameWatchStatusUniqProcessFinalSwim : public soStatusUniqProcess {
public:
    virtual ~ftGameWatchStatusUniqProcessFinalSwim() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftGameWatchStatusUniqProcessFinalRun : public ftGameWatchStatusUniqProcessFinalSwim {
public:
    virtual ~ftGameWatchStatusUniqProcessFinalRun() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    void setMotionRate(soModuleAccesser* moduleAccesser);
};

ftGameWatchStatusUniqProcessFinalRun g_ftGameWatchStatusUniqProcessFinalRun;

// HYPOTHESIS: constants from lbl_107_rodata_88 and lbl_107_rodata_8C (values not verified).
static const float s_runSpeedMax = 1.0f;
static const float s_runThreshold = 0.5f;

// Shared sora_melee helpers, called by their fn_27 names (signatures inferred from the call sites).
extern "C" void fn_27_1652A8(soModuleAccesser* moduleAccesser, int, float, u32, float, u32, int);
extern "C" void fn_27_165458(soModuleAccesser* moduleAccesser, u32, float, float, float, float, float, float);

static ftGameWatchRunParam* getRunParam() {
    ftData* data = g_ftCommonDataAccesser.getData(Fighter_GameWatch);
    return *reinterpret_cast<ftGameWatchRunParam**>(reinterpret_cast<u8*>(data) + 0xB8);
}

void ftGameWatchStatusUniqProcessFinalRun::initStatus(soModuleAccesser* moduleAccesser) {
    ftGameWatchRunParam* param = getRunParam();
    fn_27_1652A8(moduleAccesser, 0x1f7, param->m_1C, 0x21000005, s_runSpeedMax, 0x21000007, 0);
    setMotionRate(moduleAccesser);
    ftGameWatchStatusUniqProcessFinalSwim::initStatus(moduleAccesser);
}

void ftGameWatchStatusUniqProcessFinalRun::execStatus(soModuleAccesser* moduleAccesser) {
    ftGameWatchRunParam* param = getRunParam();
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    float constant = soValueAccesser::getConstantFloat(moduleAccesser, 0xc37, 0);
    float value = workManage.getFloat(0x21000005);
    fn_27_165458(moduleAccesser, 0x21000005, value, param->m_1C, param->m_14, param->m_18, constant, s_runSpeedMax);
    setMotionRate(moduleAccesser);
    if (workManage.getFloat(0x21000004) > s_runThreshold) {
        workManage.subFloat(s_runSpeedMax, 0x21000004);
    }
    ftGameWatchStatusUniqProcessFinalSwim::execStatus(moduleAccesser);
}

void ftGameWatchStatusUniqProcessFinalRun::setMotionRate(soModuleAccesser* moduleAccesser) {
    float speed = fabs(moduleAccesser->getWorkManageModule().getFloat(0x21000005));
    float rate = moduleAccesser->getWorkManageModule().getFloat(0x21000007);
    moduleAccesser->getMotionModule().setRate(speed * rate);
}
