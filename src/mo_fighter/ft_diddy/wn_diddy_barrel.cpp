#define SO_BUILDER_HELPERS_WEAK
#define IP_INPUT_BUTTON_RAW
#define WN_ARTICLE_ANIMCMD_INT
#define WN_DAMAGE_ACTOR_INTERFACE
#define WN_WEAPON_BUILDER
#define FT_REL_LINK_EXTERN
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#define WN_KINETIC_GENERIC_BUILDER
#include <wn/wn_module_accesser_builder.h>
#include <wn/diddy/wn_diddy_barrel.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <mt/mt_prng.h>

static wnDiddyBarrelStatusUniqProcess g_wnDiddyBarrelStatusUniqProcess;

wnDiddyBarrelStatusUniqProcess* wnDiddyBarrelStatusUniqProcess::getInstance() {
    return &g_wnDiddyBarrelStatusUniqProcess;
}

wnDiddyBarrelStatusUniqProcess::~wnDiddyBarrelStatusUniqProcess() { }

void wnDiddyBarrelStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) {
    soParamAccesser* params = (soParamAccesser*)moduleAccesser->getWorkManageModule().getParamAccesser();
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 1:
        moduleAccesser->getWorkManageModule().onFlag(0x22000000);
        moduleAccesser->getWorkManageModule().setInt(params->getParamInt(moduleAccesser, 0x5DC1, 0), 0x20000002);
        break;
    case 2:
        moduleAccesser->getWorkManageModule().setInt(params->getParamInt(moduleAccesser, 0x5DC4, 0), 0x20000000);
        break;
    }
}

// HYPOTHESIS: the native speed stores expand this member setter.
inline void soKineticEnergyNormal::setSpeed(Vec2f* speed) {
    m_speed.m_x = speed->m_x;
    m_speed.m_y = speed->m_y;
}

void wnDiddyBarrelStatusUniqProcess::execStatus(soModuleAccesser* moduleAccesser) {
    soParamAccesser* params = (soParamAccesser*)moduleAccesser->getWorkManageModule().getParamAccesser();
    moduleAccesser->getWorkManageModule().getInt(0x20000000);
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 1: break;
    default: return;
    }

    soKineticEnergyNormal& normal = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
    wnKineticEnergyGravity& gravity = dynamic_cast<wnKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    float lr = moduleAccesser->getPostureModule().getLr();

    moduleAccesser->getWorkManageModule().addInt(-1, 0x20000001);
    int frame = moduleAccesser->getWorkManageModule().getInt(0x20000001);
    if (frame <= 0 || moduleAccesser->getWorkManageModule().isFlag(0x22000000)) {
        float randomAngle = 360.0f * randf();
        moduleAccesser->getWorkManageModule().setFloat(randomAngle, 0x21000001);
        int delay = params->getParamInt(moduleAccesser, 0x5DC2, 0);
        int randomDelay = params->getParamInt(moduleAccesser, 0x5DC3, 0);
        if (randomDelay > 0) {
            randomDelay = params->getParamInt(moduleAccesser, 0x5DC3, 0);
            delay += randi(randomDelay);
        }
        moduleAccesser->getWorkManageModule().setInt(delay, 0x20000001);
        moduleAccesser->getWorkManageModule().offFlag(0x22000000);
    }

    Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    Vec2f direction = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    direction.normalize();
    float angle = 57.29578f * atan2f(direction.m_x, direction.m_y);
    if (angle < 0.0f) angle = 360.0f + angle;

    float targetAngle = moduleAccesser->getWorkManageModule().getFloat(0x21000001);
    float delta = targetAngle - angle;
    float turnDirection;
    float distance = __fabsf(delta);
    if (delta > 0.0f) {
        if (delta - 180.0f < 0.0f) {
            turnDirection = 1.0f;
        } else {
            turnDirection = -1.0f;
        }
    } else {
        if (distance - 180.0f < 0.0f) {
            turnDirection = -1.0f;
        } else {
            turnDirection = 1.0f;
        }
    }

    float step = distance;
    if (lr >= 0.0f) {
        if (turnDirection > 0.0f) {
            if (distance > params->getParamFloat(moduleAccesser, 0xFA3, 0))
                step = params->getParamFloat(moduleAccesser, 0xFA3, 0);
        } else {
            if (distance > params->getParamFloat(moduleAccesser, 0xFA2, 0))
                step = params->getParamFloat(moduleAccesser, 0xFA2, 0);
        }
    } else {
        if (turnDirection > 0.0f) {
            if (distance > params->getParamFloat(moduleAccesser, 0xFA2, 0))
                step = params->getParamFloat(moduleAccesser, 0xFA2, 0);
        } else {
            if (distance > params->getParamFloat(moduleAccesser, 0xFA3, 0))
                step = params->getParamFloat(moduleAccesser, 0xFA3, 0);
        }
    }
    if (step == distance) moduleAccesser->getWorkManageModule().onFlag(0x22000000);

    step *= turnDirection;
    angle += step;
    if (angle > 360.0f) angle -= 360.0f;

    float magnitude = speed.length();
    if (moduleAccesser->getWorkManageModule().getInt(0x20000002) > 0) {
        moduleAccesser->getWorkManageModule().addInt(-1, 0x20000002);
        float minimum = moduleAccesser->getWorkManageModule().getFloat(0x21000002);
        if (magnitude < minimum) magnitude = moduleAccesser->getWorkManageModule().getFloat(0x21000002);
    }

    Vec2f rotated(0.0f, magnitude);
    rotated.rot(0.01745329238f * (360.0f - angle), &rotated);
    normal.setSpeed(&Vec2f(rotated.m_x, 0.0f));
    gravity.m_speedY = rotated.m_y;

    Vec3f rotation = moduleAccesser->getModelModule().getNodeRotate(3);
    rotation.m_x = angle * lr;
    moduleAccesser->getModelModule().setNodeRotate(3, &rotation);

    float speedMul = params->getParamFloat(moduleAccesser, 0xFA7, 0);
    float speedRatio = magnitude / params->getParamFloat(moduleAccesser, 0xFA6, 0);
    float power = speedRatio * speedMul;
    moduleAccesser->getCollisionAttackModule().setPowerAddStatus(power);
}

