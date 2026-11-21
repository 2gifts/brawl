#define SO_KINETIC_CONTACT_IMPLEMENTATION
#include <ft/ft_kinetic_energy.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>

soKineticEnergyJostle::soKineticEnergyJostle() : m_speed(0.0f, 0.0f, 0.0f) { }

void soKineticEnergyJostle::updateEnergy(soModuleAccesser* acc) {
    if (acc->getJostleModule().isNull() == true) {
        m_speed.m_x = 0.0f;
        m_speed.m_y = 0.0f;
        m_speed.m_z = 0.0f;
    } else {
        Vec2f push(acc->getJostleModule().getSpeedX(), 0.0f);
        // Use the supporting floor's normal to make separation follow its slope.
        if (push.lengthSq() != 0.0f && acc->getSituationModule().getKind() == Situation_Ground &&
            acc->getGroundModule().isTouch(grCollStatus::TouchMask(8), 0)) {
            Vec2f normal = acc->getGroundModule().getTouchNormal(8, 0);
            push = soKineticUtility::projectionNormalFollow(&push, &normal);
        }
        Vec3f position = acc->getPostureModule().getPos();
        m_speed.m_x = push.m_x;
        m_speed.m_y = push.m_y;
        soGroundModule& ground = acc->getGroundModule();
        float depth = acc->getJostleModule().getPosZ();
        float groundZ = ground.getZ();
        m_speed.m_z = groundZ + depth - position.m_z;
    }
}

// Jostling is recomputed from collision separation every frame.
void soKineticEnergyJostle::reflectAccel(const Vec3f&) { }
void soKineticEnergyJostle::reflectSpeed(const Vec3f&) { }
void soKineticEnergyJostle::mulAccel(Vec3f*) { }
void soKineticEnergyJostle::mulSpeed(Vec3f*) { }
void soKineticEnergyJostle::clearRotSpeed() { }
void soKineticEnergyJostle::clearSpeed() {
    m_speed.m_x = 0.0f;
    m_speed.m_y = 0.0f;
    m_speed.m_z = 0.0f;
}
void soKineticEnergyJostle::resetEnergy(int, const Vec2f&, const Vec3f&, soModuleAccesser*) { }
Vec3f soKineticEnergyJostle::getRotation() const { return Vec3f(0.0f, 0.0f, 0.0f); }
Vec3f soKineticEnergyJostle::getSpeed3f() const { return m_speed; }
Vec2f soKineticEnergyJostle::getSpeed() const { return Vec2f(m_speed.m_x, m_speed.m_y); }
soKineticEnergyJostle::~soKineticEnergyJostle() { }
