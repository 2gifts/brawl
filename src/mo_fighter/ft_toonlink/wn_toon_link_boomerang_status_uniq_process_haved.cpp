#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnToonLinkBoomerangStatusUniqProcessHaved : public soStatusUniqProcess {
public:
    virtual ~wnToonLinkBoomerangStatusUniqProcessHaved() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

wnToonLinkBoomerangStatusUniqProcessHaved g_wnToonLinkBoomerangStatusUniqProcessHaved;

void wnToonLinkBoomerangStatusUniqProcessHaved::initStatus(soModuleAccesser* moduleAccesser) { }
