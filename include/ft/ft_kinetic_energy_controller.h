#pragma once

#include <so/so_kinetic_energy_normal.h>
#include <types.h>

// Energy driven by the controller stick (walk/run/air drift etc.). Member meanings are
// HYPOTHESES from resetEnergy/updateEnergy.
class ftKineticEnergyController : public soKineticEnergyNormal {
public:
    // The mode argument of resetEnergy (m_mode). Status code that picks the mode in a local of this type keeps it in a
    // saved register until the call: an enum local is not constant-propagated. Only the values that are held that way
    // are listed; their names are not known, so they carry the number.
    enum ResetType {
        ResetType_0 = 0,   // walk-style setup from the common constants 0xbd3-0xbd8 (resetEnergy case 0)
        ResetType_11 = 0xb
    };

    ftKineticEnergyController();
    virtual void updateEnergy(soModuleAccesser* moduleAccesser);
    virtual void resetEnergy(int mode, Vec2f* speed, Vec3f*, soModuleAccesser* moduleAccesser);
    virtual ~ftKineticEnergyController();

    void mulXSpeedMax(float mul);
    void mulXAccelMul(float mul);

    int m_mode;     // +0x34
    float m_unk38;  // +0x38
    float m_accelMul; // +0x3c
    float m_unk40;  // +0x40
    float m_unk44;  // +0x44
    float m_unk48;  // +0x48
};
