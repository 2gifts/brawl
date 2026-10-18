#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <ft/ft_status_uniq_process_views.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessCaptureGokulin : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptureGokulin() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessCaptureGokulin g_ftStatusUniqProcessCaptureGokulin;

// HYPOTHESIS: installs a single capsule hit part on node 0x130 (start (0, 10, 0), end (0, 12, 0), size 8) in hit
// group 0, sets its status to 3, then resets part 0 to status 0 (names follow soCollisionHitModule's slot order).
void ftStatusUniqProcessCaptureGokulin::initStatus(soModuleAccesser* moduleAccesser) {
    soCollisionHitData hitData;
    hitData.m_startOffsetPos.m_x = 0.0f;
    hitData.m_startOffsetPos.m_y = 10.0f;
    hitData.m_startOffsetPos.m_z = 0.0f;
    hitData.m_endOffsetPos.m_x = 0.0f;
    hitData.m_endOffsetPos.m_y = 12.0f;
    hitData.m_endOffsetPos.m_z = 0.0f;
    hitData.m_size = 8.0f;
    hitData.m_nodeIndex = moduleAccesser->getModelModule().getCorrectNodeId(0x130);
    hitData.m_part = (soCollision::Part)0;
    hitData.m_height = soCollisionHitData::Height_Low;
    hitData.m_0x1c_3 = 0;
    hitData.m_shapeType = (soCollision::ShapeType)1;
    moduleAccesser->getCollisionHitModule().set(0, &hitData, 0);
    moduleAccesser->getCollisionHitModule().setStatusAll(3, 0);
    moduleAccesser->getCollisionHitModule().setStatus(0, 0, 0);
}
void ftStatusUniqProcessCaptureGokulin::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot44();
}
void ftStatusUniqProcessCaptureGokulin::exitStatus(soModuleAccesser* moduleAccesser, int) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot30(0, 0);
    // HYPOTHESIS: the indefinite constant 0xA803 is the address of the fighter's hit data set.
    moduleAccesser->getCollisionHitModule().set(
        (soSet<soCollisionHitData>*)soValueAccesser::getConstantIndefinite(moduleAccesser, 0xA803, 0), 0);
}
bool ftStatusUniqProcessCaptureGokulin::checkDamage(soModuleAccesser* moduleAccesser, void*) { return true; }
