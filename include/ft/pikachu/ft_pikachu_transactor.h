#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soModuleAccesser;
class wnPikachuThunderJolt;

// Pikachu's per-fighter helper (the code is in sora_melee): starts the Thunder Jolt article.
class ftPikachuTransactor : public ftTransactor {
public:
    ftPikachuTransactor();
    virtual ~ftPikachuTransactor();
    // HYPOTHESIS: the id is the effect resource of the fighter.
    bool activeArticle(wnPikachuThunderJolt* weapon, soModuleAccesser* acc, u32 etcResId);
};
static_assert(sizeof(ftPikachuTransactor) == 4, "Transactor singleton layout is wrong!");
