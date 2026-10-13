#define WN_WEAPON_BUILDER
#define WN_DAMAGE_ACTOR_INTERFACE
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_OCTOPUS_ARRAYS
#define WN_NULL_DAMAGE_OUT_OF_LINE
#define WN_OCTOPUS_PARAM_ACCESSER_OUT_OF_LINE
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <wn/gamewatch/wn_game_watch_octopus.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <so/resource/so_resource_module_impl.h>

#include <it/item.h>

wnGameWatchOctopusLinkEventCollision::wnGameWatchOctopusLinkEventCollision(int eventKind, float power, bool eventFlag) :
    soLinkEventArgs(eventKind), m_power(power), unkC(eventFlag) { }

void wnGameWatchOctopus::notifyEventCollisionHit(float power,
                                                 soCollisionAttackData* attackData,
                                                 u32 index, int unk6,
                                                 soModuleAccesser* moduleAccesser,
                                                 soCollisionLog* collisionLog) {
    // HYPOTHESIS: the unused argument names/types follow the canonical
    // soCollisionHitEventObserver ABI; this body only consumes power/accesser.
    (void)attackData;
    (void)index;
    (void)unk6;
    Weapon::notifyEventCollisionHit(power, attackData, index, unk6,
                                    moduleAccesser, collisionLog);

    wnGameWatchOctopusLinkEventCollision event(0x839, power, true);
    moduleAccesser->getLinkModule().sendEventParents(3, event);
    setHitStop(power, 1.0f, 1);
}

void wnGameWatchOctopus::notifyEventLink(soLinkEventArgs* eventInfo,
                                         soModuleAccesser* moduleAccesser,
                                         StageObject* other, int unk4) {
    switch (eventInfo->m_eventKind) {
    case StageObject::Link::Event_Touch_Item: {
        // soLinkTouchItemEventArgs stores its item pointer at +8.
        // HYPOTHESIS: the native virtual query is the linked item's subkind;
        // sibling Wario bike code makes the same Event_Touch_Item query.
        BaseItem* item = reinterpret_cast<soLinkTouchItemEventArgs*>(eventInfo)->m_item;
        const int itemSubKind = item->soGetSubKind();
        switch (itemSubKind) {
        case 5:
        case 15:
            moduleAccesser->getLinkModule().sendEventParents(3, *eventInfo);
            break;
        }
        break;
    }
    default:
        break;
    }
    Weapon::notifyEventLink(eventInfo, moduleAccesser, other, unk4);
}

void wnGameWatchOctopus::adjustParentGroundCollision(int unk1,
                                                       float* collisionData) {
    (void)unk1;
    // HYPOTHESIS: semantic field meanings are unknown; these are the exact
    // target stores to the float array: offsets +8, +0xC, then +0.
    collisionData[2] = -10.0f;
    collisionData[3] = 10.0f;
    collisionData[0] = 30.0f;
}


void soDamageEffectorNull::reqTipEffect(soModuleAccesser*, soCollisionLog*) {  }

void soDamageEffectorNull::reqStop() {  }

void soDamageEffectorNull::reqQuake(float, soModuleAccesser*, int) {  }

void soDamageEffectorNull::reqDamageGroundBeatDownEffect(soModuleAccesser*, Vec2f*) {  }

void soDamageEffectorNull::reqDamageEffectParam(float, float, soModuleAccesser*, soCollisionAttackData*) {  }

void soDamageEffectorNull::reqInvincibleEffect(soModuleAccesser*, soCollisionLog*) {  }

void soDamageEffectorNull::reqUniqEffect(soModuleAccesser*, int, soCollisionAttackData*) {  }

void soDamageEffectorNull::reqCommonEffect(float, float, float, soModuleAccesser*, soCollisionAttackData*, soCollisionLog*) {  }

void soDamageEffectorNull::reqShake(soModuleAccesser*, int, Vec2f*, soCollisionAttackData*, int) {  }

float soDamageTransactor::getWeightReactionMul(soModuleAccesser* moduleAccesser) { return 1.0f; }

bool soDamageTransactor::preProcessCheckDamage(soModuleAccesser* moduleAccesser, soDamage* damage) { return false; }

int soDamageTransactorNull::checkDownDamage(float, float, soModuleAccesser*) { return 0; }

