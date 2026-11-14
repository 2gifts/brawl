#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftPikachuStatusUniqProcessSpecialThunderJolt : public soStatusUniqProcess {
public:
    virtual ~ftPikachuStatusUniqProcessSpecialThunderJolt() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftPikachuStatusUniqProcessSpecialThunderJolt g_ftPikachuStatusUniqProcessSpecialThunderJolt;

// Leaving the jolt in the air stores the landing-lag constant.
void ftPikachuStatusUniqProcessSpecialThunderJolt::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        int landingLag = moduleAccesser->getConstantIntKirby(0x5dc0);
        moduleAccesser->getWorkManageModule().setFloat(landingLag, 0x11000000);
    }
}
