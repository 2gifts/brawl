#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnToonLinkBoomerangStatusUniqProcessTurn : public soStatusUniqProcess {
public:
    virtual ~wnToonLinkBoomerangStatusUniqProcessTurn() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnToonLinkBoomerangStatusUniqProcessTurn g_wnToonLinkBoomerangStatusUniqProcessTurn;

void wnToonLinkBoomerangStatusUniqProcessTurn::initStatus(soModuleAccesser* moduleAccesser) { }
void wnToonLinkBoomerangStatusUniqProcessTurn::execStatus(soModuleAccesser* moduleAccesser) { }
void wnToonLinkBoomerangStatusUniqProcessTurn::execFixPos(soModuleAccesser* moduleAccesser) { }
