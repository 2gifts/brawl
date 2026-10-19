#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soModuleAccesser;
class wnDiddyGun;

// Diddy Kong's per-fighter helper (the code is in sora_melee): starts the popgun article.
class ftDiddyTransactor : public ftTransactor {
public:
    ftDiddyTransactor();
    virtual ~ftDiddyTransactor();
    // HYPOTHESIS: the id is the effect resource of the fighter.
    bool activeArticle(wnDiddyGun* weapon, soModuleAccesser* acc, u32 etcResId);
};
static_assert(sizeof(ftDiddyTransactor) == 4, "Transactor singleton layout is wrong!");
