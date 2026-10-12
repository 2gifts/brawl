#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessBittenWario : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessBittenWario() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessBittenWario g_ftStatusUniqProcessBittenWario;

void ftStatusUniqProcessBittenWario::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessBittenWario::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessBittenWario::execStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessBittenWario::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessBittenWario::execFixPos(soModuleAccesser* moduleAccesser) { }
bool ftStatusUniqProcessBittenWario::checkDamage(soModuleAccesser* moduleAccesser, void*) { return false; }
