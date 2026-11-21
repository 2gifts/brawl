#pragma once

#include <types.h>

class soModuleAccesser;

// HYPOTHESIS: the transactor of the Ikarus article; the per-mode changeKineticSub helpers run on an empty one-byte object.
class wnPitIkarusKineticTransactor {
    u8 unk0;
public:
    static void changeKinetic(int mode, void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub(void* pools, soModuleAccesser* moduleAccesser) const;
    void changeKineticSub(const void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub(const void* pools, soModuleAccesser* moduleAccesser) const;
};

#ifdef WN_PIT_IKARUS_FULL_BUILDER
// BYTES: the builder holds the full 3f energy (size 0x48); the unit must see it complete before anything else includes it.
#ifndef SO_KINETIC_ENERGY_3F_FULL
#define SO_KINETIC_ENERGY_3F_FULL
#endif
#include <so/kinetic/so_kinetic_energy_3f.h>
#include <so/so_kinetic_energy_rot_normal.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <wn/wn_kinetic_mediator_impl.h>
#include <wn/wn_kinetic_transactor.h>
// The mediator template itself: here it follows the accesser and builder templates (wn_pit_ikarus.h includes them first).
#include <wn/wn_kinetic_mediator.h>

// Four energies: a free 3f energy (mode 0x20), a rotation energy (mode 0x21), a normal and a gravity energy.
// Certain from the map names: the soTypeList / soInstancePoolInfo / soKineticEnergyInitInfo<0..3, ...> chain.
// HYPOTHESIS: the second InitInfo argument of each (cut off in the map names).
typedef soTypeList<soInstancePoolInfo<soKineticEnergy3f, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<0, 1> >,
        soTypeList<soInstancePoolInfo<soKineticEnergyRotNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<1, 1> >,
        soTypeList<soInstancePoolInfo<wnKineticEnergyNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<2, 1> >,
        soTypeList<soInstancePoolInfo<wnKineticEnergyGravity, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<3, 1> >,
        soTypeListNullType> > > > wnPitIkarusKineticEnergyTypeList;
// HYPOTHESIS: transactor info arguments follow the Dolfin / Diddy barrel pattern.
typedef soKineticTransactorInfo<wnPitIkarusKineticTransactor, wnKineticTransactor, wnKineticTransactor, soKineticTransactHelper>
    wnPitIkarusKineticTransactorInfo;
#endif
