#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_views.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/visibility/so_visibility_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessSwallowedDrink : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessSwallowedDrink() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessSwallowedDrink g_ftStatusUniqProcessSwallowedDrink;

// HYPOTHESIS: being swallowed stops all four kinetic energies, holds the capture, makes the fighter intangible and sets
// work flags 0x12000001 and 0x12000029.
void ftStatusUniqProcessSwallowedDrink::initStatus(soModuleAccesser* moduleAccesser) {
    ftKineticEnergyDisableAndClear(3, moduleAccesser);
    ftKineticEnergyDisableAndClear(1, moduleAccesser);
    ftKineticEnergyDisableAndClear(2, moduleAccesser);
    ftKineticEnergyDisableAndClear(4, moduleAccesser);
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot3c();
    moduleAccesser->getCollisionHitModule().setXluGlobal(0);
    moduleAccesser->getWorkManageModule().onFlag(0x12000001);
    moduleAccesser->getWorkManageModule().onFlag(0x12000029);
}

void ftStatusUniqProcessSwallowedDrink::execStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessSwallowedDrink::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot44();
}

// HYPOTHESIS: leaving the swallow (except into status 0xd2) releases the capture and restores intangibility and visibility.
void ftStatusUniqProcessSwallowedDrink::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (nextStatus) {
    case 0xd2:
        return;
    default:
        reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot30(1, 0);
        moduleAccesser->getCollisionHitModule().cancelXluGlobal(0);
        moduleAccesser->getVisibilityModule().setWhole(1);
        moduleAccesser->getEffectModule().setWhole(true);
        break;
    }
}
