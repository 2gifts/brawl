#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessBittenWarioEnd : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessBittenWarioEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessBittenWarioEnd g_ftStatusUniqProcessBittenWarioEnd;

void ftStatusUniqProcessBittenWarioEnd::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessBittenWarioEnd::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessBittenWarioEnd::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessBittenWarioEnd::execFixPos(soModuleAccesser* moduleAccesser) { }
bool ftStatusUniqProcessBittenWarioEnd::checkDamage(soModuleAccesser* moduleAccesser, void*) { return false; }
