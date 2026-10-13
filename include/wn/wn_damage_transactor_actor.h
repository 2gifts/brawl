#pragma once

// Article damage actors share the core transactor slots, followed by their
// damage-stop callback. Fighter and item tail slots are excluded by this policy.
#ifndef WN_DAMAGE_ACTOR_INTERFACE
#error wn_damage_transactor_actor.h requires the article damage actor policy
#endif
#include <so/damage/so_damage_transactor_actor.h>

// HYPOTHESIS: unused formal parameters retain the canonical actor declarations;
// the native table establishes method order and class identity.
class wnDamageTransactorActor : public soDamageTransactor {
public:
    virtual bool onDamage(soModuleAccesser*, soDamage*, soDamageLog*);
    virtual void setupDamageStatusNormal(soModuleAccesser*, soDamage*, soDamageLog*, int);
    virtual void setupDamageStatusTurn(soModuleAccesser*, soDamage*, soDamageLog*);
    virtual void setupSpeedDamage(soModuleAccesser*, soDamage*, soDamageLog*);
    virtual void setupDamageStatusNoReaction(soModuleAccesser*, soDamage*, soDamageLog*);
    virtual void setupDamageFlyRollStatus(float, float, soModuleAccesser*, soDamageLog*);
    virtual void checkCheer(float, float, soModuleAccesser*, soDamageLog*) { }
    virtual float getDamageForReaction(float damage, soModuleAccesser*) { return damage; }
    virtual bool checkNoReaction(soModuleAccesser*, soDamage*) { return false; }
    virtual int checkDownDamage(float, float, soModuleAccesser*) { return 0; }
    virtual void onLeaveDamageStop(soModuleAccesser*) = 0;
};
static_assert(sizeof(wnDamageTransactorActor) == 4, "Article damage actor extent");
