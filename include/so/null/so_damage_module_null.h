#pragma once

#include <so/damage/so_damage_module_impl.h>
#include <so/damage/so_damage_effector_null.h>

inline soDamageModule::~soDamageModule() { }

class soDamageModuleNull : public soDamageModule {
public:
    virtual ~soDamageModuleNull();
    virtual void activate(float damage);
    virtual void deactivate();
    virtual void sleep(bool);
    virtual void update();
    virtual void initDamage(float damage);
    virtual void initInfo();
    virtual void setInfo();
    virtual void storeDamage(soDamage* damage, float addedDamage);
    virtual bool check(bool);
    virtual float getDamageLr(float, float, void*, void*, int, int);
    virtual bool setForceDamage(StageObject* stageObject, Vec3f* pos, u32 collisionIndex, u32 damageIndex, bool getAbsolute, bool initInfo);
    virtual bool setForceDamage(StageObject* stageObject, int nodeId, u32 collisionIndex, u32 damageIndex, bool getAbsolute, bool initInfo);
    virtual bool setForceDamage(StageObject* stageObject, const char* nodeName, u32 collisionIndex, u32 damageIndex, bool getAbsolute, bool initInfo);
    virtual void addDamage(float damage, u32 damageIndex);
    virtual float getDamage(u32 damageIndex);
    virtual float getReaction(u32 damageIndex);
    virtual float getPowerMax(u32 damageIndex);
    virtual bool checkNoReaction(void*);
    virtual void setNoReactionModeStatus(float, float, int mode);
    virtual void setNoReactionMode2nd(float, float, int mode);
    virtual void resetNoReactionModeStatus();
    virtual void setNoReactionMode(int mode);
    virtual void resetNoReactionMode();
    virtual void setNoReactionModeAlways(bool);
    virtual void setNoReactionModePerfect(bool);
    virtual bool isNoReactionModePerfect();
    virtual void toTurnDamage();
    virtual void leaveTurnDamage();
    virtual soDamageLog* getDamageLog();
    virtual void clearDamageLog();
    virtual void getAttackerInfo(soDamageAttackerInfo* attackerInfo);
    virtual soDamageAttackerInfo* getAttackerInfo();
    virtual void restoreAttackerInfo(soDamageAttackerInfo* attackerInfo);
    virtual void clearAttackerInfo();
    virtual soDamageEffector* getEffector();
    virtual soDamageTransactor* getTransactor() {
        static soDamageTransactorNull s_transactor;
        return &s_transactor;
    }
    virtual bool isReaction();
    virtual bool isCatchCut();
    virtual float getCaptureDamage();
    virtual bool isCaptureCut();
    virtual bool isCaptureCut(void*);
    virtual void setDamageMul(float damageMul);
    virtual void setReactionMul(float reactionMul);
    virtual void setReactionMul2nd(float reactionMul2nd);
    virtual void setDamageLock(bool);
    virtual bool isDamageLock();
    virtual void heal(float healAmount, u32 damageIndex);
    virtual void updateAttackerInfo();
};
