#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

extern "C" {
// Typeinfo symbols of the bomb weapon classes (names not recovered yet, kept as in the target).
extern char lbl_27_data_7D468[];
extern char lbl_27_data_45940[];
extern char __RTTI__11StageObject[];
extern char __RTTI__21wnKineticEnergyNormal[];
extern char __RTTI__15soKineticEnergy[];
void* __dynamic_cast(void* ptr, long vtblOffset, const void* target, const void* source, int isRef);
// Bomb weapon getter (sora_melee REL); no declaration exists in the headers yet.
void* fn_27_2E95E8(void* weapon);
}

// Weapon data as returned by the getter: the value pointer sits at +0x34.
class wnSamusBombData {
public:
    char _00[0x34];
    int* m_value;
};

// Kinetic energy flag byte at +5; the top bit (0x80) of that byte is cleared here.
class soKineticEnergyFlagView {
public:
    char _00[5];
    u8 m_flag80 : 1;
};

static inline void clearEnergyFlag80(void* energy) {
    static_cast<soKineticEnergyFlagView*>(energy)->m_flag80 = 0;
}

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
        void* weapon = __dynamic_cast(moduleAccesser->m_stageObject, 0x3c, lbl_27_data_7D468, __RTTI__11StageObject, 1);
        wnSamusBombData* data = static_cast<wnSamusBombData*>(fn_27_2E95E8(weapon));
        moduleAccesser->getWorkManageModule().setInt(*data->m_value, 0x10000005);
        break;
    }
    case 1: {
        clearEnergyFlag80(__dynamic_cast(moduleAccesser->getKineticModule().getEnergy(0), 0, __RTTI__21wnKineticEnergyNormal, __RTTI__15soKineticEnergy, 1));
        clearEnergyFlag80(__dynamic_cast(moduleAccesser->getKineticModule().getEnergy(2), 0, lbl_27_data_45940, __RTTI__15soKineticEnergy, 1));
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
        void* energy = __dynamic_cast(moduleAccesser->getKineticModule().getEnergy(2), 0, lbl_27_data_45940, __RTTI__15soKineticEnergy, 1);
        if (situation == 2) {
            static_cast<soKineticEnergyFlagView*>(energy)->m_flag80 = 1;
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
