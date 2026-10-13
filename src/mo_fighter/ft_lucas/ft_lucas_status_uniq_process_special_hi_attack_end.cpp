#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/lucas/ft_lucas_status_uniq_process_special_hi.h>
#include <so/model/so_model_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>

// Native recovery tuning: facing multiplier, degrees per radian, zero, zero.
extern float lbl_114_rodata_98[];
extern float lbl_114_rodata_9C;
extern float lbl_114_rodata_A0[];

void ftLucasStatusUniqProcessSpecialHiAttackEnd::initStatus(soModuleAccesser* a) {
    soWorkManageModule& work = a->getWorkManageModule();
    soModelModuleImpl& model = dynamic_cast<soModelModuleImpl&>(a->getModelModule());
    float angle = work.getFloat(0x21000004);
    if (a->getStatusModule().getStatusKind() == 0x11e) angle *= -1.0f;
    model.setNodeRotateX(3, angle * 57.29578f);
    a->getStageObject().updateNodeSRT();
}

void ftLucasStatusUniqProcessSpecialHiAttackEnd::execStatus(soModuleAccesser* a) {
    float* constants = lbl_114_rodata_98;
    soWorkManageModule& work = a->getWorkManageModule();
    soModelModuleImpl& model = dynamic_cast<soModelModuleImpl&>(a->getModelModule());
    float angle = work.getFloat(0x21000004);
    float step = work.getFloat(0x21000006);
    if (angle > constants[2]) {
        angle -= step;
        if (angle < constants[2]) angle = constants[2];
    } else if (angle < constants[2]) {
        angle += step;
        if (angle > constants[2]) angle = constants[2];
    }
    work.setFloat(angle, 0x21000004);
    if (a->getStatusModule().getStatusKind() == 0x11e) angle *= constants[0];
    model.setNodeRotateX(3, constants[1] * angle);
}

void ftLucasStatusUniqProcessSpecialHiAttackEnd::exitStatus(soModuleAccesser* a, int) {
    soModelModuleImpl& model = dynamic_cast<soModelModuleImpl&>(a->getModelModule());
    model.setNodeRotateX(3, 0.0f);
    soPostureModule& posture = a->getPostureModule();
    Vec3f zero(0.0f, 0.0f, 0.0f);
    posture.setRot(&zero, 0);
}

ftLucasStatusUniqProcessSpecialHiAttackEnd::~ftLucasStatusUniqProcessSpecialHiAttackEnd() {}
ftLucasStatusUniqProcessSpecialHiAttackEnd g_ftLucasStatusUniqProcessSpecialHiAttackEnd;
