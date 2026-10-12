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

#pragma dont_inline on
float wnZeldaDeinStatusUniqProcessMove::ABS(float value) {
    return __fabsf(value);
}
#pragma dont_inline reset

void wnZeldaDeinStatusUniqProcessMove::execStatus(soModuleAccesser* a) {
    soWorkManageModule& work = a->getWorkManageModule();
    soControllerModule& controller = a->getControllerModule();
    soPostureModule& posture = a->getPostureModule();
    soKineticModule& kinetic = a->getKineticModule();
    wnZeldaDein& dein = dynamic_cast<wnZeldaDein&>(a->getStageObject());
    wnZeldaDeinParam* param = dein.m_param;
    // HYPOTHESIS: lbl_104_rodata_A8 is taken as 0.0f and lbl_104_rodata_AC as 1.0f (values unverified; they must differ or the branch folds).
    const float kA8 = 0.0f;
    const float kAC = 1.0f;
    if (!work.isFlag(0x22000000)) {
        float lr = posture.getLr();
        Vec2f stick;
        stick.m_x = controller.getStickX();
        stick.m_y = controller.getStickY();
        float stickY = stick.m_y;
        float t = work.getFloat(0x21000002);
        if (ABS(stickY) > param->unk20) {
            t += lr * (param->unk24 * stickY);
            if (ABS(t) > param->unk28) {
                float clamp;
                if (t > kA8) {
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
        float base;
        if (lr > kA8) {
            base = kA8;
        } else {
            base = kAC;
        }
        float x = f28 * static_cast<float>(cos(base + t));
        float y = f28 * static_cast<float>(sin(base + t));
        wnZeldaDeinKineticEnergyView& energy =
            dynamic_cast<wnZeldaDeinKineticEnergyView&>(*kinetic.getEnergy(0));
        energy.m_speed.m_x = x;
        energy.m_speed.m_y = y;
    }
    float tt = work.getFloat(0x21000001);
    float scale = param->unk8 + tt * ((param->unkC - param->unk8) / param->unk4);
    posture.setScale(scale);
}
