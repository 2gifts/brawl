#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <wn/zelda/wn_zelda_dein_view.h>
#include <math.h>
#include <types.h>

class wnZeldaDeinStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnZeldaDeinStatusUniqProcessMove() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
private:
    static float ABS(float value);
};

wnZeldaDeinStatusUniqProcessMove g_wnZeldaDeinStatusUniqProcessMove;

void wnZeldaDeinStatusUniqProcessMove::execStatus(soModuleAccesser* a) {
    soWorkManageModule& work = a->getWorkManageModule();
    soControllerModule& controller = a->getControllerModule();
    soPostureModule& posture = a->getPostureModule();
    soKineticModule& kinetic = a->getKineticModule();
    wnZeldaDein& dein = dynamic_cast<wnZeldaDein&>(a->getStageObject());
    wnZeldaDeinParam* param = dein.m_param;
    if (!work.isFlag(0x22000000)) {
        float lr = posture.getLr();
        Vec2f stick;
        stick.m_x = controller.getStickX();
        stick.m_y = controller.getStickY();
        float t = work.getFloat(0x21000002);
        if (ABS(stick.m_y) > param->unk20) {
            t += lr * (param->unk24 * stick.m_y);
            if (ABS(t) > param->unk28) {
                float clamp;
                if (t > 0.0f) {
                    clamp = param->unk28;
                } else {
                    clamp = -param->unk28;
                }
                t = clamp;
            }
            work.setFloat(t, 0x21000002);
        }
        float cur = work.getFloat(0x21000003);
        float f28 = cur + param->unk18;
        if (f28 > param->unk1C) {
            f28 = param->unk1C;
        }
        work.setFloat(f28, 0x21000003);
        // The drift angle is measured from 0 facing right and from pi facing left (rodata 0.0f and 3.1415927f).
        float base = lr > 0.0f ? 0.0f : 3.1415927f;
        Vec2f speed;
        speed.m_x = f28 * static_cast<float>(cos(base + t));
        speed.m_y = f28 * static_cast<float>(sin(base + t));
        wnZeldaDeinKineticEnergyView& energy =
            dynamic_cast<wnZeldaDeinKineticEnergyView&>(*kinetic.getEnergy(0));
        energy.m_speed = speed;
    }
    float tt = work.getFloat(0x21000001);
    float scale = param->unk8 + tt * ((param->unkC - param->unk8) / param->unk4);
    posture.setScale(scale);
}

#pragma dont_inline on
float wnZeldaDeinStatusUniqProcessMove::ABS(float value) {
    return __fabsf(value);
}
#pragma dont_inline reset
