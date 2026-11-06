#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <ft/ft_common_data_accesser.h>
#include <gf/gf_task_scheduler.h>
#include <so/so_external_value_accesser.h>
#include <wn/samus/wn_samus_laser2.h>

extern "C" {
// Typeinfo of the Samus laser2 weapon class (its name is not recovered yet).
extern char lbl_27_data_7F450[];
extern char __RTTI__11StageObject[];
extern char __RTTI__6gfTask[];
void* __dynamic_cast(void* ptr, long vtblOffset, const void* target, const void* source, int isRef);
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
    wnSamusLaser2Data* data = const_cast<wnSamusLaser2Data*>(static_cast<wnSamusLaser2*>(weapon)->getData());
    moduleAccesser->getWorkManageModule().setInt(*data->m_value, 0x10000005);
    workManage->setFloat(1.0f, 0x11000002);
}

void wnSamusLaser2StatusUniqProcess::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getStatusModule().getStatusKind();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    float length = work.getFloat(0x11000002);
    length += static_cast<float*>(g_ftCommonDataAccesser.getData(static_cast<ftKind>(3))->extendParam[4])[5];
    work.setFloat(length, 0x11000002);
    int founderId = moduleAccesser->getWorkManageModule().getInt(0x10000000);
    StageObject* founder = static_cast<StageObject*>(__dynamic_cast(gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Fighter, founderId), 0x3C, __RTTI__11StageObject, __RTTI__6gfTask, 0));
    if (founder != NULL) {
        soModuleAccesser* founderAccesser = soExternalValueAccesser::getModuleAccesser(founder);
        soWorkManageModule& laserWork = moduleAccesser->getWorkManageModule();
        float rotation = founderAccesser->getWorkManageModule().getFloat(0x11000014);
        Vec3f position = founderAccesser->getModelModule().getNodeGlobalPosition(0x2B, false);
        float scale = moduleAccesser->getPostureModule().getScale();
        float height = laserWork.getFloat(0x11000001);
        height *= scale;
        position.m_y += height;
        moduleAccesser->getPostureModule().init(&position, founderAccesser->getPostureModule().getLr());
        Vec3f offset(0.0f, 0.0f, length);
        moduleAccesser->getCollisionAttackModule().setOffset(0, &offset);
        Vec3f angles(0.0f, 0.0f, rotation);
        moduleAccesser->getPostureModule().setRot(&angles, 0);
    }
}
