#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soModuleAccesser;
class wnPopoIceShot;

// Popo's per-fighter helper (the code is in sora_melee): starts the ice shot.
class ftPopoTransactor : public ftTransactor {
public:
    ftPopoTransactor();
    virtual ~ftPopoTransactor();
    // Local declaration of the REL singleton-holder entry point.
    static ftPopoTransactor* getInstance();
    // HYPOTHESIS: the id is the effect resource of the fighter.
    bool activeArticle1(wnPopoIceShot* weapon, soModuleAccesser* acc, u32 etcResId);
};
static_assert(sizeof(ftPopoTransactor) == 4, "Transactor singleton layout is wrong!");
