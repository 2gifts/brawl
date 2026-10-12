#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftFoxStatusUniqProcessIllusion : public soStatusUniqProcess {
public:
    virtual ~ftFoxStatusUniqProcessIllusion() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftFoxStatusUniqProcessIllusion g_ftFoxStatusUniqProcessIllusion;

void ftFoxStatusUniqProcessIllusion::initStatus(soModuleAccesser* moduleAccesser) { }
void ftFoxStatusUniqProcessIllusion::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftFoxStatusUniqProcessIllusion::execStatus(soModuleAccesser* moduleAccesser) { }
