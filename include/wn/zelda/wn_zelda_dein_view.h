#pragma once

// Local views of the Zelda Dein article and its kinetic energy, shared by the Dein status units in this
// directory. The article class is not reconstructed yet; only the parameter pointer at +0x207C and the
// energy class used by the Tame and Move statuses are named. HYPOTHESIS: the energy view is a Normal energy
// whose m_speed (+0x08) is written by those statuses.

#include <wn/weapon.h>
#include <so/so_kinetic_energy_normal.h>
#include <types.h>

// HYPOTHESIS: parameter floats are named by their use in the Tame and Move statuses (offsets verified).
struct wnZeldaDeinParam {
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    float unk10;
    float unk14;
    float unk18;
    float unk1C;
    float unk20;
    float unk24;
    float unk28;
    float unk2C;
};

class wnZeldaDein : public Weapon {
    u8 unkA8[0x207C - sizeof(Weapon)];
public:
    wnZeldaDeinParam* m_param;
};

class wnZeldaDeinKineticEnergyView : public soKineticEnergyNormal {
};
