#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soArticle;
class soModuleAccesser;

// Marth's per-fighter helper (the code is in sora_melee): starts the sword simple article of Kirby's copy.
class ftMarthTransactor : public ftTransactor {
public:
    ftMarthTransactor();
    virtual ~ftMarthTransactor();
    virtual bool activeArticle(soArticle* article, soModuleAccesser* acc);
};
static_assert(sizeof(ftMarthTransactor) == 4, "Transactor singleton layout is wrong!");
