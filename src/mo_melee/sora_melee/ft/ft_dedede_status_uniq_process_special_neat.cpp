#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftDededeStatusUniqProcessSpecialNEat : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialNEat() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftDededeStatusUniqProcessSpecialNEat g_ftDededeStatusUniqProcessSpecialNEat;

void ftDededeStatusUniqProcessSpecialNEat::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialNEat::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialNEat::execStop(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialNEat::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialNEat::exitStatus(soModuleAccesser* moduleAccesser, int) {
}
