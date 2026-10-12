#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftDededeStatusUniqProcessSpecialN : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialN() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftDededeStatusUniqProcessSpecialN g_ftDededeStatusUniqProcessSpecialN;

void ftDededeStatusUniqProcessSpecialN::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialN::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialN::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialN::exitStatus(soModuleAccesser* moduleAccesser, int) {
}
