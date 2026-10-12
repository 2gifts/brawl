#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftSheikStatusUniqProcessSpecialN : public soStatusUniqProcess {
public:
    virtual ~ftSheikStatusUniqProcessSpecialN() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftSheikStatusUniqProcessSpecialN g_ftSheikStatusUniqProcessSpecialN;

void ftSheikStatusUniqProcessSpecialN::exitStatus(soModuleAccesser* moduleAccesser, int) { }
bool ftSheikStatusUniqProcessSpecialN::checkDamage(soModuleAccesser* moduleAccesser, void*) { return false; }
