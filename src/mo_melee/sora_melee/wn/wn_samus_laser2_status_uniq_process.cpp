#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

extern "C" {
// Typeinfo of the Samus laser2 weapon class (its name is not recovered yet).
extern char lbl_27_data_7F450[];
extern char __RTTI__11StageObject[];
void* __dynamic_cast(void* ptr, long vtblOffset, const void* target, const void* source, int isRef);
// wnSamusLaser2::getData (sora_melee REL); no declaration exists in the headers yet.
void* fn_27_2F3AA4(void* weapon);
}

// Weapon data as returned by getData: the value pointer sits at +0x34.
class wnSamusLaser2Data {
public:
    char _00[0x34];
    int* m_value;
};

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSamusLaser2StatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnSamusLaser2StatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnSamusLaser2StatusUniqProcess g_wnSamusLaser2StatusUniqProcess;

// Copies the weapon's stored value into work int 0x10000005 and sets work float 0x11000002.
// HYPOTHESIS: the float constant is from lbl_27_rodata_64D8 (value not verified).
void wnSamusLaser2StatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* workManage = &moduleAccesser->getWorkManageModule();
    void* weapon = __dynamic_cast(moduleAccesser->m_stageObject, 0x3c, lbl_27_data_7F450, __RTTI__11StageObject, 1);
    wnSamusLaser2Data* data = static_cast<wnSamusLaser2Data*>(fn_27_2F3AA4(weapon));
    moduleAccesser->getWorkManageModule().setInt(*data->m_value, 0x10000005);
    workManage->setFloat(1.0f, 0x11000002);
}

void wnSamusLaser2StatusUniqProcess::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
