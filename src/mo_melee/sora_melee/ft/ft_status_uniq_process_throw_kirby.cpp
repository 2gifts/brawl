#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// MATCH-ONLY: the -O2,s fighter flags stop the implicit constructor inlining into the static initializer;
// the Matching neighbours build at the default -O4,p.
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessThrowKirby : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessThrowKirby() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessThrowKirby g_ftStatusUniqProcessThrowKirby;

// TODO: body not decoded yet.
void ftStatusUniqProcessThrowKirby::initStatus(soModuleAccesser*) {}
void ftStatusUniqProcessThrowKirby::execFixPos(soModuleAccesser*) {}
void ftStatusUniqProcessThrowKirby::exitStatus(soModuleAccesser*, int) {}
bool ftStatusUniqProcessThrowKirby::checkDamage(soModuleAccesser*, void*) { return false; }
