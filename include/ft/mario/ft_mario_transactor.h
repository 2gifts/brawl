#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soArticle;
class soModuleAccesser;
class wnMarioFireball;

// Mario's per-fighter helper (the code is in sora_melee): starts the fireball article. Its entry point is a virtual of the
// transactor, one past the interface of ftTransactor (vtable offset 0x50).
class ftMarioTransactor : public ftTransactor {
public:
    ftMarioTransactor();
    virtual ~ftMarioTransactor();
    virtual bool activeArticle(soArticle* article, soModuleAccesser* acc);
    // Starts the fireball at the given model node (0x1b for Mario, 0x1b1 for Kirby's copy) with the effect resource.
    virtual bool activeArticle1(wnMarioFireball* weapon, soModuleAccesser* acc, u32 etcResId, int nodeId);
};
static_assert(sizeof(ftMarioTransactor) == 4, "Transactor singleton layout is wrong!");
