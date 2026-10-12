#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/controller/so_controller_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <mt/mt_common.h>
#include <types.h>
#include <math.h>

class ftSnakeStatusUniqProcessNikitaOperation : public soStatusUniqProcess {
public:
    virtual ~ftSnakeStatusUniqProcessNikitaOperation() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

// Native tuning block: zero, minimum normal float, unit stick magnitude,
// radians-to-degrees scale, and a full turn in degrees.
extern float lbl_122_rodata_40[5];

#pragma dont_inline on
void ftSnakeStatusUniqProcessNikitaOperation::execStatus(soModuleAccesser* moduleAccesser) {
    float* c = lbl_122_rodata_40;
    soWorkManageModule& workManage = *moduleAccesser->m_enumerationStart->m_workManageModule;
    float prevY = workManage.getFloat(0x21000005);
    float prevX = workManage.getFloat(0x21000004);
    Vec2f from(prevX, prevY);
    soControllerModule& controller = *moduleAccesser->m_enumerationStart->m_controllerModule;
    float stickY = controller.getStickY();
    Vec2f stick(controller.getStickX(), stickY);
    stick.m_x = from.m_x + (stick.m_x - from.m_x) *
        moduleAccesser->m_enumerationStart->m_workManageModule->getFloat(0x21000006);
    stick.m_y = from.m_y + (stick.m_y - from.m_y) *
        moduleAccesser->m_enumerationStart->m_workManageModule->getFloat(0x21000006);
    float lengthSquared = stick.m_x * stick.m_x + stick.m_y * stick.m_y;
    float length;
    double absolute = __fabs(lengthSquared);
    float magnitude = absolute;
    if (magnitude <= c[1]) {
        length = c[0];
    } else {
        length = lengthSquared * rsqrtf(lengthSquared);
    }
    if (length > c[2]) {
        length = c[2];
    }
    (*moduleAccesser->m_enumerationStart->m_motionModule).setWeightRate(c[2] - length - (*moduleAccesser->m_enumerationStart->m_motionModule).getPrevWeight());
    float angle = c[3] * static_cast<float>(atan2(stick.m_y, stick.m_x * moduleAccesser->m_enumerationStart->m_postureModule->getLr()));
    if (angle > c[4]) {
        angle -= c[4];
    } else if (angle < c[0]) {
        angle += c[4];
    }
    // Advance the second animation toward the wrapped aiming angle.
    soMotionModule& motion = *moduleAccesser->m_enumerationStart->m_motionModule;
    motion.set2ndRate(angle - motion.get2ndFrame());
    (*moduleAccesser->m_enumerationStart->m_workManageModule).setFloat(stick.m_x, 0x21000004);
    (*moduleAccesser->m_enumerationStart->m_workManageModule).setFloat(stick.m_y, 0x21000005);
}

#pragma dont_inline reset
ftSnakeStatusUniqProcessNikitaOperation g_ftSnakeStatusUniqProcessNikitaOperation;
