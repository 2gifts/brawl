#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_util.h>
#include <so/camera/so_camera_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessWalk : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessWalk() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool onChangeLr(soModuleAccesser* moduleAccesser, float, float);
};

ftStatusUniqProcessWalk g_ftStatusUniqProcessWalk;

// Walk helpers (ftStatusUniqProcessWalkUtil) live in another unit; declared by their module-map names.
// HYPOTHESIS: argument meanings and the work ids 0x21000004 / 0x21000005 (speed and walk speed work) are not verified.
extern "C" {
void fn_27_1652A8(soModuleAccesser* moduleAccesser, int kind, int workA, int workB, int flag, float a, float b);
void fn_27_164BF0(soModuleAccesser* moduleAccesser, const int* table, int workA, int workB, float speed);
void fn_27_165458(soModuleAccesser* moduleAccesser, int work, float f1, float f2, float f3, float f4, float f5, float f6);
void fn_27_164E48(soModuleAccesser* moduleAccesser, const int* table, int workA, int workB, float speed);
}

// Walk motion kind table passed to the shared walk helpers (module rodata lbl_27_rodata_2018).
static const int s_walkTable[4] = { 12, 11, 10, 0 };

void ftStatusUniqProcessWalk::initStatus(soModuleAccesser* moduleAccesser) {
    float speedMul = ftUtil::getWalkSpeedMul(moduleAccesser);
    float constSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xbbb, 0);
    fn_27_1652A8(moduleAccesser, 0xc, 0x21000004, 0x21000005, 0, constSpeed, speedMul);
    fn_27_164BF0(moduleAccesser, s_walkTable, 0x21000004, 0x21000005, speedMul);
    moduleAccesser->getCameraModule().setRun(0);
}

void ftStatusUniqProcessWalk::execStatus(soModuleAccesser* moduleAccesser) {
    float speedMul = ftUtil::getWalkSpeedMul(moduleAccesser);
    float c2e = soValueAccesser::getConstantFloat(moduleAccesser, 0xc2e, 0);
    float cba = soValueAccesser::getConstantFloat(moduleAccesser, 0xbba, 0);
    float cb9 = soValueAccesser::getConstantFloat(moduleAccesser, 0xbb9, 0);
    float cbb = soValueAccesser::getConstantFloat(moduleAccesser, 0xbbb, 0);
    float current = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
    fn_27_165458(moduleAccesser, 0x21000004, current, cbb, cb9, cba, c2e, speedMul);
    fn_27_164E48(moduleAccesser, s_walkTable, 0x21000004, 0x21000005, speedMul);
}

bool ftStatusUniqProcessWalk::onChangeLr(soModuleAccesser* moduleAccesser, float, float) {
    float speed = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
    moduleAccesser->getWorkManageModule().setFloat(-speed, 0x21000004);
    return false;
}

void ftStatusUniqProcessWalk::exitStatus(soModuleAccesser* moduleAccesser, int) {
    moduleAccesser->getCameraModule().exitRun(0);
}
