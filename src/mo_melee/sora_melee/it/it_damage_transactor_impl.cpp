#include <so/damage/so_damage_transactor_actor.h>
#include <so/so_module_accesser.h>
#include <it/item.h>
#include <types.h>

// TODO: onDamageChangeStatusRequest, damageItem, setupDamageStatusNoReaction and the static initializer.

// The damage transactor shared by the items: the turn and speed damage questions go to the item itself.
class itDamageTransactorImpl : public soDamageTransactorActor {
public:
    virtual ~itDamageTransactorImpl();
    virtual bool onDamageChangeStatusRequest(int statusKind, soModuleAccesser* moduleAccesser, soDamageLog* damageLog);
    virtual int getDamageStatusKind(soModuleAccesser* moduleAccesser);
    virtual int getDamageValueParam(soModuleAccesser* moduleAccesser);
    virtual int getDamageHeight(soModuleAccesser* moduleAccesser, u8 damageIndex);
    virtual bool isUseTurnDamage(soModuleAccesser* moduleAccesser);
    virtual bool isUseTurn(soModuleAccesser* moduleAccesser);
    virtual bool isApplyTurnDamage(soModuleAccesser* moduleAccesser);
    virtual bool isSpeedDamage(soModuleAccesser* moduleAccesser);
    virtual void setupDamageStatusNoReaction(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog);
};

static inline BaseItem* getItem(soModuleAccesser* moduleAccesser) {
    return dynamic_cast<BaseItem*>(&moduleAccesser->getStageObject());
}

int itDamageTransactorImpl::getDamageStatusKind(soModuleAccesser* moduleAccesser) {
    return 0;
}

int itDamageTransactorImpl::getDamageValueParam(soModuleAccesser* moduleAccesser) {
    return 0;
}

int itDamageTransactorImpl::getDamageHeight(soModuleAccesser* moduleAccesser, u8 damageIndex) {
    return moduleAccesser->getCollisionHitModule().getPartHeight(damageIndex, 0);
}

bool itDamageTransactorImpl::isUseTurnDamage(soModuleAccesser* moduleAccesser) {
    return getItem(moduleAccesser)->isUseTurnDamage();
}

bool itDamageTransactorImpl::isUseTurn(soModuleAccesser* moduleAccesser) {
    return getItem(moduleAccesser)->isUseTurnDamage();
}

bool itDamageTransactorImpl::isApplyTurnDamage(soModuleAccesser* moduleAccesser) {
    return true;
}

bool itDamageTransactorImpl::isSpeedDamage(soModuleAccesser* moduleAccesser) {
    return getItem(moduleAccesser)->isUseSpeedDamage();
}

itDamageTransactorImpl::~itDamageTransactorImpl() { }
