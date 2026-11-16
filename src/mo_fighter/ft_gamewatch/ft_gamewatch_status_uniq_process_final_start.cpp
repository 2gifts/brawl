#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftGamewatchStatusUniqProcessFinalStart : public soStatusUniqProcess {
public:
    virtual ~ftGamewatchStatusUniqProcessFinalStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftGamewatchStatusUniqProcessFinalStart g_ftGamewatchStatusUniqProcessFinalStart;

void ftGamewatchStatusUniqProcessFinalStart::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    work.setFloat(12.0f * moduleAccesser->getPostureModule().getScale(), 0x11000012);
}

// HYPOTHESIS: a rectangle setter (the original may give Rect2D a four-float constructor). Inline-call arguments that need
// computation are evaluated last to first, which is the order the original computes the four edges in.
static inline void setRect(Rect2D& rect, float left, float right, float up, float down) {
    rect.m_left = left;
    rect.m_right = right;
    rect.m_up = up;
    rect.m_down = down;
}

// Leaving the start status for another Final Smash status (0x11d to 0x138) offsets the camera range by fixed targets minus
// the current range.
void ftGamewatchStatusUniqProcessFinalStart::exitStatus(soModuleAccesser* moduleAccesser, int kind) {
    if (kind >= 0x11d && kind <= 0x138) {
        soCameraRange* range = moduleAccesser->getCameraModule().getCameraRangeData(0);
        Rect2D rect;
        setRect(rect, -50.0f - range->m_range.m_right, 50.0f - range->m_range.m_left, 20.0f - range->m_range.m_up,
                -30.0f - range->m_range.m_down);
        moduleAccesser->getCameraModule().addCameraRangeRect(&rect, 0);
        Vec3f offset(0.0f, 30.0f - range->unk0, 0.0f);
        moduleAccesser->getCameraModule().addCameraRangeOffset(&offset, 0);
        moduleAccesser->getCameraModule().setDemoDistScaleMul(3.0f, 0);
    }
}

