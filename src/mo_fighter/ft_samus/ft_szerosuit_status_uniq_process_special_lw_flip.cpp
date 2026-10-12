#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftSZerosuitStatusUniqProcessSpecialLwFlip : public soStatusUniqProcess {
public:
    virtual ~ftSZerosuitStatusUniqProcessSpecialLwFlip() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

// Copies the speed into the energy as the native code does (inlined member setter).
inline void soKineticEnergyNormal::setSpeed(Vec2f* speed) {
    m_speed.m_x = speed->m_x;
    m_speed.m_y = speed->m_y;
}

// HYPOTHESIS: extendParam[2] is the Zero Suit special-lw parameter group; params[2] is the
// horizontal launch scale and params[3] the vertical launch speed.
void ftSZerosuitStatusUniqProcessSpecialLwFlip::initStatus(soModuleAccesser* moduleAccesser) {
    soControllerModule& controller = moduleAccesser->getControllerModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    const float* params = (const float*)g_ftCommonDataAccesser.getData(static_cast<ftKind>(0x18))->extendParam[2];
    kinetic.changeKinetic(1, moduleAccesser);
    float speedX = params[2] * controller.getStickX();
    float speedY = params[3];
    ftKineticEnergyController& launch = dynamic_cast<ftKineticEnergyController&>(*kinetic.getEnergy(2));
    Vec2f speed(speedX, 0.0f); // HYPOTHESIS: zero y speed (lbl_94_rodata_70 == 0.0f)
    launch.setSpeed(&speed);
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
    gravity.m_speedY = speedY;
}

void ftSZerosuitStatusUniqProcessSpecialLwFlip::execStatus(soModuleAccesser*) { }

void ftSZerosuitStatusUniqProcessSpecialLwFlip::exitStatus(soModuleAccesser* moduleAccesser, int) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    if (workManage.isFlag(0x22000017)) {
        // HYPOTHESIS: the flip turns the fighter around (reverse facing) and refreshes its Y rotation.
        posture.reverseLr();
        posture.updateRotYLr();
    }
}

ftSZerosuitStatusUniqProcessSpecialLwFlip g_ftSZerosuitStatusUniqProcessSpecialLwFlip;
