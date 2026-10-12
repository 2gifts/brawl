#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftYoshiStatusUniqProcessSpecialNSwallow : public soStatusUniqProcess {
public:
    virtual ~ftYoshiStatusUniqProcessSpecialNSwallow() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftYoshiStatusUniqProcessSpecialNSwallow g_ftYoshiStatusUniqProcessSpecialNSwallow;

void ftYoshiStatusUniqProcessSpecialNSwallow::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftYoshiStatusUniqProcessSpecialNSwallow::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
bool ftYoshiStatusUniqProcessSpecialNSwallow::checkDamage(soModuleAccesser* moduleAccesser, void*) { return false; }
