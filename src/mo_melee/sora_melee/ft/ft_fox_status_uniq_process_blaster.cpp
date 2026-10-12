#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftFoxStatusUniqProcessBlaster : public soStatusUniqProcess {
public:
    virtual ~ftFoxStatusUniqProcessBlaster() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftFoxStatusUniqProcessBlaster g_ftFoxStatusUniqProcessBlaster;

void ftFoxStatusUniqProcessBlaster::initStatus(soModuleAccesser* moduleAccesser) { }
void ftFoxStatusUniqProcessBlaster::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftFoxStatusUniqProcessBlaster::execStatus(soModuleAccesser* moduleAccesser) { }
