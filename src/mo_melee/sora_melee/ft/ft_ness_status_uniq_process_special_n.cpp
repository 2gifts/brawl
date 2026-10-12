#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftNessStatusUniqProcessSpecialN : public soStatusUniqProcess {
public:
    virtual ~ftNessStatusUniqProcessSpecialN() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftNessStatusUniqProcessSpecialN g_ftNessStatusUniqProcessSpecialN;

void ftNessStatusUniqProcessSpecialN::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status == 0x10) {
        float value = moduleAccesser->getConstantFloatKirby(0xfa1);
        // HYPOTHESIS: comparison threshold is the float at lbl_27_rodata_75F8 (expected 0.0f).
        if (value > 0.0f) {
            moduleAccesser->getWorkManageModule().setFloat(value, 0x11000000);
        }
    }
}
