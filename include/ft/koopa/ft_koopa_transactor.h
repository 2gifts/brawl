#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soModuleAccesser;
class wnKoopaBreath;

// Bowser's per-fighter helper (the code is in sora_melee): starts the fire breath article.
class ftKoopaTransactor : public ftTransactor {
public:
    ftKoopaTransactor();
    virtual ~ftKoopaTransactor();
    bool activeArticle(wnKoopaBreath* weapon, soModuleAccesser* acc);
};
static_assert(sizeof(ftKoopaTransactor) == 4, "Transactor singleton layout is wrong!");
