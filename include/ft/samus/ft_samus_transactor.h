#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soModuleAccesser;

// Samus's per-fighter helper (the code is in sora_melee): the Charge Shot and its articles (map names; bodies not
// reconstructed yet).
class ftSamusTransactor : public ftTransactor {
public:
    ftSamusTransactor();
    virtual ~ftSamusTransactor();
    virtual void processUpdate(soModuleAccesser* moduleAccesser);
    void processUpdateSpecialNTransact(soModuleAccesser* moduleAccesser);
};