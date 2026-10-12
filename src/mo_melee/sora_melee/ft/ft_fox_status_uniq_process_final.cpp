#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftFoxStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftFoxStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftFoxStatusUniqProcessFinal g_ftFoxStatusUniqProcessFinal;

void ftFoxStatusUniqProcessFinal::initStatus(soModuleAccesser* moduleAccesser) { }
void ftFoxStatusUniqProcessFinal::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftFoxStatusUniqProcessFinal::execFixPos(soModuleAccesser* moduleAccesser) { }
