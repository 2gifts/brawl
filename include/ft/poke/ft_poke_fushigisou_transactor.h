#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soArticle;
class soModuleAccesser;
class wnPokeFushigisouSeed;

// Fushigisou's per-fighter helper (the code is in sora_melee; the class name is its RTTI string there).
class ftPokeFushigisouTransactor : public ftTransactor {
public:
    ftPokeFushigisouTransactor();
    virtual ~ftPokeFushigisouTransactor();
    virtual bool activeArticle(soArticle* article, soModuleAccesser* acc);
    // HYPOTHESIS: kind 0 from the fighter, 1 for Kirby's copy.
    bool activeArticle1(wnPokeFushigisouSeed* weapon, soModuleAccesser* acc, int kind);
};
static_assert(sizeof(ftPokeFushigisouTransactor) == 4, "Transactor singleton layout is wrong!");
