#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftZeldaStatusUniqProcessSpecialN : public soStatusUniqProcess {
public:
    virtual ~ftZeldaStatusUniqProcessSpecialN() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftZeldaStatusUniqProcessSpecialN g_ftZeldaStatusUniqProcessSpecialN;

// Initial shield-reflector setup (fn_27_33946C in the target) is not reconstructed yet.
void ftZeldaStatusUniqProcessSpecialN::initStatus(soModuleAccesser* moduleAccesser) { }

void ftZeldaStatusUniqProcessSpecialN::exitStatus(soModuleAccesser* moduleAccesser, int) {
    moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
}

// Work flags 0x22000011 / 0x22000012 request the reflector status change (and clear themselves).
void ftZeldaStatusUniqProcessSpecialN::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    int target = 0;
    switch (moduleAccesser->getStageObject().soGetSubKind()) {
    case 0xd:
        target = 0;
        break;
    case 0x5:
        target = 0;
        break;
    }

    if (workManage.isFlag(0x22000011)) {
        moduleAccesser->getCollisionReflectorModule().setStatus(target, 1, 2);
        workManage.offFlag(0x22000011);
    }
    if (workManage.isFlag(0x22000012)) {
        moduleAccesser->getCollisionReflectorModule().setStatus(target, 0, 2);
        workManage.offFlag(0x22000012);
    }
}
