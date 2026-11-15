#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soArticle;
class soModuleAccesser;
class wnPokeLizardonBreath;

// Lizardon's per-fighter helper (the code is in sora_melee): starts the flame breath and keeps its two flame gauges
// (work floats 0x11000017 and 0x11000016) topped up while the breath is not in use.
class ftPokeLizardonTransactor : public ftTransactor {
public:
    ftPokeLizardonTransactor();
    virtual ~ftPokeLizardonTransactor();
    virtual void init(soModuleAccesser* moduleAccesser);
    virtual void processUpdate(soModuleAccesser* moduleAccesser);
    virtual bool activeArticle(soArticle* article, soModuleAccesser* acc);
    void initTransact(soModuleAccesser* moduleAccesser);
    void processUpdateTransact(soModuleAccesser* moduleAccesser);
    // Local declaration of the REL singleton-holder entry point.
    static ftPokeLizardonTransactor* getInstance();
    bool activeArticle1(wnPokeLizardonBreath* weapon, soModuleAccesser* acc);
};
static_assert(sizeof(ftPokeLizardonTransactor) == 4, "Transactor singleton layout is wrong!");
