#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnLinkBowArrowStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnLinkBowArrowStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnLinkBowArrowStatusUniqProcess g_wnLinkBowArrowStatusUniqProcess;

void wnLinkBowArrowStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) { }
void wnLinkBowArrowStatusUniqProcess::execStatus(soModuleAccesser* moduleAccesser) { }