void wnDiddyBarrelStatusUniqProcess::execStop(soModuleAccesser* moduleAccesser) {
    Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    speed.normalize();
    float angle = 57.29578f * atan2f(speed.m_x, speed.m_y);
    Vec3f rotation = moduleAccesser->getModelModule().getNodeRotate(3);
    float lr = moduleAccesser->getPostureModule().getLr();
    rotation.m_x = angle * lr;
    moduleAccesser->getModelModule().setNodeRotate(3, &rotation);
}

void wnDiddyBarrelStatusUniqProcess::exitStatus(soModuleAccesser*, int) { }

wnWeaponBuilder<wnDiddyBarrelModuleAccesserBuildConfig>::wnWeaponBuilder(
    const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd) :
    Weapon(name, bd.m_kind, bd.m_data, articleId, &info, m_modules.getModuleAccesser(), true), m_modules(this, bd) {
    Weapon::initialize();
}

wnWeaponBuilder<wnDiddyBarrelModuleAccesserBuildConfig>::~wnWeaponBuilder() { }

void wnWeaponBuilder<wnDiddyBarrelModuleAccesserBuildConfig>::deactivateDescendantForce() {
    soSingletonHolder<soArticleMediatorNull>::getInstance()->deactivate();
}

template <>
void wnKineticTransactor::updateEnergy<soKineticEnergyGroundMovement>(
    soKineticEnergyGroundMovement* energy, soModuleAccesser* acc) {
    if (energy->isEnable() != true) return;
    if (energy->isSuspend()) return;
    energy->updateEnergy(acc);
}


template <>
void wnKineticTransactor::updateEnergy<soKineticEnergyWindNormal>(soKineticEnergyWindNormal* energy, soModuleAccesser* acc) {
    if (energy->isEnable() != true) return;
    if (energy->isSuspend()) return;
    energy->updateEnergy(acc);
}

