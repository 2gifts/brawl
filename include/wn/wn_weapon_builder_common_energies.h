#pragma once

// wnWeaponBuilder<BC> with the two common movement energies, spelled out for one build config. For a fighter unit whose
// other articles are built without them (WN_WEAPON_NO_COMMON_ENERGIES selects that layout for the whole unit, see
// wn/wn_module_accesser_builder.h) and that also owns an article with them: only the layout and the teardown are used
// there, the article's own unit builds it from the generic template.
#include <wn/wn_module_accesser_builder.h>

#define WN_WEAPON_BUILDER_COMMON_ENERGIES(BC)                                                                        template <>                                                                                                      class wnWeaponBuilder<BC> : public Weapon {                                                                      protected:                                                                                                           wnModuleAccesserBuilder<BC> m_modules;                                                                           wnKineticEnergyNormal m_energyNormal;       /* the main movement energy (index 0) */                             soKineticEnergyRotNormal m_energyRotNormal; /* the rotation energy (index 1) */                              public:                                                                                                              virtual ~wnWeaponBuilder() { }                                                                                   virtual void deactivateDescendantForce() { wnDeactivateArticleDescendants(this->m_modules.m_generateArticleBuilder); }     }
