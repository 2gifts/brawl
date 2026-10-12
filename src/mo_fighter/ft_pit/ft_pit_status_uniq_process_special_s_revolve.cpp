#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/controller/so_controller_impl.h>
#include <types.h>

// Pit's Silver Bow revolve (special S): the angle accumulates from the stick and the arm node follows it.
// HYPOTHESIS: work floats 0x21000004 (node angle) and 0x21000005 (angle step); node 0x48 is the bow arm.
class ftPitStatusUniqProcessSpecialSRevolve : public soStatusUniqProcess {
public:
    virtual ~ftPitStatusUniqProcessSpecialSRevolve() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    void updateAngle(soModuleAccesser* moduleAccesser);
};

ftPitStatusUniqProcessSpecialSRevolve g_ftPitStatusUniqProcessSpecialSRevolve;

void ftPitStatusUniqProcessSpecialSRevolve::initStatus(soModuleAccesser* moduleAccesser) {
    Vec3f rotate = moduleAccesser->getMotionModule().getNodeRotate(0x48);
    moduleAccesser->getWorkManageModule().setFloat(rotate.m_x, 0x21000004);
    moduleAccesser->getWorkManageModule().setFloat(soValueAccesser::getConstantFloat(moduleAccesser, 0xfa3, 0), 0x21000005);
}

void ftPitStatusUniqProcessSpecialSRevolve::execStatus(soModuleAccesser* moduleAccesser) {
    float step = moduleAccesser->getWorkManageModule().getFloat(0x21000005);
    soControllerModule& controller = moduleAccesser->getControllerModule();
    u32 specialMask = soController::getButtonMask(soController::Pad_Button_Special);
    int trigger = controller.getTrigger();
    if ((trigger & specialMask) != 0) {
        step += soValueAccesser::getConstantFloat(moduleAccesser, 0xfa6, 0);
    }
    step -= soValueAccesser::getConstantFloat(moduleAccesser, 0xfa4, 0);
    float threshold = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa8, 0);
    if (!(step <= threshold)) {
        moduleAccesser->getStatusModule().changeStatusRequest(0x11c, moduleAccesser);
        return;
    }
    float maxStep = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa5, 0);
    if (step > maxStep) {
        step = maxStep;
    }
    moduleAccesser->getWorkManageModule().setFloat(step, 0x21000005);
    float angle = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
    float sum = angle + step;
    moduleAccesser->getWorkManageModule().setFloat(sum, 0x21000004);
    updateAngle(moduleAccesser);
}

void ftPitStatusUniqProcessSpecialSRevolve::updateAngle(soModuleAccesser* moduleAccesser) {
    soModelModule& model = moduleAccesser->getModelModule();
    float angle = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
    model.setNodeRotateX(0x48, angle);
}

void ftPitStatusUniqProcessSpecialSRevolve::execStop(soModuleAccesser* moduleAccesser) {
    updateAngle(moduleAccesser);
}
