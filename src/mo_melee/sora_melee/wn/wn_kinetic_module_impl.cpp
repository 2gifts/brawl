#define WN_ARTICLE_NATIVE_BUILDERS
#include <mt/mt_vector.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <wn/wn_kinetic_module_impl.h>
#include <types.h>

// Kinetic classes of the generic article (Weapon): the planar energy the shots fly with, the vertical-only energy of
// the falling articles, and the module that owns the two energies and switches between their modes.

wnKineticEnergyNormal::wnKineticEnergyNormal() : unk34(-1) {
}

wnKineticEnergyNormal::~wnKineticEnergyNormal() {
}

void wnKineticEnergyNormal::resetEnergy(int mode, Vec2f* speed, Vec3f*, soModuleAccesser*) {
    Vec2f limit(-1.0f, -1.0f);
    Vec2f brake(0.0f, 0.0f);
    Vec2f target(-1.0f, -1.0f);
    Vec2f accel(0.0f, 0.0f);
    Vec2f::copy(m_accel, accel);
    Vec2f::copy(m_speedTarget, target);
    Vec2f::copy(m_brake, brake);
    Vec2f::copy(m_speedLimit, limit);
    m_speed = *speed;
    unk34 = mode;
    if (mode == 1) {
        Vec2f zero(0.0f, 0.0f);
        Vec2f::copy(m_speedTarget, zero);
    }
}

void wnKineticEnergyNormal::updateEnergy(soModuleAccesser* acc) {
    if (unk34 == 1) {
        float speedLen = getSpeed().length();
        if (speedLen > 0.00001f) {
            float brakeLen = m_brake.length();
            float rest = speedLen - brakeLen;
            if (rest >= 0.0f) {
                Vec2f speed = getSpeed();
                float scale = rest / speedLen - 1.0f;
                m_accel.m_x = speed.m_x * scale;
                m_accel.m_y = speed.m_y * scale;
            } else {
                m_accel.m_x = 0.0f;
                m_accel.m_y = 0.0f;
            }
        } else {
            m_accel.m_x = 0.0f;
            m_accel.m_y = 0.0f;
        }
    }
    soKineticEnergyNormal::updateEnergy(acc);
}

wnKineticModuleImpl::wnKineticModuleImpl(soModuleAccesser* acc, soInstanceManagerFullProperty<soKineticEnergy*>* energies, soKineticMediator* mediator) :
    soKineticModuleImpl(acc, energies, mediator),
    soStatusEventObserver(acc->getEventManageModule().getManageId(), -1),
    m_outsideGroundEnergyId(-1) {
}

// The kinetic mode a status asks for sits in a signed byte of the status data.
struct wnStatusKineticData {
    u8 pad[0xC];
    u32 unk0 : 11;
    s32 kineticType : 8;
    u32 unk19 : 13;
};

void wnKineticModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* data, soModuleAccesser* acc) {
    int kineticType = ((wnStatusKineticData*)data)->kineticType;
    if (kineticType >= 0) {
        changeKinetic(kineticType, acc);
    }
}

typedef void (wnKineticModuleImpl::*wnKineticInitFunc)(int, soModuleAccesser*);
static const wnKineticInitFunc sKineticInit[3] = {
    &wnKineticModuleImpl::initializeNormal,
    &wnKineticModuleImpl::initializeStop,
    &wnKineticModuleImpl::initializeReset,
};

void wnKineticModuleImpl::changeKinetic(int kineticType, soModuleAccesser* acc) {
    if (kineticType >= 0 && kineticType < 3) {
        (this->*sKineticInit[kineticType])(kineticType, acc);
    }
}

void wnKineticModuleImpl::initializeNormal(int kineticType, soModuleAccesser* acc) {
    soInstanceManagerFullProperty<soKineticEnergy*>* manager = m_kineticEnergyInstanceManager;
    soKineticEnergy* main = manager->at(0);
    soKineticEnergy* sub = manager->at(1);
    if (kineticType == 0) {
        Vec2f speed = acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
        Vec3f zero3(0.0f, 0.0f, 0.0f);
        Vec2f copy = speed;
        main->resetEnergy(0, &copy, &zero3, acc);
        Vec3f zeroRot(0.0f, 0.0f, 0.0f);
        Vec2f zero(0.0f, 0.0f);
        sub->resetEnergy(-1, &zero, &zeroRot, acc);
    }
    main->enable();
    sub->enable();
}

