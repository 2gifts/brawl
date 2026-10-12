#include <math.h>
#include <so/motion/so_motion_change_param.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftPokeTrainerStatusUniqProcessTurn : public soStatusUniqProcess {
public:
    virtual ~ftPokeTrainerStatusUniqProcessTurn() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftPokeTrainerStatusUniqProcessTurn g_ftPokeTrainerStatusUniqProcessTurn;

// Constants from module rodata (lbl_27_rodata_7C38).
static const float kTurnTable[3] = { 1.0f, -1.0f, 0.0f };

void ftPokeTrainerStatusUniqProcessTurn::initStatus(soModuleAccesser* moduleAccesser) {
    const float* table = kTurnTable;
    float start = moduleAccesser->getWorkManageModule().getFloat(0x11000018);
    float angle = moduleAccesser->getWorkManageModule().getFloat(0x11000016);
    float maxSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa2, 0);
    float limit = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa3, 0);
    float clamped = (angle >= -limit) ? angle : -limit;
    clamped = (clamped >= limit) ? limit : clamped;
    float delta = clamped - start;
    float dir = (delta > table[2]) ? table[0] : table[1];
    if (fabs(delta) > maxSpeed) {
        delta = maxSpeed * dir;
    }
    moduleAccesser->getWorkManageModule().setFloat(start + delta, 0x21000005);

    soMotionChangeParam param;
    param.m_kind = (dir >= table[2]) ? 0x1d3 : 0x1d2;
    param.m_frame = table[2];
    param.m_rate = table[0];
    param._12 = 0;
    param._13 = 0;
    param._14 = 0;
    param._15 = 0;
    moduleAccesser->getMotionModule().changeMotionRequest(&param);

    float endFrame = moduleAccesser->getMotionModule().getEndFrame();
    moduleAccesser->getWorkManageModule().setFloat(delta / (endFrame - table[0]), 0x21000004);
}

// HYPOTHESIS: threshold from lbl_27_rodata_7C44 (+0); value not verified yet.
static const float kTurnExecThreshold[1] = { 0.1f };

void ftPokeTrainerStatusUniqProcessTurn::execStatus(soModuleAccesser* moduleAccesser) {
    const float* threshold = kTurnExecThreshold;
    float start = moduleAccesser->getWorkManageModule().getFloat(0x11000018);
    float target = moduleAccesser->getWorkManageModule().getFloat(0x21000005);
    float sum = start + moduleAccesser->getWorkManageModule().getFloat(0x21000004);
    if (fabs(target - sum) >= threshold[0]) {
        float limit = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa3, 0);
        float clamped = (sum >= -limit) ? sum : -limit;
        target = (clamped - limit >= 0.0f) ? limit : clamped;
    }
    moduleAccesser->getWorkManageModule().setFloat(target, 0x11000018);
}

void ftPokeTrainerStatusUniqProcessTurn::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftPokeTrainerStatusUniqProcessTurn::exitStatus(soModuleAccesser* moduleAccesser, int) {
}
