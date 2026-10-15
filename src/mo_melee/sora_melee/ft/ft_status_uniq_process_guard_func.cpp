#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_guard.h>
#include <math.h>
#include <mt/mt_common.h>
#include <mt/mt_vector.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/stop/so_stop_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

ftStatusUniqProcessGuardFunc g_ftStatusUniqProcessGuardFunc;

// HYPOTHESIS: the original uses its own inline length helper (float FLT_MIN compare and inline fabs) instead of
// Vec3f::length from the shared header (same helper as in the glide statuses).
static inline float guardStickLength(float lengthSq) {
    float length;
    if ((float)__fabs(lengthSq) <= 1.17549435e-38f) {
        length = 0.0f;
    } else {
        length = lengthSq * rsqrtf(lengthSq);
    }
    return length;
}

// Every frame of the shield: the stick is smoothed against the value kept in work floats 0x21000004/5, its length
// (clamped to 1) drives the shield's animation blend and its direction (degrees, 0..360) the second animation frame, so
// the bubble tilts towards the stick. The smoothed stick is stored back and the shield scale is refreshed.
void ftStatusUniqProcessGuardFunc::updateShield(soModuleAccesser* moduleAccesser) {
    soMotionModule* motion = moduleAccesser->m_enumerationStart->m_motionModule;
    soWorkManageModule* work = moduleAccesser->m_enumerationStart->m_workManageModule;
    Vec2f previous(work->getFloat(0x21000004), work->getFloat(0x21000005));
    Vec2f stick(moduleAccesser->getControllerModule().getStickX(), moduleAccesser->getControllerModule().getStickY());
    stick.m_x = previous.m_x + (stick.m_x - previous.m_x) * soValueAccesser::getConstantFloat(moduleAccesser, 0xcb2, 0);
    stick.m_y = previous.m_y + (stick.m_y - previous.m_y) * soValueAccesser::getConstantFloat(moduleAccesser, 0xcb2, 0);
    float lengthSq = stick.m_x * stick.m_x + stick.m_y * stick.m_y;
    float length = guardStickLength(lengthSq);
    if (length > 1.0f) {
        length = 1.0f;
    }
    motion->setWeightRate((1.0f - length) - motion->getPrevWeight());
    float lr = reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).getLr();
    float stickX = stick.m_x;
    float stickY = stick.m_y;
    float scaledX = stickX * lr;
    float angle = atan2(stickY, scaledX);
    float degrees = 57.29578f * angle;
    if (degrees > 360.0f) {
        degrees -= 360.0f;
    } else if (degrees < 0.0f) {
        degrees += 360.0f;
    }
    motion->set2ndRate(degrees - motion->get2ndFrame());
    work->setFloat(stick.m_x, 0x21000004);
    work->setFloat(stick.m_y, 0x21000005);
    setShieldScale(moduleAccesser);
}

// The bubble scale follows the shield health (work float 0x11000003 over its maximum) between the minimum scale and 1,
// and is applied to the shield node (0x12c).
void ftStatusUniqProcessGuardFunc::setShieldScale(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
    float maxHealth = soValueAccesser::getConstantFloat(moduleAccesser, 0xca1, 0);
    float ratio = work->getFloat(0x11000003) / maxHealth;
    float fullScale = soValueAccesser::getConstantFloat(moduleAccesser, 0xcaf, 0);
    ratio = fullScale * ratio;
    float minScale = soValueAccesser::getConstantFloat(moduleAccesser, 0xca2, 0);
    ratio = minScale + ratio * (1.0f - minScale);
    float scale = ratio * soValueAccesser::getConstantFloat(moduleAccesser, 0xbe6, 0);
    Vec3f nodeScale(scale, scale, scale);
    soModelModule* model = moduleAccesser->m_enumerationStart->m_modelModule;
    model->setNodeScale(model->getCorrectNodeId(0x12c), &nodeScale);
}

// At the end of hit-stop a held stick pushes the shielding fighter sideways (shield DI): the stick x, scaled by constant
// 0xc67 and the work float 0x21000006 (HYPOTHESIS: the shield push strength), is added to the x position.
void ftStatusUniqProcessGuardFunc::checkHitStopDelay(soModuleAccesser* moduleAccesser, bool isHitStopEnd) {
    if (isHitStopEnd == true) {
        float stickX = moduleAccesser->getControllerModule().getStickX();
        double wideX = __fabs(stickX);
        float absX = wideX;
        if (absX >= soValueAccesser::getConstantFloat(moduleAccesser, 0xc65, 0)) {
            Vec3f position = moduleAccesser->getPostureModule().getPos();
            float strength = moduleAccesser->getWorkManageModule().getFloat(0x21000006);
            float pushScale = soValueAccesser::getConstantFloat(moduleAccesser, 0xc67, 0);
            float push = stickX * pushScale;
            position.m_x = position.m_x + push * strength;
            moduleAccesser->getPostureModule().setPos(&position);
        }
    }
}

// Same push while the hit-stop is running: only when the stick was flicked recently (flick frames below constant 0xc65),
// with the scale constants 0xc66 and 0xc68, and the flick is consumed afterwards.
void ftStatusUniqProcessGuardFunc::checkHitStopDelayFlick(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->m_enumerationStart->m_stopModule->isHit() == true) {
        soControllerModule* controller = &moduleAccesser->getControllerModule();
        float limit = soValueAccesser::getConstantFloat(moduleAccesser, 0xc65, 0);
        if ((float)controller->getFlickX() < limit) {
            float stickX = moduleAccesser->getControllerModule().getStickX();
            double wideX = __fabs(stickX);
        float absX = wideX;
            if (absX >= soValueAccesser::getConstantFloat(moduleAccesser, 0xc65, 0)) {
                Vec3f position = moduleAccesser->getPostureModule().getPos();
                soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
                float scaleB = soValueAccesser::getConstantFloat(moduleAccesser, 0xc68, 0);
                float scaleA = soValueAccesser::getConstantFloat(moduleAccesser, 0xc66, 0);
                float partial = stickX * scaleA;
                float push = partial * scaleB;
                float strength = work->getFloat(0x21000006);
                position.m_x = position.m_x + strength * push;
                moduleAccesser->getPostureModule().setPos(&position);
                moduleAccesser->getControllerModule().resetFlickX();
            }
        }
    }
}
