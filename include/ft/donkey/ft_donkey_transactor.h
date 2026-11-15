#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soModuleAccesser;

// Donkey Kong's per-fighter helper (the code is in sora_melee): drives the fully-charged Giant Punch glow while the
// charge count (work int 0x10000040) is at its maximum, keeping the effect handle in work int 0x10000041.
class ftDonkeyTransactor : public ftTransactor {
public:
    ftDonkeyTransactor();
    virtual ~ftDonkeyTransactor();
    virtual void init(soModuleAccesser* moduleAccesser);
    virtual void exit(soModuleAccesser* moduleAccesser);
    virtual void processUpdate(soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(ftDonkeyTransactor) == 4, "Transactor singleton layout is wrong!");
