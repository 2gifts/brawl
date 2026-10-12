#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnGameWatchOctopusStatusUniqProcessCommon : public soStatusUniqProcess {
public:
    virtual ~wnGameWatchOctopusStatusUniqProcessCommon() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnGameWatchOctopusStatusUniqProcessCommon g_wnGameWatchOctopusStatusUniqProcessCommon;

void wnGameWatchOctopusStatusUniqProcessCommon::execStatus(soModuleAccesser* moduleAccesser) { }
