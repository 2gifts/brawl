#define MT_VEC3F_CTOR_NOINLINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <mt/mt_vector.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <math.h>

// HYPOTHESIS: extendParam group 3 of the GameWatch data (ftData::extendParam[2]): the stick dead zone, the tilt
// (degrees per unit of stick past the dead zone) and the value written to work float 0x11000000 on the way into status 0x19.
struct ftGameWatchSpecialHiStartGroup {
    float m_stickDeadZone;
    float m_tiltPerStick;
    float m_exitWorkValue;
};

// Work flag set once the launch tilt has been taken from the stick.
static const int Work_Flag_SpecialHiTiltSet = 0x22000011;

static const float s_degToRad = 0.017453292f;

class ftGamewatchStatusUniqProcessSpecialHiStart : public soStatusUniqProcess {
public:
    virtual ~ftGamewatchStatusUniqProcessSpecialHiStart() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftGamewatchStatusUniqProcessSpecialHiStart g_ftGamewatchStatusUniqProcessSpecialHiStart;

void ftGamewatchStatusUniqProcessSpecialHiStart::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soControllerModule& controller = moduleAccesser->getControllerModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    ftGameWatchSpecialHiStartGroup* group = static_cast<ftGameWatchSpecialHiStartGroup*>(
        g_ftCommonDataAccesser.getData(Fighter_GameWatch)->extendParam[2]);
    if (!workManage.isFlag(Work_Flag_SpecialHiTiltSet)) {
        float stickX = controller.getStickX();
        float stickAbs = __fabsf(stickX);
        if (stickAbs >= group->m_stickDeadZone) {
            float over = stickAbs - group->m_stickDeadZone;
            float sign = stickX > 0.0f ? -1.0f : 1.0f;
            float signedOver = over * sign;
            float tilt = signedOver * group->m_tiltPerStick;
            posture.setRot(&Vec3f(tilt, 0.0f, 0.0f), 0);
            posture.setStickLR(0.0f);
            posture.updateRotYLr();
            moduleAccesser->getStageObject().updateNodeSRT();
            ftKineticEnergyMotion& energy = dynamic_cast<ftKineticEnergyMotion&>(*kinetic.getEnergy(0));
            energy.m_rotation = s_degToRad * tilt;
            workManage.onFlag(Work_Flag_SpecialHiTiltSet);
        }
    }
}

void ftGamewatchStatusUniqProcessSpecialHiStart::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus == 0x19) {
        ftData* data = g_ftCommonDataAccesser.getData(Fighter_GameWatch);
        moduleAccesser->getWorkManageModule().setFloat(
            static_cast<ftGameWatchSpecialHiStartGroup*>(data->extendParam[2])->m_exitWorkValue, 0x11000000);
    }
    if (nextStatus != 0x117) {
        moduleAccesser->getGenerateArticleManageModule().remove(2, 0);
    }
    moduleAccesser->getPostureModule().setRot(&Vec3f(0.0f, 0.0f, 0.0f), 0);
}
