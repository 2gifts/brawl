#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnPitIkarusStatusUniqProcessEntry : public soStatusUniqProcess {
public:
    virtual ~wnPitIkarusStatusUniqProcessEntry() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

wnPitIkarusStatusUniqProcessEntry g_wnPitIkarusStatusUniqProcessEntry;

void wnPitIkarusStatusUniqProcessEntry::initStatus(soModuleAccesser* moduleAccesser) { }
