#pragma once

// soDamageTransactorNull: the damage transactor of stage objects that never react to damage (weapons, gimmicks).
// Every slot answers "no reaction"; its virtuals are inline (emitted as weak copies where the vtable is needed).
// Moved out of yk/yk_normal.h so that the weapon builders can share it.

#include <so/damage/so_damage_transactor_actor.h>

class soDamageTransactorNull : public soDamageTransactor {
public:
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual int getDamageValueParam(soModuleAccesser*);
    #else
    virtual int getDamageValueParam(soModuleAccesser*) { return 0; }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual bool onDamageChangeStatusRequest(int, soModuleAccesser*, soDamageLog*);
    #else
    virtual bool onDamageChangeStatusRequest(int, soModuleAccesser*, soDamageLog*) { return true; }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual int getDamageStatusKind(soModuleAccesser*);
    #else
    virtual int getDamageStatusKind(soModuleAccesser*) { return 0; }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual bool isUseTurnDamage(soModuleAccesser*);
    #else
    virtual bool isUseTurnDamage(soModuleAccesser*) { return false; }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual bool isUseTurn(soModuleAccesser*);
    #else
    virtual bool isUseTurn(soModuleAccesser*) { return false; }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual bool isApplyTurnDamage(soModuleAccesser*);
    #else
    virtual bool isApplyTurnDamage(soModuleAccesser*) { return true; }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual int getDamageHeight(soModuleAccesser*, u8);
    #else
    virtual int getDamageHeight(soModuleAccesser*, u8) { return 0; }
    #endif
    // The weapon and stage copies both load the same 1.0 constant as getReactionMul.
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual float getHitStopMul(soModuleAccesser*);
    #else
    virtual float getHitStopMul(soModuleAccesser*) { return 1.0f; }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual bool isSlip(soModuleAccesser*, float);
    #else
    virtual bool isSlip(soModuleAccesser*, float) { return false; }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual bool isSleepStatus(soModuleAccesser*);
    #else
    virtual bool isSleepStatus(soModuleAccesser*) { return false; }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual bool isParalyzeDamage(soModuleAccesser*);
    #else
    virtual bool isParalyzeDamage(soModuleAccesser*) { return false; }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual void addSleepTime(soModuleAccesser*, soDamage*, soDamageLog*);
    #else
    virtual void addSleepTime(soModuleAccesser*, soDamage*, soDamageLog*) { }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual void onFlowerDamage(soModuleAccesser*, soDamage*);
    #else
    virtual void onFlowerDamage(soModuleAccesser*, soDamage*) { }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual void onParalyzeDamage(soModuleAccesser*, soDamage*, soDamageLog*);
    #else
    virtual void onParalyzeDamage(soModuleAccesser*, soDamage*, soDamageLog*) { }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual void setFlagDownDamage3(soModuleAccesser*, bool);
    #else
    virtual void setFlagDownDamage3(soModuleAccesser*, bool) { }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual bool isCheckGroundDamage(soModuleAccesser*);
    #else
    virtual bool isCheckGroundDamage(soModuleAccesser*) { return false; }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual void onGroundDamageAfter(soModuleAccesser*);
    #else
    virtual void onGroundDamageAfter(soModuleAccesser*) { }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual bool onCompositionDamageSpeed(soModuleAccesser*, soDamage*, Vec2f*, int);
    #else
    virtual bool onCompositionDamageSpeed(soModuleAccesser*, soDamage*, Vec2f*, int) { return false; }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual bool onDamage(soModuleAccesser*, soDamage*, soDamageLog*);
    #else
    virtual bool onDamage(soModuleAccesser*, soDamage*, soDamageLog*) { return false; }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual void setupDamageStatusNormal(soModuleAccesser*, soDamage*, soDamageLog*, int);
    #else
    virtual void setupDamageStatusNormal(soModuleAccesser*, soDamage*, soDamageLog*, int) { }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual void setupDamageStatusTurn(soModuleAccesser*, soDamage*, soDamageLog*);
    #else
    virtual void setupDamageStatusTurn(soModuleAccesser*, soDamage*, soDamageLog*) { }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual void setupSpeedDamage(soModuleAccesser*, soDamage*, soDamageLog*);
    #else
    virtual void setupSpeedDamage(soModuleAccesser*, soDamage*, soDamageLog*) { }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual void setupDamageStatusNoReaction(soModuleAccesser*, soDamage*, soDamageLog*);
    #else
    virtual void setupDamageStatusNoReaction(soModuleAccesser*, soDamage*, soDamageLog*) { }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual void setupDamageFlyRollStatus(float, float, soModuleAccesser*, soDamageLog*);
    #else
    virtual void setupDamageFlyRollStatus(float, float, soModuleAccesser*, soDamageLog*) { }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual float getReactionSub(soModuleAccesser*);
    #else
    virtual float getReactionSub(soModuleAccesser*) { return 0.0f; }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual float getReactionMul(soModuleAccesser*);
    #else
    virtual float getReactionMul(soModuleAccesser*) { return 1.0f; }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual float getDamageMul(soModuleAccesser*);
    #else
    virtual float getDamageMul(soModuleAccesser*) { return 1.0f; }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual void checkCheer(float, float, soModuleAccesser*, soDamageLog*);
    #else
    virtual void checkCheer(float, float, soModuleAccesser*, soDamageLog*) { }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual float getDamageForReaction(float damage, soModuleAccesser*);
    #else
    virtual float getDamageForReaction(float damage, soModuleAccesser*) { return damage; }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual bool checkNoReaction(soModuleAccesser*, soDamage*);
    #else
    virtual bool checkNoReaction(soModuleAccesser*, soDamage*) { return true; }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual int checkDownDamage(float, float, soModuleAccesser*);
    #else
    virtual int checkDownDamage(float, float, soModuleAccesser*) { return 0; }
    #endif
#if !defined(YK_STAGE_FULL) && !defined(WN_OCTOPUS_ARRAYS)
    // HYPOTHESIS: the module maps show no copies of these for the null transactor; they only complete the interface here.
    virtual bool isBindStatus(soModuleAccesser*) { return false; }
    virtual bool isBuryStatus(soModuleAccesser*) { return false; }
    virtual bool isSpeedDamage(soModuleAccesser*) { return false; }
#endif
};
static_assert(sizeof(soDamageTransactorNull) == 4, "Class is wrong size!");
