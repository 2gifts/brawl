#define SO_KINETIC_LOCAL_NORMAL_IMPL
#include <wn/snake/so_kinetic_energy_local_normal.h>

static inline Vec2f normalSpeed(const soKineticEnergyNormal* energy) {
    return energy->m_speed;
}

Vec2f soKineticEnergyLocalNormal::getSpeed() const {
    Vec2f speed = normalSpeed(this);
    speed.rot(0.017453292f * unk34, &speed);
    return speed;
}

soKineticEnergyLocalNormal::~soKineticEnergyLocalNormal() { }
