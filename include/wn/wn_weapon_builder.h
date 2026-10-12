#pragma once

// Local BrawlHeaders shadow: Weapon ends at 0xCC; keep the unresolved default
// builder's established total extent until its configuration is reconstructed.
#include <StaticAssert.h>
#include <types.h>
#include <wn/weapon.h>

#ifdef WN_WEAPON_BUILDER
// Article units: the generic builder (wnWeaponBuilder<BC> over the article's build config).
#include <wn/wn_module_accesser_builder.h>
#else
template <typename T>
class wnWeaponBuilder : public Weapon {
public:
    char unkCC[0x2160 - sizeof(Weapon)];
    // TODO: virtual functions
};
static_assert(sizeof(wnWeaponBuilder<void>) == 0x2160, "Class is wrong size!");
#endif
