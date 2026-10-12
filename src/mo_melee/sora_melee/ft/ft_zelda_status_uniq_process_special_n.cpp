#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftZeldaStatusUniqProcessSpecialN : public soStatusUniqProcess {
public:
    virtual ~ftZeldaStatusUniqProcessSpecialN() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftZeldaStatusUniqProcessSpecialN g_ftZeldaStatusUniqProcessSpecialN;

void ftZeldaStatusUniqProcessSpecialN::initStatus(soModuleAccesser* moduleAccesser) { }
void ftZeldaStatusUniqProcessSpecialN::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftZeldaStatusUniqProcessSpecialN::execStatus(soModuleAccesser* moduleAccesser) { }
