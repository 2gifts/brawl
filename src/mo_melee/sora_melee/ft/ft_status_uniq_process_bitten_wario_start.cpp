#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessBittenWarioStart : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessBittenWarioStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessBittenWarioStart g_ftStatusUniqProcessBittenWarioStart;

void ftStatusUniqProcessBittenWarioStart::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessBittenWarioStart::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessBittenWarioStart::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessBittenWarioStart::execFixPos(soModuleAccesser* moduleAccesser) { }
bool ftStatusUniqProcessBittenWarioStart::checkDamage(soModuleAccesser* moduleAccesser, void*) { return false; }
