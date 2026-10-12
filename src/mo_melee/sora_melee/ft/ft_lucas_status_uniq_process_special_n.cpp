#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftLucasStatusUniqProcessSpecialN : public soStatusUniqProcess {
public:
    virtual ~ftLucasStatusUniqProcessSpecialN() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

ftLucasStatusUniqProcessSpecialN g_ftLucasStatusUniqProcessSpecialN;

void ftLucasStatusUniqProcessSpecialN::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x10) {
        return;
    }
    // HYPOTHESIS: constant threshold from lbl_27_rodata_7AA8 (value unverified, assumed 0.0f).
    float value = moduleAccesser->getConstantFloatKirby(0xfa1);
    if (value > 0.0f) {
        moduleAccesser->getWorkManageModule().setFloat(value, 0x11000000);
    }
}
