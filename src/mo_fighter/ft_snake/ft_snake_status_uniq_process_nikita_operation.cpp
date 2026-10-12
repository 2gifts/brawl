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

// HYPOTHESIS: aim-tuning constants (c0 default magnitude, c1 small-magnitude limit, c2 maximum magnitude,
// c3 angle scale, c4 full turn). Values are in the rodata block and are not yet verified.
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
    float blend = (*moduleAccesser->m_enumerationStart->m_workManageModule).getFloat(0x21000006);
    stick.m_x = from.m_x + (stick.m_x - from.m_x) * blend;
    stick.m_y = from.m_y + (stick.m_y - from.m_y) * blend;
    float lengthSquared = stick.m_x * stick.m_x + stick.m_y * stick.m_y;
    float length;
    if (__fabsf(lengthSquared) > c[1]) {
        length = c[0];
    } else {
        length = lengthSquared * rsqrtf(lengthSquared);
    }
    if (length > c[2]) {
        length = c[2];
    }
    (*moduleAccesser->m_enumerationStart->m_motionModule).setWeightRate(c[2] - length - (*moduleAccesser->m_enumerationStart->m_motionModule).getPrevWeight());
    float angle = static_cast<float>(atan2(stick.m_y, stick.m_x * moduleAccesser->m_enumerationStart->m_postureModule->getLr()));
    angle = c[3] * angle;
    if (angle > c[4]) {
        angle -= c[4];
    } else if (angle < c[0]) {
        angle += c[4];
    }
    // HYPOTHESIS: the final angle is written to the motion module's second-animation rate.
    (*moduleAccesser->m_enumerationStart->m_motionModule).set2ndRate(angle - (*moduleAccesser->m_enumerationStart->m_motionModule).get2ndFrame());
    (*moduleAccesser->m_enumerationStart->m_workManageModule).setFloat(stick.m_x, 0x21000004);
    (*moduleAccesser->m_enumerationStart->m_workManageModule).setFloat(stick.m_y, 0x21000005);
}

#pragma dont_inline reset
ftSnakeStatusUniqProcessNikitaOperation g_ftSnakeStatusUniqProcessNikitaOperation;
