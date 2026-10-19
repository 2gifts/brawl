#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soModuleAccesser;
class wnGameWatchSausage;

// Game & Watch's per-fighter helper (the code is in sora_melee): starts the sausage article.
class ftGameWatchTransactor : public ftTransactor {
public:
    ftGameWatchTransactor();
    virtual ~ftGameWatchTransactor();
    // HYPOTHESIS: the id is the model resource of the fighter.
    bool activeArticle(wnGameWatchSausage* weapon, soModuleAccesser* acc, u32 mdlResId);
};
static_assert(sizeof(ftGameWatchTransactor) == 4, "Transactor singleton layout is wrong!");
