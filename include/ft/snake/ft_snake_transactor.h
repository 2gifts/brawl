#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soArticle;
class soModuleAccesser;
class wnSimple;
struct wnSimpleData;

// Snake's per-fighter helper as sora_melee defines it (the fighter unit keeps a local view): Kirby's copy box simple
// article, the grenade throw and the eject hook.
class ftSnakeTransactor : public ftTransactor {
public:
    ftSnakeTransactor();
    virtual ~ftSnakeTransactor();
    virtual bool activeArticle(soArticle* article, soModuleAccesser* acc);
    // HYPOTHESIS: the parameter order of the simple-article start.
    bool activeArticle1(wnSimple* article, soModuleAccesser* acc, u32 resId, const wnSimpleData* data);
};
static_assert(sizeof(ftSnakeTransactor) == 4, "Transactor singleton layout is wrong!");