wnDiddyBarrel::wnDiddyBarrel(s32 articleId, const wnArticleBuildInfo& info, const wnDiddyBarrelData* data) :
    wnWeaponBuilder<wnDiddyBarrelModuleAccesserBuildConfig>(
        "DiddyBarrel", articleId, info,
        wnModuleAccesserBuildData(0x37, data, info.getHeapModule()->getInstanceHeap(),
                                  info.getHeapModule()->getNWModelInstanceHeap(), info.getHeapModule()->getNWMotionInstanceHeap(),
                                  data->m_resGroupNo, -1, &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_data(data), m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[3];
    uniqProcs[0] = wnDiddyBarrelStatusUniqProcess::getInstance();
    uniqProcs[1] = wnDiddyBarrelStatusUniqProcess::getInstance();
    uniqProcs[2] = wnDiddyBarrelStatusUniqProcess::getInstance();
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 3);
    m_moduleAccesser->getCollisionHitModule().add((soSet<soCollisionHitData>*)&data->m_hitDataStorage, 0);
    Weapon::deactivate(false);
}

void wnDiddyBarrel::onDeactivate() { }

void wnDiddyBarrel::processUpdate() {
    m_moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x11000001);
    Weapon::processUpdate();
}

bool wnDiddyBarrel::notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* acc, int index) {
    return Weapon::notifyEventAnimCmd(cmd, acc, index);
}

void wnDiddyBarrel::notifyEventCollisionAttack(float power, soCollisionLog* collisionLog,
                                                soModuleAccesser* moduleAccesser) {
    float minPower = m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    if (!(minPower >= power)) {
        m_moduleAccesser->getWorkManageModule().setFloat(power, 0x11000001);
    }
    Weapon::notifyEventCollisionAttack(power, collisionLog, moduleAccesser);
}

bool wnDiddyBarrel::notifyEventCollisionAttackCheck(u32 flags) {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 1) {
        if ((flags & 3) == 0 && (flags & 0x10) == 0) {
            if (flags & 4) {
                if (unkA4Bit7) return hop();
            } else if (flags & 8) {
                soReflectModule& reflectModule = m_moduleAccesser->getReflectModule();
                if (reflectModule.isCountMax()) {
                    m_moduleAccesser->getPostureModule().reverseLr();
                    m_moduleAccesser->getPostureModule().initRotYLr();
                    int team = reflectModule.getTeam();
                    soTeamModule& teamModule = m_moduleAccesser->getTeamModule();
                    teamModule.setTeam(team, true);
                    teamModule.setHitTeam(team);
                    teamModule.setTeamOwnerId(reflectModule.getTaskId());
                    return reflect();
                }
            }
        }
    }

    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    float hitStop = work.getFloat(0x11000001);
    if (!(hitStop <= 0.0f)) {
        hitStop = work.getFloat(0x11000001);
        setHitStop(hitStop, 1.0f, 0);
    }
    return false;
}

bool wnDiddyBarrel::isCanEat() {
    return m_moduleAccesser->getStatusModule().getStatusKind() != 2;
}

void wnDiddyBarrel::activate(int founderTaskId, int unk1, int team, Vec3f* pos, Vec2f* speed, bool unk5,
                             float lr, float rot) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = desc.unk8 = desc.unkC = unk1;
    desc.unk10 = desc.unk14 = -1;
    desc.unk18 = desc.unk1C = 0;
    desc.pos = *pos;
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);

    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    work.setFloat(rot, 0x21000000);
    work.setFloat((float)m_param->unk0, 0x21000003);
    if (unk5) {
        m_moduleAccesser->getStatusModule().changeStatusForce(2, m_moduleAccesser);
    } else {
        work.setFloat(m_param->unk4 + rot * m_param->unk8, 0x21000002);
        work.setFloat(speed->m_x, 0x21000004);
        work.setFloat(speed->m_y, 0x21000005);
        m_moduleAccesser->getStatusModule().changeStatusForce(1, m_moduleAccesser);
    }
}

void wnDiddyBarrel::toShoot(Vec3f* pos, float lr, Vec2f* speed, float value, float, bool unk0) {
    m_moduleAccesser->getPostureModule().init(pos, lr);
    m_moduleAccesser->getLinkModule().removeModelConstraint(true);
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    work.setFloat(value, 0x21000000);
    if (unk0) {
        m_moduleAccesser->getStatusModule().changeStatusForce(2, m_moduleAccesser);
    } else {
        work.setFloat(speed->m_x, 0x21000004);
        work.setFloat(speed->m_y, 0x21000005);
        m_moduleAccesser->getStatusModule().changeStatusForce(1, m_moduleAccesser);
    }
}
