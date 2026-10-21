#define WN_KINETIC_GENERIC_BUILDER
#include <types.h>
#include <ft/builder/ft_builder_kinetic.h>
#include <so/so_kinetic_energy_normal.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

// The energies of the follower's kinetic mediator (the list of the Diddy Kong barrel).
typedef soInstancePoolInfo<soKineticEnergyNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<0, 1> > wnDededeFollowerEnergy0Info;
typedef soInstancePoolInfo<wnKineticEnergyGravity, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<1, 1> > wnDededeFollowerEnergy1Info;
typedef soInstancePoolInfo<soKineticEnergyNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<2, 2> > wnDededeFollowerEnergy2Info;
typedef soInstancePoolInfo<soKineticEnergyGroundMovement, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<3, 8> > wnDededeFollowerEnergy3Info;
typedef soInstancePoolInfo<soKineticEnergyWindNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<4, 4> > wnDededeFollowerEnergy4Info;

// The energy pools, innermost first; their destructors call each other out of line.
#pragma dont_inline on
template class soKineticEnergyHolder<soKineticEnergyNormal, soTypeListNullType, soKineticEnergyInitInfo<0, 1> >;
template class soInstancePoolSub<wnDededeFollowerEnergy0Info, soTypeListNullType>;
template class soInstancePool<wnDededeFollowerEnergy0Info, soInstancePoolRoot, soTypeListNullType, 0>;
template class soLineInvertHierarchy<soTypeList<wnDededeFollowerEnergy0Info, soTypeListNullType>, soInstancePool, soInstancePoolRoot>;
template class soKineticEnergyHolder<wnKineticEnergyGravity, soTypeListNullType, soKineticEnergyInitInfo<1, 1> >;
template class soInstancePoolSub<wnDededeFollowerEnergy1Info, soTypeListNullType>;
template class soInstancePool<wnDededeFollowerEnergy1Info, soLineInvertHierarchy<soTypeList<wnDededeFollowerEnergy0Info, soTypeListNullType>, soInstancePool, soInstancePoolRoot>, soTypeListNullType, 1>;
template class soLineInvertHierarchy<soTypeList<wnDededeFollowerEnergy0Info, soTypeList<wnDededeFollowerEnergy1Info, soTypeListNullType> >, soInstancePool, soInstancePoolRoot>;
template class soKineticEnergyHolder<soKineticEnergyNormal, soTypeListNullType, soKineticEnergyInitInfo<2, 2> >;
template class soInstancePoolSub<wnDededeFollowerEnergy2Info, soTypeListNullType>;
template class soInstancePool<wnDededeFollowerEnergy2Info, soLineInvertHierarchy<soTypeList<wnDededeFollowerEnergy0Info, soTypeList<wnDededeFollowerEnergy1Info, soTypeListNullType> >, soInstancePool, soInstancePoolRoot>, soTypeListNullType, 2>;
template class soLineInvertHierarchy<soTypeList<wnDededeFollowerEnergy0Info, soTypeList<wnDededeFollowerEnergy1Info, soTypeList<wnDededeFollowerEnergy2Info, soTypeListNullType> > >, soInstancePool, soInstancePoolRoot>;
template class soKineticEnergyHolder<soKineticEnergyGroundMovement, soTypeListNullType, soKineticEnergyInitInfo<3, 8> >;
template class soInstancePoolSub<wnDededeFollowerEnergy3Info, soTypeListNullType>;
template class soInstancePool<wnDededeFollowerEnergy3Info, soLineInvertHierarchy<soTypeList<wnDededeFollowerEnergy0Info, soTypeList<wnDededeFollowerEnergy1Info, soTypeList<wnDededeFollowerEnergy2Info, soTypeListNullType> > >, soInstancePool, soInstancePoolRoot>, soTypeListNullType, 3>;
template class soLineInvertHierarchy<soTypeList<wnDededeFollowerEnergy0Info, soTypeList<wnDededeFollowerEnergy1Info, soTypeList<wnDededeFollowerEnergy2Info, soTypeList<wnDededeFollowerEnergy3Info, soTypeListNullType> > > >, soInstancePool, soInstancePoolRoot>;
template class soKineticEnergyHolder<soKineticEnergyWindNormal, soTypeListNullType, soKineticEnergyInitInfo<4, 4> >;
template class soInstancePoolSub<wnDededeFollowerEnergy4Info, soTypeListNullType>;
template class soInstancePool<wnDededeFollowerEnergy4Info, soLineInvertHierarchy<soTypeList<wnDededeFollowerEnergy0Info, soTypeList<wnDededeFollowerEnergy1Info, soTypeList<wnDededeFollowerEnergy2Info, soTypeList<wnDededeFollowerEnergy3Info, soTypeListNullType> > > >, soInstancePool, soInstancePoolRoot>, soTypeListNullType, 4>;
template class soLineInvertHierarchy<soTypeList<wnDededeFollowerEnergy0Info, soTypeList<wnDededeFollowerEnergy1Info, soTypeList<wnDededeFollowerEnergy2Info, soTypeList<wnDededeFollowerEnergy3Info, soTypeList<wnDededeFollowerEnergy4Info, soTypeListNullType> > > > >, soInstancePool, soInstancePoolRoot>;
#pragma dont_inline off
