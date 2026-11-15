#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soArticle;
class soModuleAccesser;

// Meta Knight's per-fighter helper (the code is in sora_melee): starts the sword simple article of Kirby's copy.
class ftMetaknightTransactor : public ftTransactor {
public:
    ftMetaknightTransactor();
    virtual ~ftMetaknightTransactor();
    virtual bool activeArticle(soArticle* article, soModuleAccesser* acc);
};
static_assert(sizeof(ftMetaknightTransactor) == 4, "Transactor singleton layout is wrong!");
