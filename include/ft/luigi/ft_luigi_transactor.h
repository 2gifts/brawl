#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soModuleAccesser;
class wnLuigiFireball;

// Luigi's per-fighter helper (the code is in sora_melee): starts the fireball article. Its entry point is a virtual of the
// transactor, one past the interface of ftTransactor (vtable offset 0x50).
class ftLuigiTransactor : public ftTransactor {
public:
    ftLuigiTransactor();
    virtual ~ftLuigiTransactor();
    // HYPOTHESIS: the id is the effect resource of the fighter and the kind (0x1a) selects the fireball variant.
    virtual bool activeArticle1(wnLuigiFireball* weapon, soModuleAccesser* acc, u32 etcResId, int kind);
};
static_assert(sizeof(ftLuigiTransactor) == 4, "Transactor singleton layout is wrong!");
