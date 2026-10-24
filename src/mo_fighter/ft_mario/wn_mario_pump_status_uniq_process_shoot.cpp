#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <so/so_module_accesser.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>

// F.L.U.D.D. nozzle aiming: interpolate the desired angle from stick Y, then
// approach it at a bounded angular speed. Hitstop keeps the stored angle.
class wnMarioPumpStatusUniqProcessShoot : public soStatusUniqProcess {
public:
    wnMarioPumpStatusUniqProcessShoot() { }
    virtual ~wnMarioPumpStatusUniqProcessShoot() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
};

wnMarioPumpStatusUniqProcessShoot g_wnMarioPumpStatusUniqProcessShoot;

void wnMarioPumpStatusUniqProcessShoot::execStatus(soModuleAccesser* moduleAccesser) {
    float stickY = moduleAccesser->getControllerModule().getStickY();
    float neutral = soValueAccesser::getConstantFloat(moduleAccesser, 4000, 0);
    if (stickY >= 0.0f) {
        stickY = neutral + stickY * (soValueAccesser::getConstantFloat(moduleAccesser, 4001, 0) - neutral);
    } else {
        float down = -soValueAccesser::getConstantFloat(moduleAccesser, 4002, 0);
        float amount = -stickY;
        stickY = neutral + amount * (down - neutral);
    }
    float angle = moduleAccesser->getWorkManageModule().getFloat(0x21000000);
    float step = soValueAccesser::getConstantFloat(moduleAccesser, 4003, 0);
    float delta = stickY - angle;
    if (delta > step) delta = step;
    else if (delta < -step) delta = -step;
    angle += delta;
    moduleAccesser->getWorkManageModule().setFloat(angle, 0x21000000);
    moduleAccesser->getModelModule().setNodeRotateX(5, -angle);
}
void wnMarioPumpStatusUniqProcessShoot::execStop(soModuleAccesser* moduleAccesser) {
    soModelModule& model = moduleAccesser->getModelModule();
    model.setNodeRotateX(5, -moduleAccesser->getWorkManageModule().getFloat(0x21000000));
}
