#pragma once

#include <ft/mario/ft_mario_transactor.h>
#include <types.h>

class soModuleAccesser;
class soArticle;
class wnMarioFireball;

// Luigi's per-fighter helper (the code is in sora_melee): Mario's, with Luigi's Kirby resource for the copied fireball.
class ftLuigiTransactor : public ftMarioTransactor {
public:
    ftLuigiTransactor();
    virtual ~ftLuigiTransactor();
    virtual bool activeArticle(soArticle* article, soModuleAccesser* acc);
    virtual bool activeArticle1(wnMarioFireball* weapon, soModuleAccesser* acc, u32 etcResId, int nodeId);
};
static_assert(sizeof(ftLuigiTransactor) == 4, "Transactor singleton layout is wrong!");
