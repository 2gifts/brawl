#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soModuleAccesser;
class wnLucarioAuraBall;

// Lucario's per-fighter helper (the code is in sora_melee): starts and throws the aura ball article.
class ftLucarioTransactor : public ftTransactor {
public:
    ftLucarioTransactor();
    virtual ~ftLucarioTransactor();
    // Local declaration of the REL singleton-holder entry point.
    static ftLucarioTransactor* getInstance();
    bool activeArticle1(wnLucarioAuraBall* weapon, soModuleAccesser* acc);
    bool shootArticle(wnLucarioAuraBall* weapon, soModuleAccesser* acc);
};
static_assert(sizeof(ftLucarioTransactor) == 4, "Transactor singleton layout is wrong!");
