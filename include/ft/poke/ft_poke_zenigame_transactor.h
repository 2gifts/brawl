#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soModuleAccesser;

// Zenigame's per-fighter helper (the code is in sora_melee; the class name is its RTTI string there).
class ftPokeZenigameTransactor : public ftTransactor {
public:
    ftPokeZenigameTransactor();
    virtual ~ftPokeZenigameTransactor();
    // Local declaration of the REL singleton-holder entry point.
    static ftPokeZenigameTransactor* getInstance();
};
static_assert(sizeof(ftPokeZenigameTransactor) == 4, "Transactor singleton layout is wrong!");
