#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Local view of the visibility module; its vtable has two leading words before the first declared slot.
class ftSheikVisibilityModuleView {
public:
    virtual void activate();
    virtual void setData(void*);
    virtual void set(u32, int);
    virtual void setStatusDefault(u32, u8, bool);
    virtual void setDefaultAll();
    virtual void update();
    virtual void hideAll();
    virtual void setWhole(u8);
};

class ftSheikStatusUniqProcessSpecialHiMove : public soStatusUniqProcess {
public:
    virtual ~ftSheikStatusUniqProcessSpecialHiMove() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

// HYPOTHESIS: not reconstructed yet (direction and kinetic setup with vector helpers); left as a stub.
void ftSheikStatusUniqProcessSpecialHiMove::initStatus(soModuleAccesser*) {
}

// HYPOTHESIS: not reconstructed yet (vector angle and length helpers); left as a stub.
void ftSheikStatusUniqProcessSpecialHiMove::execFixPosCounter(soModuleAccesser*) {
}

void ftSheikStatusUniqProcessSpecialHiMove::execFixPos(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soStatusModule& status = moduleAccesser->getStatusModule();
    if (workManage.isFlag(0x22000012)) {
        workManage.offFlag(0x22000012);
        status.changeStatusRequest(0x11d, moduleAccesser);
    }
}

void ftSheikStatusUniqProcessSpecialHiMove::exitStatus(soModuleAccesser* moduleAccesser, int) {
    reinterpret_cast<ftSheikVisibilityModuleView*>(&moduleAccesser->getVisibilityModule())->setWhole(1);
}

ftSheikStatusUniqProcessSpecialHiMove g_ftSheikStatusUniqProcessSpecialHiMove;
