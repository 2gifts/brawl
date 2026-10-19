#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soModuleAccesser;
class wnMarioFireball;

// Mario's per-fighter helper (the code is in sora_melee): starts the fireball article. Its entry point is a virtual of the
// transactor, one past the interface of ftTransactor (vtable offset 0x50).
class ftMarioTransactor : public ftTransactor {
public:
    ftMarioTransactor();
    virtual ~ftMarioTransactor();
    // HYPOTHESIS: the id is the effect resource of the fighter and the kind (0x1b) selects the fireball variant.
    virtual bool activeArticle1(wnMarioFireball* weapon, soModuleAccesser* acc, u32 etcResId, int kind);
};
static_assert(sizeof(ftMarioTransactor) == 4, "Transactor singleton layout is wrong!");
