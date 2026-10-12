#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftPokeZenigameStatusUniqProcessSpecialN : public soStatusUniqProcess {
public:
    virtual ~ftPokeZenigameStatusUniqProcessSpecialN() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftPokeZenigameStatusUniqProcessSpecialN g_ftPokeZenigameStatusUniqProcessSpecialN;

void ftPokeZenigameStatusUniqProcessSpecialN::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftPokeZenigameStatusUniqProcessSpecialN::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftPokeZenigameStatusUniqProcessSpecialN::exitStatus(soModuleAccesser* moduleAccesser, int) {
}
