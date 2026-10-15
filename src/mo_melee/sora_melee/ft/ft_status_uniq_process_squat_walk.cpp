#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_posture_view.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessSquatWalk : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessSquatWalk() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessSquatWalk g_ftStatusUniqProcessSquatWalk;

// Squat-walk stick-rate helper (first function of this unit): maps the stick position (times the facing) to a 0.5..1.0 rate using the
// dead zone constants of the current squat motion (status 0x13 / 0x14).
extern "C" float soModuleAccesser__SquatWalk_GetStickXRate(soModuleAccesser* moduleAccesser) {
    float value = moduleAccesser->getControllerModule().getStickX() * reinterpret_cast<ftStatusPostureView&>(moduleAccesser->getPostureModule()).unk2c();
    float lo = 0.0f;
    float hi = lo;
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x13:
        lo = soValueAccesser::getConstantFloat(moduleAccesser, 0xc3e, 0);
        hi = soValueAccesser::getConstantFloat(moduleAccesser, 0xc3f, 0);
        if (value < lo) {
            return 0.5f;
        }
        if (value > hi) {
            return 1.0f;
        }
        break;
    case 0x14:
        lo = soValueAccesser::getConstantFloat(moduleAccesser, 0xc40, 0);
        hi = soValueAccesser::getConstantFloat(moduleAccesser, 0xc41, 0);
        if (value > lo) {
            return 0.5f;
        }
        if (value < hi) {
            return 1.0f;
        }
        break;
    }
    float span = hi - lo;
    if ((float)__fabs(span) > 1.1920929e-07f) {
        return 0.5f + 0.5f * ((value - lo) / span);
    }
    return 1.0f;
}


void ftStatusUniqProcessSquatWalk::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessSquatWalk::execStatus(soModuleAccesser* moduleAccesser) { }
