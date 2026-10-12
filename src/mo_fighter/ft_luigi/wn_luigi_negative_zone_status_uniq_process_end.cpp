#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

class wnLuigiNegativeZoneStatusUniqProcessEnd : public soStatusUniqProcess {
public:
    virtual ~wnLuigiNegativeZoneStatusUniqProcessEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

wnLuigiNegativeZoneStatusUniqProcessEnd g_wnLuigiNegativeZoneStatusUniqProcessEnd;

void wnLuigiNegativeZoneStatusUniqProcessEnd::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: param ids 0x5DC1 (frames, int) and 0xFA0 (float) are unnamed; the int is converted to float.
    float frames = (float)soValueAccesser::getConstantInt(moduleAccesser, 0x5dc1, 0);
    float rate = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa0, 0) / frames;
    moduleAccesser->getWorkManageModule().setFloat(rate, 0x21000000);
}
