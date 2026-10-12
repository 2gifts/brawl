#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnPopoRubberStatusUniqProcessCommon : public soStatusUniqProcess {
public:
    virtual ~wnPopoRubberStatusUniqProcessCommon() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnPopoRubberStatusUniqProcessCommon g_wnPopoRubberStatusUniqProcessCommon;

void wnPopoRubberStatusUniqProcessCommon::initStatus(soModuleAccesser* moduleAccesser) { }
void wnPopoRubberStatusUniqProcessCommon::execFixPos(soModuleAccesser* moduleAccesser) { }
