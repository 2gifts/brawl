#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftSheikStatusUniqProcessSpecialNShoot : public soStatusUniqProcess {
public:
    virtual ~ftSheikStatusUniqProcessSpecialNShoot() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftSheikStatusUniqProcessSpecialNShoot g_ftSheikStatusUniqProcessSpecialNShoot;

void ftSheikStatusUniqProcessSpecialNShoot::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftSheikStatusUniqProcessSpecialNShoot::execStatus(soModuleAccesser* moduleAccesser) { }
void ftSheikStatusUniqProcessSpecialNShoot::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
bool ftSheikStatusUniqProcessSpecialNShoot::checkDamage(soModuleAccesser* moduleAccesser, void*) { return false; }
