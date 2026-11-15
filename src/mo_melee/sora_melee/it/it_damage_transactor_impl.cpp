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
    void damageItem(int statusKind, soModuleAccesser* moduleAccesser, soDamageLog* damageLog, bool notifyItem);
};

static inline BaseItem* getItem(soModuleAccesser* moduleAccesser) {
    return dynamic_cast<BaseItem*>(&moduleAccesser->getStageObject());
}

// The hit takes the damage off the item's hit points (work float 0x11000006, if it has any; flag 0x22000002 when they
// run out) and raises the damage flags.
void itDamageTransactorImpl::damageItem(int statusKind, soModuleAccesser* moduleAccesser, soDamageLog* damageLog, bool notifyItem) {
    BaseItem* item = getItem(moduleAccesser);
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (item->onPreDamageCheck(statusKind, damageLog)) {
        work.onFlag(0x22000001);
        work.onFlag(0x12000004);
        float hitPoint = work.getFloat(0x11000006);
        if (hitPoint > 0.0f) {
            hitPoint -= damageLog->m_damageAdd;
            if (hitPoint <= 0.001f) {
                work.onFlag(0x22000002);
                hitPoint = 0.0f;
            }
            work.setFloat(hitPoint, 0x11000006);
        }
        item->requestAnimEvent(0);
        if (notifyItem) {
            item->onDamage(statusKind, damageLog);
        }
    }
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
