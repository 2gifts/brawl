#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

// Ike's per-fighter helper (the code is in sora_melee); only the singleton shell is needed by the Kirby copy ability module.
// HYPOTHESIS: the overrides are not reconstructed yet.
class ftIkeTransactor : public ftTransactor {
public:
    ftIkeTransactor();
    virtual ~ftIkeTransactor();
};
