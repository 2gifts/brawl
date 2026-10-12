#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <wn/zelda/wn_zelda_dein_view.h>
#include <types.h>

class wnZeldaDeinStatusUniqProcessTame : public soStatusUniqProcess {
public:
    virtual ~wnZeldaDeinStatusUniqProcessTame() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnZeldaDeinStatusUniqProcessTame g_wnZeldaDeinStatusUniqProcessTame;

void wnZeldaDeinStatusUniqProcessTame::initStatus(soModuleAccesser* a) {
    soWorkManageModule& work = a->getWorkManageModule();
    work.getInt(0x20000000);
    work.setFloat(dynamic_cast<wnZeldaDein&>(a->getStageObject()).m_param->unk2C, 0x21000000);
}

void wnZeldaDeinStatusUniqProcessTame::execStatus(soModuleAccesser* a) {
    wnZeldaDeinKineticEnergyView& energy =
        dynamic_cast<wnZeldaDeinKineticEnergyView&>(*a->getKineticModule().getEnergy(0));
    // HYPOTHESIS: both speed components start from the rodata constant lbl_104_rodata_B0 (value unverified).
    float c = 0.0f;
    energy.m_speed = Vec2f(c, c);
}
