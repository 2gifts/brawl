#pragma once

#include <StaticAssert.h>
#include <so/damage/so_damage_transactor_actor.h>
#include <types.h>

class ftDamageTransactorImpl : public soDamageTransactorActor {
public:
    // HYPOTHESIS: the interface slot returns the address of the damage status kind table as an int.
    virtual int getDamageValueParam(soModuleAccesser* moduleAccesser);
    virtual bool onDamageChangeStatusRequest(int statusKind, soModuleAccesser* moduleAccesser, soDamageLog* damageLog);
    virtual int getDamageStatusKind(soModuleAccesser* moduleAccesser);
    virtual bool isUseTurnDamage(soModuleAccesser* moduleAccesser);
    virtual void setupDamageStatusTurn(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog);
    virtual bool isUseTurn(soModuleAccesser* moduleAccesser);
    virtual bool isApplyTurnDamage(soModuleAccesser* moduleAccesser);
    virtual int getDamageHeight(soModuleAccesser* moduleAccesser, u8 damageIndex);
    virtual float getHitStopMul(soModuleAccesser* moduleAccesser);
    virtual bool isSlip(soModuleAccesser* moduleAccesser, float slipChance);
    virtual bool isSleepStatus(soModuleAccesser* moduleAccesser);
    virtual bool isBindStatus(soModuleAccesser* moduleAccesser);
    virtual bool isBuryStatus(soModuleAccesser* moduleAccesser);
    virtual bool isParalyzeDamage(soModuleAccesser* moduleAccesser);
    virtual void addSleepTime(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog);
    virtual void onFlowerDamage(soModuleAccesser* moduleAccesser, soDamage* damage);
    virtual void onParalyzeDamage(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog);
    virtual void setFlagDownDamage3(soModuleAccesser* moduleAccesser, bool flag);
    virtual bool isCheckGroundDamage(soModuleAccesser* moduleAccesser);
    virtual void onGroundDamageAfter(soModuleAccesser* moduleAccesser);
    virtual bool onCompositionDamageSpeed(soModuleAccesser* moduleAccesser, soDamage* damage, Vec2f* speed, int level);
    virtual bool isSpeedDamage(soModuleAccesser* moduleAccesser);
    virtual bool preProcessCheckDamage(soModuleAccesser* moduleAccesser, soDamage* damage);
    virtual float getDamageMul(soModuleAccesser* moduleAccesser);
    virtual void checkCheer(float reaction, float angle, soModuleAccesser* moduleAccesser, soDamageLog* damageLog);
    virtual float getDamageForReaction(float damage, soModuleAccesser* moduleAccesser);
    virtual bool checkNoReaction(soModuleAccesser* moduleAccesser, soDamage* damage);
    virtual int checkDownDamage(float reaction, float speed, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(ftDamageTransactorImpl) == 4, "Class is wrong size!");
