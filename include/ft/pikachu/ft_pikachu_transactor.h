#pragma once

#include <ft/ft_transactor.h>
#include <ft/kirby/ft_kirby_copy_ability_process.h>
#include <types.h>

class soArticle;
class soModuleAccesser;
class wnPikachuThunderJolt;

// Pikachu's per-fighter helper (the code is in sora_melee): starts the Thunder Jolt article.
// The sora_melee unit (FT_PIKACHU_TRANSACTOR_COPY_ABILITY_BASE) derives from ftKirbyCopyAbilityProcess like the Yoshi and Wario
// transactors, as the vtable and RTTI in the original show; the fighter module keeps the ftTransactor view.
#ifdef FT_PIKACHU_TRANSACTOR_COPY_ABILITY_BASE
typedef ftKirbyCopyAbilityProcess ftPikachuTransactorBase;
#else
typedef ftTransactor ftPikachuTransactorBase;
#endif
class ftPikachuTransactor : public ftPikachuTransactorBase {
public:
    ftPikachuTransactor();
    virtual ~ftPikachuTransactor();
    virtual bool activeArticle(soArticle* article, soModuleAccesser* acc);
    // HYPOTHESIS: the id is the effect resource of the fighter.
    bool activeArticle(wnPikachuThunderJolt* weapon, soModuleAccesser* acc, u32 etcResId);
};
static_assert(sizeof(ftPikachuTransactor) == 4, "Transactor singleton layout is wrong!");
