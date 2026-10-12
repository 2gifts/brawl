#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnLuigiNegativeZoneStatusUniqProcessEnd : public soStatusUniqProcess {
public:
    virtual ~wnLuigiNegativeZoneStatusUniqProcessEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

wnLuigiNegativeZoneStatusUniqProcessEnd g_wnLuigiNegativeZoneStatusUniqProcessEnd;

void wnLuigiNegativeZoneStatusUniqProcessEnd::initStatus(soModuleAccesser* moduleAccesser) { }
