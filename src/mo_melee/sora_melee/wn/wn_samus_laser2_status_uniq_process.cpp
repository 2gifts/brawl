#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <ft/ft_common_data_accesser.h>
#include <gf/gf_task_scheduler.h>
#include <so/so_external_value_accesser.h>
#include <wn/samus/wn_samus_laser2.h>

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
    wnSamusLaser2& weapon = dynamic_cast<wnSamusLaser2&>(*moduleAccesser->m_stageObject);
    int* value = weapon.getData()->m_value;
    moduleAccesser->getWorkManageModule().setInt(*value, 0x10000005);
    workManage->setFloat(1.0f, 0x11000002);
}

void wnSamusLaser2StatusUniqProcess::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getStatusModule().getStatusKind();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    float length = work.getFloat(0x11000002);
    length += static_cast<float*>(g_ftCommonDataAccesser.getData(static_cast<ftKind>(3))->extendParam[4])[5];
    work.setFloat(length, 0x11000002);
    int founderId = moduleAccesser->getWorkManageModule().getInt(0x10000000);
    StageObject* founder = dynamic_cast<StageObject*>(gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Fighter, founderId));
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
