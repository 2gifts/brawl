#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/samus/wn_samus_bomb.h>
#include <wn/wn_kinetic_energy_normal.h>


// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSamusBombStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnSamusBombStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnSamusBombStatusUniqProcess g_wnSamusBombStatusUniqProcess;

void wnSamusBombStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: the status module query (slot 0x48) selects the branch (0 = copy the weapon value,
    // 1 = reset the kinetic energies and visibility).
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0: {
        wnSamusBomb& weapon = dynamic_cast<wnSamusBomb&>(*moduleAccesser->m_stageObject);
        const wnSamusBombParam* param = weapon.getData()->m_param;
        moduleAccesser->getWorkManageModule().setInt(param->m_life, 0x10000005);
        break;
    }
    case 1: {
        dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0)).disable();
        dynamic_cast<wnKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(2)).disable();
        reinterpret_cast<soVisibilityModule*>(moduleAccesser->m_enumerationStart->m_visibilityModule)->setWhole(0);
        moduleAccesser->getWorkManageModule().setInt(1, 0x10000006);
        break;
    }
    }
}

void wnSamusBombStatusUniqProcess::execFixPos(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0: {
        // HYPOTHESIS: situation query (slot 0x14) decides whether the energy keeps its 0x80 flag.
        int situation = moduleAccesser->getSituationModule().getKind();
        wnKineticEnergyGravity& gravity = dynamic_cast<wnKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(2));
        if (situation == 2) {
            gravity.enable();
        }
        break;
    }
    case 1:
        if (moduleAccesser->getWorkManageModule().getInt(0x10000006) != 0) {
            moduleAccesser->getWorkManageModule().setInt(0, 0x10000006);
        }
        break;
    }
}
