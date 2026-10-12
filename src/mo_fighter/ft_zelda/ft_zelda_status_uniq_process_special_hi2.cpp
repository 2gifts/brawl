#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_energy.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Local view of the visibility module; its vtable has two leading words before the first declared slot.
class ftZeldaVisibilityModuleView {
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

class ftZeldaStatusUniqProcessSpecialHi2 : public soStatusUniqProcess {
public:
    virtual ~ftZeldaStatusUniqProcessSpecialHi2() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

// HYPOTHESIS: not reconstructed yet (large initialization with kinetic and parameter math); left as a stub.
void ftZeldaStatusUniqProcessSpecialHi2::initStatus(soModuleAccesser*) {
}

// HYPOTHESIS: not reconstructed yet (atan2-based direction math and kinetic updates); left as a stub.
void ftZeldaStatusUniqProcessSpecialHi2::execFixPosCounter(soModuleAccesser*) {
}

void ftZeldaStatusUniqProcessSpecialHi2::execFixPos(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soStatusModule& status = moduleAccesser->getStatusModule();
    if (workManage.isFlag(0x22000012)) {
        workManage.offFlag(0x22000012);
        status.changeStatusRequest(0x11a, moduleAccesser);
    }
}

void ftZeldaStatusUniqProcessSpecialHi2::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x11a) {
        reinterpret_cast<ftZeldaVisibilityModuleView*>(&moduleAccesser->getVisibilityModule())->setWhole(1);
    }
}

ftZeldaStatusUniqProcessSpecialHi2 g_ftZeldaStatusUniqProcessSpecialHi2;
