#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <ft/ft_common_data_accesser.h>
#include <gf/gf_task_scheduler.h>
#include <so/so_external_value_accesser.h>
#include <ft/samus/ft_samus_status_uniq_process.h>
#include <wn/samus/wn_samus_laser.h>

extern "C" char __RTTI__6gfTask[];

extern "C" {
// Typeinfo of the Samus laser weapon class (its name is not recovered yet).
extern char lbl_27_data_7ED88[];
extern char __RTTI__11StageObject[];
void* __dynamic_cast(void* ptr, long vtblOffset, const void* target, const void* source, int isRef);
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
        workManage->setInt(*static_cast<wnSamusLaser*>(weapon)->getData()->m_value, 0x10000005);
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
void wnSamusLaserStatusUniqProcess::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    int status = moduleAccesser->getStatusModule().getStatusKind();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (status == 0) {
        return;
    }
    float offset = work.getFloat(0x11000001);
    offset += static_cast<ftSamusExtendParamFinal*>(g_ftCommonDataAccesser.getData(Fighter_Samus)->extendParam[4])->unk14;
    work.setFloat(offset, 0x11000001);
    int ownerId = work.getInt(0x10000000);
    StageObject* owner = static_cast<StageObject*>(__dynamic_cast(gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Fighter, ownerId), 0x3c, __RTTI__11StageObject, __RTTI__6gfTask, 0));
    if (owner == 0) {
        return;
    }
    soModuleAccesser* ownerAccesser = soExternalValueAccesser::getModuleAccesser(owner);
    float rotation = ownerAccesser->getWorkManageModule().getFloat(0x11000014);
    Vec3f position = ownerAccesser->getModelModule().getNodeGlobalPosition(0x2B, false);
    moduleAccesser->getPostureModule().init(&position, ownerAccesser->getPostureModule().getLr());
    Vec3f attackOffset(0.0f, 0.0f, offset);
    moduleAccesser->getCollisionAttackModule().setOffset(0, &attackOffset);
    Vec3f angle(0.0f, 0.0f, rotation);
    moduleAccesser->getPostureModule().setRot(&angle, 0);
}