void wnKineticModuleImpl::initializeStop(int kineticType, soModuleAccesser* acc) {
    soInstanceManagerFullProperty<soKineticEnergy*>* manager = m_kineticEnergyInstanceManager;
    soKineticEnergy* main = manager->at(0);
    soKineticEnergy* sub = manager->at(1);
    if (kineticType == 0) {
        Vec2f speed = acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
        Vec3f zero3(0.0f, 0.0f, 0.0f);
        Vec2f copy = speed;
        main->resetEnergy(1, &copy, &zero3, acc);
        Vec3f zeroRot(0.0f, 0.0f, 0.0f);
        Vec2f zero(0.0f, 0.0f);
        sub->resetEnergy(-1, &zero, &zeroRot, acc);
    }
    main->enable();
    sub->enable();
}

void wnKineticModuleImpl::initializeReset(int, soModuleAccesser*) {
    soInstanceManagerFullProperty<soKineticEnergy*>* manager = m_kineticEnergyInstanceManager;
    soKineticEnergy* main = manager->at(0);
    main->clearSpeed();
    main->disable();
    soKineticEnergy* sub = manager->at(1);
    sub->clearSpeed();
    sub->disable();
}

void wnKineticModuleImpl::setOutsideMovementGroundEnergyId(soKineticModule* module, int energyId) {
    wnKineticModuleImpl* impl = dynamic_cast<wnKineticModuleImpl*>(module);
    if (impl != NULL) {
        impl->m_outsideGroundEnergyId = energyId;
    }
}

void wnKineticModuleImpl::addSpeedOutside(soKineticEnergy::OutsideType type, Vec3f* speed) {
    if (type == soKineticEnergy::Outside_Movement_Ground && m_outsideGroundEnergyId != -1) {
        soKineticUtility::addSpeedOutsideMovementGround(getEnergy(m_outsideGroundEnergyId), speed, m_moduleAccesser);
    }
}

void wnKineticModuleImpl::addSpeed(Vec3f*, soModuleAccesser*) {
}

void wnKineticEnergyGravity::resetEnergy(int, Vec2f* speed, Vec3f*, soModuleAccesser*) {
    m_speedY = speed->m_y;
}

void wnKineticEnergyGravity::updateEnergy(soModuleAccesser*) {
    m_speedY += m_gravity;
    if (m_speedY < 0.0f && m_speedLimit > 0.0f && __fabsf(m_speedLimit) < __fabsf(m_speedY)) {
        m_speedY = soKineticUtility::limitSpeed(m_speedY, m_speedLimit);
    }
}

// Reflects v about the normal n (out = v - 2 n (n . v)).
static inline void reflectVec(Vec2f& out, const Vec2f& v, const Vec2f& n) {
    out = v - n * 2.0f * (n.m_x * v.m_x + n.m_y * v.m_y);
}

void wnKineticEnergyGravity::reflectSpeed(Vec3f* normal) {
    Vec2f n = *normal->xy();
    Vec2f v(0.0f, m_speedY);
    reflectVec(v, v, n);
    m_speedY = v.m_y;
}

void wnKineticEnergyGravity::reflectAccel(Vec3f* normal) {
    Vec2f n = *normal->xy();
    Vec2f v(0.0f, m_gravity);
    reflectVec(v, v, n);
    m_gravity = v.m_y;
}

void wnKineticEnergyGravity::mulAccel(Vec3f* mul) {
    m_gravity *= mul->m_y;
}

void wnKineticEnergyGravity::mulSpeed(Vec3f* mul) {
    m_speedY *= mul->m_y;
}

void wnKineticEnergyGravity::clearRotSpeed() {
}

void wnKineticEnergyGravity::clearSpeed() {
    m_speedY = 0.0f;
}

Vec3f wnKineticEnergyGravity::getRotation() {
    return Vec3f(0.0f, 0.0f, 0.0f);
}

Vec2f wnKineticEnergyGravity::getSpeed() {
    return Vec2f(0.0f, m_speedY);
}

wnKineticEnergyGravity::~wnKineticEnergyGravity() {
}
