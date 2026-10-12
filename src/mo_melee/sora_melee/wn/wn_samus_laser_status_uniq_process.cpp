#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

extern "C" {
// Typeinfo of the Samus laser weapon class (its name is not recovered yet).
extern char lbl_27_data_7ED88[];
extern char __RTTI__11StageObject[];
void* __dynamic_cast(void* ptr, long vtblOffset, const void* target, const void* source, int isRef);
// wnSamusLaser getter (sora_melee REL); no declaration exists in the headers yet.
void* fn_27_2F2104(void* weapon);
}

// Weapon data as returned by the getter: the value pointer sits at +0x34.
class wnSamusLaserData {
public:
    char _00[0x34];
    int* m_value;
};

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSamusLaserStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnSamusLaserStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnSamusLaserStatusUniqProcess g_wnSamusLaserStatusUniqProcess;

void wnSamusLaserStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: the status module query (slot 0x48) selects whether the weapon data is copied.
    int kind = moduleAccesser->getStatusModule().getStatusKind();
    soWorkManageModule* workManage = &moduleAccesser->getWorkManageModule();
    switch (kind) {
    case 0: {
        void* weapon = __dynamic_cast(moduleAccesser->m_stageObject, 0x3c, lbl_27_data_7ED88, __RTTI__11StageObject, 1);
        workManage->setInt(*static_cast<wnSamusLaserData*>(fn_27_2F2104(weapon))->m_value, 0x10000005);
        // HYPOTHESIS: constant from lbl_27_rodata_64C8 (value not verified).
        workManage->setFloat(1.0f, 0x11000001);
        break;
    }
    case 1:
        break;
    }
}

void wnSamusLaserStatusUniqProcess::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void wnSamusLaserStatusUniqProcess::execStatus(soModuleAccesser* moduleAccesser) { }
void wnSamusLaserStatusUniqProcess::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
