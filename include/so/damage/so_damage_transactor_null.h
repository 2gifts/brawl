#pragma once

// soDamageTransactorNull: the damage transactor of stage objects that never react to damage (weapons, gimmicks).
// Every slot answers "no reaction"; its virtuals are inline (emitted as weak copies where the vtable is needed).
// Moved out of yk/yk_normal.h so that the weapon builders can share it.

#include <so/damage/so_damage_transactor_actor.h>

class soDamageTransactorNull : public soDamageTransactor {
public:
    virtual int getDamageValueParam(soModuleAccesser*) { return 0; }
    virtual bool onDamageChangeStatusRequest(int, soModuleAccesser*, soDamageLog*) { return true; }
    virtual int getDamageStatusKind(soModuleAccesser*) { return 0; }
    virtual bool isUseTurnDamage(soModuleAccesser*) { return false; }
    virtual bool isUseTurn(soModuleAccesser*) { return false; }
    virtual bool isApplyTurnDamage(soModuleAccesser*) { return true; }
    virtual int getDamageHeight(soModuleAccesser*, u8) { return 0; }
    // The weapon and stage copies both load the same 1.0 constant as getReactionMul.
    virtual float getHitStopMul(soModuleAccesser*) { return 1.0f; }
    virtual bool isSlip(soModuleAccesser*, float) { return false; }
    virtual bool isSleepStatus(soModuleAccesser*) { return false; }
    virtual bool isParalyzeDamage(soModuleAccesser*) { return false; }
    virtual void addSleepTime(soModuleAccesser*, soDamage*, soDamageLog*) { }
    virtual void onFlowerDamage(soModuleAccesser*, soDamage*) { }
    virtual void onParalyzeDamage(soModuleAccesser*, soDamage*, soDamageLog*) { }
    virtual void setFlagDownDamage3(soModuleAccesser*, bool) { }
    virtual bool isCheckGroundDamage(soModuleAccesser*) { return false; }
    virtual void onGroundDamageAfter(soModuleAccesser*) { }
    virtual bool onCompositionDamageSpeed(soModuleAccesser*, soDamage*, Vec2f*, int) { return false; }
    virtual bool onDamage(soModuleAccesser*, soDamage*, soDamageLog*) { return false; }
    virtual void setupDamageStatusNormal(soModuleAccesser*, soDamage*, soDamageLog*, int) { }
    virtual void setupDamageStatusTurn(soModuleAccesser*, soDamage*, soDamageLog*) { }
    virtual void setupSpeedDamage(soModuleAccesser*, soDamage*, soDamageLog*) { }
    virtual void setupDamageStatusNoReaction(soModuleAccesser*, soDamage*, soDamageLog*) { }
    virtual void setupDamageFlyRollStatus(float, float, soModuleAccesser*, soDamageLog*) { }
    virtual float getReactionSub(soModuleAccesser*) { return 0.0f; }
    virtual float getReactionMul(soModuleAccesser*) { return 1.0f; }
    virtual float getDamageMul(soModuleAccesser*) { return 1.0f; }
    virtual void checkCheer(float, float, soModuleAccesser*, soDamageLog*) { }
    virtual float getDamageForReaction(float damage, soModuleAccesser*) { return damage; }
    virtual bool checkNoReaction(soModuleAccesser*, soDamage*) { return true; }
    virtual int checkDownDamage(float, float, soModuleAccesser*) { return 0; }
#ifndef YK_STAGE_FULL
    // HYPOTHESIS: the module maps show no copies of these for the null transactor; they only complete the interface here.
    virtual bool isBindStatus(soModuleAccesser*) { return false; }
    virtual bool isBuryStatus(soModuleAccesser*) { return false; }
    virtual bool isSpeedDamage(soModuleAccesser*) { return false; }
#endif
};
static_assert(sizeof(soDamageTransactorNull) == 4, "Class is wrong size!");
