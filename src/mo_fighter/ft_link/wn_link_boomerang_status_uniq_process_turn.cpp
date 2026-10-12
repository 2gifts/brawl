#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnLinkBoomerangStatusUniqProcessTurn : public soStatusUniqProcess {
public:
    virtual ~wnLinkBoomerangStatusUniqProcessTurn() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnLinkBoomerangStatusUniqProcessTurn g_wnLinkBoomerangStatusUniqProcessTurn;

void wnLinkBoomerangStatusUniqProcessTurn::initStatus(soModuleAccesser* moduleAccesser) { }
void wnLinkBoomerangStatusUniqProcessTurn::execStatus(soModuleAccesser* moduleAccesser) { }
void wnLinkBoomerangStatusUniqProcessTurn::execFixPos(soModuleAccesser* moduleAccesser) { }
