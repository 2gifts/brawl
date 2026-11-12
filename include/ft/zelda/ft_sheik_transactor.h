#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

// Sheik's per-fighter helper (the code is in sora_melee): starts her articles (map names; bodies not reconstructed yet).
class ftSheikTransactor : public ftTransactor {
public:
    ftSheikTransactor();
    virtual ~ftSheikTransactor();
};