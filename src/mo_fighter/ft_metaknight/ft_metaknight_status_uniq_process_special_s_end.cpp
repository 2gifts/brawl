#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/metaknight/ft_metaknight_status_views.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftMetaknightStatusUniqProcessSpecialSEnd : public soStatusUniqProcess {
public:
    virtual ~ftMetaknightStatusUniqProcessSpecialSEnd() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

// Drill Rush recovers its pitch by twice the current rotation divided by the remaining count.
static const float kSEndVectorValue = 0.0f;
static const float kSEndScale = 2.0f;

void ftMetaknightStatusUniqProcessSpecialSEnd::execStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: work id 0x20000002 is a remaining-count work slot (0 means the move is finished).
    int count = moduleAccesser->getWorkManageModule().getInt(0x20000002);
    if (count <= 0) {
        soPostureModule* posture = &moduleAccesser->getPostureModule();
        posture->setRot(&Vec3f(kSEndVectorValue, kSEndVectorValue, kSEndVectorValue), 0);
        return;
    }
    float x = moduleAccesser->getPostureModule().getRot(0).m_x;
    float step = (kSEndScale * x) / (float)count;
    soPostureModule* posture = &moduleAccesser->getPostureModule();
    posture->setRot(&Vec3f(x - step, posture->getRot(0).m_y, posture->getRot(0).m_z), 0);
}

void ftMetaknightStatusUniqProcessSpecialSEnd::exitStatus(soModuleAccesser* moduleAccesser, int) {
    soPostureModule* posture = &moduleAccesser->getPostureModule();
    posture->setRot(&Vec3f(kSEndVectorValue, kSEndVectorValue, kSEndVectorValue), 0);
    moduleAccesser->getPostureModule().updateRotYLr();
}

ftMetaknightStatusUniqProcessSpecialSEnd g_ftMetaknightStatusUniqProcessSpecialSEnd;
