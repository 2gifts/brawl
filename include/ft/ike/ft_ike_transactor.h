#pragma once

#include <ft/kirby/ft_kirby_copy_ability_process.h>
#include <types.h>

class soArticle;
class soModuleAccesser;

// Ike's per-fighter helper (the code is in sora_melee; its vtable and RTTI name ftKirbyCopyAbilityProcess as the base): starts the sword simple article of Kirby's copy.
class ftIkeTransactor : public ftKirbyCopyAbilityProcess {
public:
    ftIkeTransactor();
    virtual ~ftIkeTransactor();
    virtual bool activeArticle(soArticle* article, soModuleAccesser* acc);
};
static_assert(sizeof(ftIkeTransactor) == 4, "Transactor singleton layout is wrong!");
