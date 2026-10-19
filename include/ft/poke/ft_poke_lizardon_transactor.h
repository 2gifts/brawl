#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soModuleAccesser;
class wnPokeLizardonBreath;

// Lizardon's per-fighter helper (the code is in sora_melee): starts the flame breath.
class ftPokeLizardonTransactor : public ftTransactor {
public:
    ftPokeLizardonTransactor();
    virtual ~ftPokeLizardonTransactor();
    // Local declaration of the REL singleton-holder entry point.
    static ftPokeLizardonTransactor* getInstance();
    bool activeArticle1(wnPokeLizardonBreath* weapon, soModuleAccesser* acc);
};
static_assert(sizeof(ftPokeLizardonTransactor) == 4, "Transactor singleton layout is wrong!");
