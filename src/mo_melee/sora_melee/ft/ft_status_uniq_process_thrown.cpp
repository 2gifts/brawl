#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// MATCH-ONLY: the -O2,s fighter flags stop the implicit constructor inlining into the static initializer;
// the Matching neighbours build at the default -O4,p.
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessThrown : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessThrown() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessThrown g_ftStatusUniqProcessThrown;

// TODO: body not decoded yet.
void ftStatusUniqProcessThrown::initStatus(soModuleAccesser*) {}
void ftStatusUniqProcessThrown::execFixPos(soModuleAccesser*) {}
void ftStatusUniqProcessThrown::exitStatus(soModuleAccesser*, int) {}
bool ftStatusUniqProcessThrown::checkDamage(soModuleAccesser*, void*) { return false; }
