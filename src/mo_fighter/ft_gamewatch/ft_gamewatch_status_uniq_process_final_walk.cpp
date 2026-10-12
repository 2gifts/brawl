#include <ft/ft_common_data_accesser.h>
#include <ft/ft_util.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: walk parameter block pointer stored at ftData+0xB8 (past the declared ftData fields); floats at +0x0/+0x4/+0x8.
struct ftGameWatchWalkParam {
    float m_0;
    float m_4;
    float m_8;
};

// HYPOTHESIS: base status class of the walk statuses (its layout and virtuals are defined in the FinalSwim unit).
class ftGameWatchStatusUniqProcessFinalSwim : public soStatusUniqProcess {
public:
    virtual ~ftGameWatchStatusUniqProcessFinalSwim() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftGameWatchStatusUniqProcessFinalWalk : public ftGameWatchStatusUniqProcessFinalSwim {
public:
    virtual ~ftGameWatchStatusUniqProcessFinalWalk() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftGameWatchStatusUniqProcessFinalWalk g_ftGameWatchStatusUniqProcessFinalWalk;

// Motion kind table passed to the shared walk helpers (module rodata lbl_107_rodata_78).
static const int s_walkTable[4] = { 500, 499, 498, 0 };

// Shared sora_melee helpers, called by their fn_27 names (signatures inferred from the call sites).
extern "C" void fn_27_1652A8(soModuleAccesser* moduleAccesser, int, float, u32, float, u32, int);
extern "C" void fn_27_164BF0(soModuleAccesser* moduleAccesser, const int* table, u32, float, u32);
extern "C" void fn_27_165458(soModuleAccesser* moduleAccesser, u32, float, float, float, float, float, float);
extern "C" void fn_27_164E48(soModuleAccesser* moduleAccesser, const int* table, u32, float, u32);

static ftGameWatchWalkParam* getWalkParam() {
    ftData* data = g_ftCommonDataAccesser.getData(Fighter_GameWatch);
    return *reinterpret_cast<ftGameWatchWalkParam**>(reinterpret_cast<u8*>(data) + 0xB8);
}

void ftGameWatchStatusUniqProcessFinalWalk::initStatus(soModuleAccesser* moduleAccesser) {
    ftGameWatchWalkParam* param = getWalkParam();
    float speedMul = ftUtil::getWalkSpeedMul(moduleAccesser);
    float rate = param->m_8;
    fn_27_1652A8(moduleAccesser, 0x1f4, rate, 0x21000004, speedMul, 0x21000005, 0);
    fn_27_164BF0(moduleAccesser, s_walkTable, 0x21000004, speedMul, 0x21000005);
    ftGameWatchStatusUniqProcessFinalSwim::initStatus(moduleAccesser);
}

void ftGameWatchStatusUniqProcessFinalWalk::execStatus(soModuleAccesser* moduleAccesser) {
    ftGameWatchWalkParam* param = getWalkParam();
    float speedMul = ftUtil::getWalkSpeedMul(moduleAccesser);
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    float constant = soValueAccesser::getConstantFloat(moduleAccesser, 0xc2e, 0);
    float value = workManage.getFloat(0x21000004);
    float fixed = speedMul;
    fn_27_165458(moduleAccesser, 0x21000004, value, param->m_8, param->m_0, param->m_4, constant, fixed);
    fn_27_164E48(moduleAccesser, s_walkTable, 0x21000004, speedMul, 0x21000005);
    ftGameWatchStatusUniqProcessFinalSwim::execStatus(moduleAccesser);
}
