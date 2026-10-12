#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftLinkStatusUniqProcessSpecialBow : public soStatusUniqProcess {
public:
    virtual ~ftLinkStatusUniqProcessSpecialBow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftLinkStatusUniqProcessSpecialBow g_ftLinkStatusUniqProcessSpecialBow;

void ftLinkStatusUniqProcessSpecialBow::initStatus(soModuleAccesser* moduleAccesser) { }
void ftLinkStatusUniqProcessSpecialBow::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftLinkStatusUniqProcessSpecialBow::execStatus(soModuleAccesser* moduleAccesser) { }
void ftLinkStatusUniqProcessSpecialBow::execFixPos(soModuleAccesser* moduleAccesser) { }
