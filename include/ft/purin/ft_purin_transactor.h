#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soModuleAccesser;

// Jigglypuff's per-fighter helper (the code is in sora_melee): the Rollout hit and damage reactions of the special N
// statuses (Kirby's copy included, through the copy-ability status ids).
class ftPurinTransactor : public ftTransactor {
public:
    ftPurinTransactor();
    virtual ~ftPurinTransactor();
    virtual void notifyEventCollisionAttackFighter(soCollisionLog* log, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAttackCheck(u32 flags, soModuleAccesser* moduleAccesser);
    virtual void notifyEventOnDamage(soDamage* damage, bool isDamage, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(ftPurinTransactor) == 4, "Transactor singleton layout is wrong!");
