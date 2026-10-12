#include <it/it_gen_archive.h>
#include <math.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Native per-unit item sheets are initialized even in the article path.
static itGenSheetKind g_unkItemSheet0(Stages::srStageKind(0xff), 0);
static itGenSheetKind g_unkItemSheet1(Stages::srStageKind(0xff), 1);

class ftDededeStatusUniqProcessSpecialWaddledeeThrow : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialWaddledeeThrow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

float ftDededeWaddledeeThrowABS(float value);

// TODO: reconstruct selection, launch and cleanup; this unit remains NonMatching.
void ftDededeStatusUniqProcessSpecialWaddledeeThrow::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialWaddledeeThrow::execStatus(soModuleAccesser* moduleAccesser) {
    float angle = soValueAccesser::getConstantFloat(moduleAccesser, 0xFAC, 0);
    float stickY = moduleAccesser->getControllerModule().getStickY();

    float delta;
    if (stickY < 0.0f) {
        if (soValueAccesser::getConstantFloat(moduleAccesser, 0xFAD, 0) < 0.0f) {
            delta = angle + ftDededeWaddledeeThrowABS(
                soValueAccesser::getConstantFloat(moduleAccesser, 0xFAD, 0));
        } else {
            delta = angle - soValueAccesser::getConstantFloat(moduleAccesser, 0xFAD, 0);
        }
        angle = angle - delta * ftDededeWaddledeeThrowABS(stickY);
    } else if (stickY > 0.0f) {
        delta = soValueAccesser::getConstantFloat(moduleAccesser, 0xFAE, 0) - angle;
        angle = angle + delta * ftDededeWaddledeeThrowABS(stickY);
    }

    if (angle < 0.0f) {
        angle += 360.0f;
    }

    moduleAccesser->getWorkManageModule().setFloat(angle, 0x21000004);
}

// HYPOTHESIS: source name for the map's file-local ABS_f helper.
#pragma dont_inline on
float ftDededeWaddledeeThrowABS(float value) {
    return __fabsf(value);
}
#pragma dont_inline reset

void ftDededeStatusUniqProcessSpecialWaddledeeThrow::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialWaddledeeThrow::exitStatus(soModuleAccesser* moduleAccesser, int) {
}

ftDededeStatusUniqProcessSpecialWaddledeeThrow g_ftDededeStatusUniqProcessSpecialWaddledeeThrow;