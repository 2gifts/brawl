#pragma once

#include <so/kinetic/so_kinetic_energy.h>

// The null kinetic energy. sora_melee has its RTTI ("soKineticEnergyNull", base soKineticEnergy at +0) and its vtable
// (0x7FD58..0x7FDF0): the destructor and every pure function of the base are inline here, the 3D speed and the
// ground-friction switches stay the base's. No unit builds one yet; the class is all inline, so a unit that includes this
// header emits the base's inline virtual functions at this class's place in the link order.
class soKineticEnergyNull : public soKineticEnergy {
public:
    // HYPOTHESIS: the constructor (no caller decompiled yet); the base takes the null flag.
    soKineticEnergyNull() : soKineticEnergy(true) { }
    virtual void updateEnergy(soModuleAccesser*) { }
    virtual Vec2f getSpeed() const { return Vec2f(0.0f, 0.0f); }
    virtual Vec3f getRotation() const { return Vec3f(0.0f, 0.0f, 0.0f); }
    virtual void resetEnergy(int, const Vec2f&, const Vec3f&, soModuleAccesser*) { }
    virtual void clearSpeed() { }
    virtual void clearRotSpeed() { }
    virtual void mulSpeed(Vec3f*) { }
    virtual void mulAccel(Vec3f*) { }
    virtual void reflectSpeed(const Vec3f&) { }
    virtual void reflectAccel(const Vec3f&) { }
    virtual ~soKineticEnergyNull() { }
};
