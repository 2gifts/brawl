#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <ft/ft_common_data_accesser.h>
#include <wn/lucario/wn_lucario_aura_ball.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <mt/mt_prng.h>
#include <math.h>

// HYPOTHESIS: the Lucario aura ball charge record of the fighter data (only the read fields).
struct ftLucarioAuraBallParam {
    s32 m_chargeLevelNum; // +0x0
    u8 unk4[0xC];
    s32 m_chargeLevelFrame; // +0x10
    float m_startPowerMul;  // +0x14
};

// The charge-record lookup helper over the module accesser.
static inline const ftLucarioAuraBallParam* getAuraBallParam(soModuleAccesser* acc) {
    return g_ftCommonDataAccesser.getLucarioAuraBallParam(acc->getWorkManageModule().getInt(0x10000009));
}

// The three statuses of the ball: held and charging (Start/Charge), then flying (Shoot).
class wnLucarioAuraBallStatusUniqProcessStart : public soStatusUniqProcess {
public:
    virtual ~wnLucarioAuraBallStatusUniqProcessStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class wnLucarioAuraBallStatusUniqProcessCharge : public soStatusUniqProcess {
public:
    virtual ~wnLucarioAuraBallStatusUniqProcessCharge() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class wnLucarioAuraBallStatusUniqProcessShoot : public soStatusUniqProcess {
public:
    virtual ~wnLucarioAuraBallStatusUniqProcessShoot() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    static void initShoot(soModuleAccesser* acc) __attribute__((never_inline));
    static void updateShoot(soModuleAccesser* acc) __attribute__((never_inline));
    static void applyShoot(soModuleAccesser* acc) __attribute__((never_inline));
};

// The attack power follows the charge scale.
static inline void applyPowerMul(soModuleAccesser* acc) {
    const ftLucarioAuraBallParam* param = getAuraBallParam(acc);
    float charge = acc->getWorkManageModule().getFloat(0x11000009);
    acc->getCollisionAttackModule().setPowerMul(charge * param->m_startPowerMul);
}

void wnLucarioAuraBallStatusUniqProcessStart::initStatus(soModuleAccesser* acc) {
    wnLucarioAuraBall& ball = dynamic_cast<wnLucarioAuraBall&>(*acc->m_stageObject);
    soWorkManageModule& work = acc->getWorkManageModule();
    work.setInt(ball.getChargeLevel(), 0x10000006);
}

void wnLucarioAuraBallStatusUniqProcessStart::execStatus(soModuleAccesser* acc) {
    applyPowerMul(acc);
}

void wnLucarioAuraBallStatusUniqProcessCharge::execStatus(soModuleAccesser* acc) {
    soWorkManageModule& work = acc->getWorkManageModule();
    const ftLucarioAuraBallParam* param = g_ftCommonDataAccesser.getLucarioAuraBallParam(work.getInt(0x10000009));
    if (dynamic_cast<wnLucarioAuraBall&>(*acc->m_stageObject).getChargeLevel() < param->m_chargeLevelNum) {
        work.incInt(0x10000005);
        wnLucarioAuraBall& ball = dynamic_cast<wnLucarioAuraBall&>(*acc->m_stageObject);
        acc->getWorkManageModule().setInt(ball.getChargeLevel(), 0x10000006);
        wnLucarioAuraBall::setEffectScale(acc);
    }
    applyPowerMul(acc);
}

void wnLucarioAuraBallStatusUniqProcessShoot::initStatus(soModuleAccesser* acc) {
    initShoot(acc);
}

void wnLucarioAuraBallStatusUniqProcessShoot::execStatus(soModuleAccesser* acc) {
    updateShoot(acc);
    applyPowerMul(acc);
    applyShoot(acc);
}

static inline float absAngle(float value) {
    return fabs(value);
}

// Touching a steep surface from the side ends the flight.
void wnLucarioAuraBallStatusUniqProcessShoot::execFixPos(soModuleAccesser* acc) {
    Vec2f normal(0.0f, 0.0f);
    soGroundModule& ground = acc->getGroundModule();
    if (ground.isTouch(8, 0)) {
        normal = ground.getTouchNormal(8, 0);
    } else if (ground.isTouch(1, 0)) {
        normal = ground.getTouchNormal(1, 0);
    }
    if (normal.length() > 0.00001f) {
        float ny = normal.m_y;
        float nx = normal.m_x;
        float raw = static_cast<float>(atan2(nx, ny));
        double absRaw = fabs(raw);
        float angle = absRaw;
        float limit = 0.017453292f * soValueAccesser::getConstantFloat(acc, 0xfaf, 0);
        float upper = 3.1415927f - limit;
        if (angle > limit && angle < upper) {
            if (normal.m_x * acc->getPostureModule().getLr() < -0.00001f) {
                dynamic_cast<wnLucarioAuraBall&>(*acc->m_stageObject).deactivate(false);
            }
        }
    }
}

// HYPOTHESIS: work floats 0x11000005..8 are the wobble angle, speed, speed change and size; 0x10000007 counts frames.
void wnLucarioAuraBallStatusUniqProcessShoot::initShoot(soModuleAccesser* acc) {
    soWorkManageModule& work = acc->getWorkManageModule();
    work.setInt(soValueAccesser::getConstantInt(acc, 0x5dc1, 0), 0x10000007);
    work.setFloat(1.5707964f, 0x11000005);
    work.setFloat(soValueAccesser::getConstantFloat(acc, 0xfa3, 0), 0x11000006);
    work.setFloat(0.0f, 0x11000007);
    float rate = wnLucarioAuraBall::getChargeRate(acc);
    float grow = 0.5f + 0.5f * rate;
    float zero = 0.0f;
    float shrink = 0.7f + 1.9f * zero;
    float sizeMin = soValueAccesser::getConstantFloat(acc, 0xfa4, 0);
    float sizeMax = soValueAccesser::getConstantFloat(acc, 0xfa5, 0);
    work.setFloat((sizeMin + rate * (sizeMax - sizeMin)) * (grow * shrink), 0x11000008);
}

void wnLucarioAuraBallStatusUniqProcessShoot::updateShoot(soModuleAccesser* acc) {
    soWorkManageModule& work = acc->getWorkManageModule();
    work.incInt(0x10000007);
    int frame = work.getInt(0x10000007);
    int limit = soValueAccesser::getConstantInt(acc, 0x5dc1, 0);
    if (frame >= limit) {
        work.setInt(0, 0x10000007);
        work.addFloat(0.7853982f * (2.0f * randf() - 1.0f), 0x11000005);
        work.setFloat(-work.getFloat(0x11000006) / (0.5f * static_cast<float>(limit)), 0x11000007);
    }
    work.addFloat(work.getFloat(0x11000007), 0x11000006);
    float angle = work.getFloat(0x11000005);
    float speed = work.getFloat(0x11000006);
    float size = work.getFloat(0x11000008);
    float x = size * (speed * static_cast<float>(cos(angle)));
    float y = size * (speed * static_cast<float>(sin(angle)));
    work.addFloat(x, 0x11000003);
    work.addFloat(y, 0x11000004);
}

void wnLucarioAuraBallStatusUniqProcessShoot::applyShoot(soModuleAccesser* acc) {
    soPostureModule& posture = acc->getPostureModule();
    soWorkManageModule& work = acc->getWorkManageModule();
    float scale = work.getFloat(0x1100000A);
    float baseX = work.getFloat(0x11000001);
    float baseY = work.getFloat(0x11000002);
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*acc->getKineticModule().getEnergy(0));
    Vec2f speed = energy.getSpeed();
    speed.m_x *= scale;
    speed.m_y *= scale;
    float x = baseX + speed.m_x;
    float y = baseY + speed.m_y;
    work.setFloat(x, 0x11000001);
    work.setFloat(y, 0x11000002);
    float wobbleX = work.getFloat(0x11000003);
    float wobbleY = work.getFloat(0x11000004);
    Vec3f pos = posture.getPos();
    pos.m_x = x + wobbleX * scale;
    pos.m_y = y + wobbleY * scale;
    posture.setPos(&pos);
}

wnLucarioAuraBallStatusUniqProcessStart g_wnLucarioAuraBallStatusUniqProcessStart;
wnLucarioAuraBallStatusUniqProcessCharge g_wnLucarioAuraBallStatusUniqProcessCharge;
wnLucarioAuraBallStatusUniqProcessShoot g_wnLucarioAuraBallStatusUniqProcessShoot;