bool soDamageTransactorNull::checkNoReaction(soModuleAccesser*, soDamage*) { return true; }

float soDamageTransactorNull::getDamageForReaction(float damage, soModuleAccesser*) { return damage; }

void soDamageTransactorNull::checkCheer(float, float, soModuleAccesser*, soDamageLog*) {  }

float soDamageTransactorNull::getDamageMul(soModuleAccesser*) { return 1.0f; }

float soDamageTransactorNull::getReactionMul(soModuleAccesser*) { return 1.0f; }

float soDamageTransactorNull::getReactionSub(soModuleAccesser*) { return 0.0f; }

void soDamageTransactorNull::setupDamageFlyRollStatus(float, float, soModuleAccesser*, soDamageLog*) {  }

void soDamageTransactorNull::setupDamageStatusNoReaction(soModuleAccesser*, soDamage*, soDamageLog*) {  }

void soDamageTransactorNull::setupSpeedDamage(soModuleAccesser*, soDamage*, soDamageLog*) {  }

void soDamageTransactorNull::setupDamageStatusTurn(soModuleAccesser*, soDamage*, soDamageLog*) {  }

void soDamageTransactorNull::setupDamageStatusNormal(soModuleAccesser*, soDamage*, soDamageLog*, int) {  }

bool soDamageTransactorNull::onDamage(soModuleAccesser*, soDamage*, soDamageLog*) { return false; }

bool soDamageTransactorNull::onCompositionDamageSpeed(soModuleAccesser*, soDamage*, Vec2f*, int) { return false; }

void soDamageTransactorNull::onGroundDamageAfter(soModuleAccesser*) {  }

bool soDamageTransactorNull::isCheckGroundDamage(soModuleAccesser*) { return false; }

void soDamageTransactorNull::setFlagDownDamage3(soModuleAccesser*, bool) {  }

void soDamageTransactorNull::onParalyzeDamage(soModuleAccesser*, soDamage*, soDamageLog*) {  }

void soDamageTransactorNull::onFlowerDamage(soModuleAccesser*, soDamage*) {  }

void soDamageTransactorNull::addSleepTime(soModuleAccesser*, soDamage*, soDamageLog*) {  }

bool soDamageTransactorNull::isParalyzeDamage(soModuleAccesser*) { return false; }

bool soDamageTransactorNull::isSleepStatus(soModuleAccesser*) { return false; }

bool soDamageTransactorNull::isSlip(soModuleAccesser*, float) { return false; }

float soDamageTransactorNull::getHitStopMul(soModuleAccesser*) { return 1.0f; }

int soDamageTransactorNull::getDamageHeight(soModuleAccesser*, u8) { return 0; }

bool soDamageTransactorNull::isApplyTurnDamage(soModuleAccesser*) { return true; }

bool soDamageTransactorNull::isUseTurn(soModuleAccesser*) { return false; }

bool soDamageTransactorNull::isUseTurnDamage(soModuleAccesser*) { return false; }

int soDamageTransactorNull::getDamageStatusKind(soModuleAccesser*) { return 0; }

bool soDamageTransactorNull::onDamageChangeStatusRequest(int, soModuleAccesser*, soDamageLog*) { return true; }

int soDamageTransactorNull::getDamageValueParam(soModuleAccesser*) { return 0; }

float wnExtendParamAccesser<3999, 3, 23999, 0, 0, 0>::getParamFloat(soModuleAccesser*, s32 id, s32 index) {
    return m_floats[id - m_floatBase - 1][index];
}
s32 wnExtendParamAccesser<3999, 3, 23999, 0, 0, 0>::getParamInt(soModuleAccesser*, s32 id, s32 index) {
    return m_ints[id - m_intBase - 1][index];
}
void* wnExtendParamAccesser<3999, 3, 23999, 0, 0, 0>::getParamIndefinite(soModuleAccesser*, s32 id, s32) {
    return m_indefinites[id - m_indefiniteBase - 1];
}

template <class BC>
void wnWeaponBuilder<BC>::deactivateDescendantForce() {
    soSingletonHolder<soArticleMediatorNull>::getInstance()->deactivate();
}
template void wnWeaponBuilder<wnGameWatchOctopusModuleAccesserBuildConfig>::deactivateDescendantForce();
