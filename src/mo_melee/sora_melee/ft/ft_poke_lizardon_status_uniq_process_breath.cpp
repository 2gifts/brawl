#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftPokeLizardonStatusUniqProcessBreath : public soStatusUniqProcess {
public:
    virtual ~ftPokeLizardonStatusUniqProcessBreath() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftPokeLizardonStatusUniqProcessBreath g_ftPokeLizardonStatusUniqProcessBreath;

void ftPokeLizardonStatusUniqProcessBreath::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftPokeLizardonStatusUniqProcessBreath::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftPokeLizardonStatusUniqProcessBreath::exitStatus(soModuleAccesser* moduleAccesser, int) {
}
