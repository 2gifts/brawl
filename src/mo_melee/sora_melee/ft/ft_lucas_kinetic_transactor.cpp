#include <ft/builder/ft_dol_array_list.h>
#include <ft/builder/ft_builder_kinetic.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>

// Lucas's kinetic transactor (same declaration as include/ft/lucas/ft_lucas.h, which pulls in the whole fighter builder), plus the
// out-of-line setup of mode 0x68. Ness's is the same code.
class ftLucasKineticTransactor : public ftKineticTransactor {
public:
    static void changeKinetic(int mode, void* pools, soModuleAccesser* acc);
    static void changeKineticSub(bool*, void* pools, Vec2f* speed, soModuleAccesser* acc) __attribute__((never_inline));
};

// The per-mode setups (inlined; each takes the carried speed by value): the gravity energy keeps the vertical part, the
// stop energy the horizontal part, and the gravity comes from a constant.
static inline void setupMode64(soModuleAccesser* acc, Vec2f speed) {
    soKineticUtility::resetEnableEnergy(1, acc, 0, &Vec2f(0.0f, speed.m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(3, acc, 6, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    float gravity = acc->getConstantFloatKirby(0xfa0);
    dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1)).m_gravity = -gravity;
}

// The horizontal speed is divided by constant 0xfaa.
static inline void setupMode65(soModuleAccesser* acc, Vec2f speed) {
    float stopDiv = soValueAccesser::getConstantFloat(acc, 0xfaa, 0);
    float gravity = soValueAccesser::getConstantFloat(acc, 0xfab, 0);
    soKineticUtility::resetEnableEnergy(1, acc, 0, &Vec2f(0.0f, speed.m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(3, acc, 7, &Vec2f(speed.m_x / stopDiv, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1)).m_gravity = -gravity;
}

static inline void setupMode66(soModuleAccesser* acc, Vec2f speed) {
    soKineticUtility::resetEnableEnergy(1, acc, 0, &Vec2f(0.0f, speed.m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(3, acc, 7, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    float gravity = soValueAccesser::getConstantFloat(acc, 0xfab, 0);
    dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1)).m_gravity = -gravity;
}

static inline void setupMode69(soModuleAccesser* acc, Vec2f speed) {
    soKineticUtility::resetEnableEnergy(1, acc, 0, &Vec2f(0.0f, speed.m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(3, acc, 7, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    float gravity = soValueAccesser::getConstantFloat(acc, 0xfb4, 0);
    dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1)).m_gravity = -gravity;
}

// PSI magnet / PK Thunder style modes 0x64..0x6a; the lower modes go to the common transactor.
// HYPOTHESIS: which statuses use which mode.
void ftLucasKineticTransactor::changeKinetic(int mode, void* pools, soModuleAccesser* acc) {
    if (mode <= 0x63) {
        ftKineticTransactor::changeKinetic(mode, pools, acc);
        return;
    }
    Vec2f speed = ftKineticTransactHelper::preHelpProcess(acc, true, true);
    switch (mode) {
    case 0x64:
        setupMode64(acc, speed);
        break;
    case 0x65:
        setupMode65(acc, speed);
        break;
    case 0x66:
        setupMode66(acc, speed);
        break;
    case 0x67: {
        // Keeps the stop energy's speed magnitude along the entry direction and brakes it with constant 0xfad.
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
        Vec2f stopSpeed = stop.getSpeed();
        float magnitude = stopSpeed.length();
        float dir = speed.m_x < 0.0f ? -1 : 1;
        soKineticUtility::resetEnableEnergy(3, acc, 0x16, &Vec2f(magnitude * dir, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
        float brake = soValueAccesser::getConstantFloat(acc, 0xfad, 0);
        stop.m_accel = Vec2f(brake * acc->getPostureModule().getLr(), 0.0f);
        stop.m_brake = Vec2f(brake, 0.0f);
        stop.m_speedTarget = Vec2f(0.0001f, 0.0001f);
        break;
    }
    case 0x68: {
        bool result = false;
        changeKineticSub(&result, pools, &speed, acc);
        break;
    }
    case 0x69:
        setupMode69(acc, speed);
        break;
    case 0x6a: {
        float fall = soValueAccesser::getConstantFloat(acc, 0xfbc, 0);
        soKineticUtility::resetEnableEnergy(1, acc, 0, &Vec2f(0.0f, -fall), &Vec3f(0.0f, 0.0f, 0.0f));
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
        gravity.m_gravity = 0.0f;
        gravity.unk18 = 0.0f;
        gravity.m_fallSpeedMax = fall;
        gravity.unk1C = fall;
        break;
    }
    default:
        return;
    }
    enableOutsideEnergy(acc);
}

// Mode 0x68: both energies keep their current speed; gravity and fall speed from constants 0xfab/0xfac, and the stop
// energy brakes along its own direction (facing-corrected) by constant 0xfad.
void ftLucasKineticTransactor::changeKineticSub(bool*, void*, Vec2f*, soModuleAccesser* acc) {
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
    Vec2f stopSpeed = stop.getSpeed();
    Vec2f gravitySpeed = gravity.getSpeed();
    soKineticUtility::resetEnableEnergy(1, acc, 0, &gravitySpeed, &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(3, acc, 0xa, &stopSpeed, &Vec3f(0.0f, 0.0f, 0.0f));
    float gravityAccel = soValueAccesser::getConstantFloat(acc, 0xfab, 0);
    float fallSpeedMax = soValueAccesser::getConstantFloat(acc, 0xfac, 0);
    float brake = soValueAccesser::getConstantFloat(acc, 0xfad, 0);
    gravity.m_gravity = -gravityAccel;
    gravity.unk18 = 0.0f;
    gravity.m_fallSpeedMax = fallSpeedMax;
    gravity.unk1C = fallSpeedMax;
    stop.m_accel = Vec2f(0.0f, 0.0f);
    stop.m_speedTarget = Vec2f(0.0001f, 0.0001f);
    stop.m_speedLimit = stopSpeed;
    Vec2f dir = stopSpeed;
    dir.normalize();
    dir.m_x *= acc->getPostureModule().getLr();
    dir *= brake;
    stop.m_brake = dir;
}
