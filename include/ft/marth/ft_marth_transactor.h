#pragma once

#include <ft/kirby/ft_kirby_copy_ability_process.h>
#include <types.h>

class soArticle;
class soModuleAccesser;

// Marth's per-fighter helper (the code is in sora_melee; its vtable and RTTI name ftKirbyCopyAbilityProcess as the base): starts the sword simple article of Kirby's copy.
class ftMarthTransactor : public ftKirbyCopyAbilityProcess {
public:
    ftMarthTransactor();
    virtual ~ftMarthTransactor();
    virtual bool activeArticle(soArticle* article, soModuleAccesser* acc);
};
static_assert(sizeof(ftMarthTransactor) == 4, "Transactor singleton layout is wrong!");
