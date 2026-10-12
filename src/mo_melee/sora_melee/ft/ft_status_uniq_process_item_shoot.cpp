#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// MATCH-ONLY: the -O2,s fighter flags stop the implicit constructor inlining into the static initializer;
// the Matching neighbours build at the default -O4,p.
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessItemShoot : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessItemShoot() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    void attachHoldAnim(soModuleAccesser* moduleAccesser);
    void detachHoldAnim(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessItemShoot g_ftStatusUniqProcessItemShoot;

// TODO: bodies not decoded yet.
void ftStatusUniqProcessItemShoot::initStatus(soModuleAccesser*) {}
void ftStatusUniqProcessItemShoot::execStatus(soModuleAccesser*) {}
void ftStatusUniqProcessItemShoot::execFixPosCounter(soModuleAccesser*) {}
void ftStatusUniqProcessItemShoot::execFixPos(soModuleAccesser*) {}
void ftStatusUniqProcessItemShoot::exitStatus(soModuleAccesser*, int) {}
void ftStatusUniqProcessItemShoot::attachHoldAnim(soModuleAccesser*) {}
void ftStatusUniqProcessItemShoot::detachHoldAnim(soModuleAccesser*) {}
