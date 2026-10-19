#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soModuleAccesser;
class wnDededeStarmissile;

// Dedede's per-fighter helper (the code is in sora_melee): starts the star missile article (HYPOTHESIS: which special move it
// belongs to is unconfirmed).
class ftDededeTransactor : public ftTransactor {
public:
    ftDededeTransactor();
    virtual ~ftDededeTransactor();
    // Local declaration of the REL singleton-holder entry point.
    static ftDededeTransactor* getInstance();
    // HYPOTHESIS: the id is the effect resource of the fighter.
    bool activeArticle(wnDededeStarmissile* weapon, soModuleAccesser* acc, u32 etcResId);
};
static_assert(sizeof(ftDededeTransactor) == 4, "Transactor singleton layout is wrong!");
