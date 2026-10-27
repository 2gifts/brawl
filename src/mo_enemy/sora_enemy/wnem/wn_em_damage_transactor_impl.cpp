#include <wn/wn_em_damage_transactor_impl.h>
#include <so/damage/so_damage_effector_impl.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/damage/so_damage_util_actor.h>
#include <so/so_module_accesser.h>
#include <types.h>

int wnemDamageTransactorImpl::getDamageValueParam(soModuleAccesser* moduleAccesser) { return 0; }

// A hit with no reaction and no damage power is ignored; any other runs the normal damage setup.
bool wnemDamageTransactorImpl::onDamage(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog) {
    if (damage->m_reaction == 0.0f && damage->m_powerMax <= 0.0f) {
        return false;
    }
    setupDamageStatusNormal(moduleAccesser, damage, damageLog, -1);
    return true;
}

// The weapon's damage setup only plays the effects of the hit: the uniq effect, and the shake when it has a reaction.
// TODO(nonmatching): 96.9%, (a) only the scratch register of the 1.0f address differs: the original loads the float
// before the bool argument, which suggests getDamageHitStopFrame takes the factor before the flag (its mangled name in
// symbols.txt is our guess); calls, arguments, constants and branches agree.
void wnemDamageTransactorImpl::setupDamageStatusNormal(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog, int unk) {
    // MATCH-ONLY: declared ahead, which gives the frame the register of `this` once the status kind call is done.
    int hitStopFrame;
    int situationKind = moduleAccesser->getSituationModule().getKind();
    getDamageStatusKind(moduleAccesser);
    moduleAccesser->getDamageModule().getEffector()->reqUniqEffect(moduleAccesser, 0, &damage->m_attackData);
    hitStopFrame = soDamageUtilActor::getDamageHitStopFrame(moduleAccesser, damage, false, 1.0f);
    if (damage->m_reaction > 0.0f) {
        // HYPOTHESIS: the shake ignores the ground normal (it is passed uninitialized).
        Vec2f groundNormal;
        moduleAccesser->getDamageModule().getEffector()->reqShake(moduleAccesser, situationKind, &groundNormal, &damage->m_attackData, hitStopFrame);
    }
}

bool wnemDamageTransactorImpl::onDamageChangeStatusRequest(int statusKind, soModuleAccesser* moduleAccesser, soDamageLog* damageLog) { return true; }

int wnemDamageTransactorImpl::getDamageStatusKind(soModuleAccesser* moduleAccesser) { return 0; }

bool wnemDamageTransactorImpl::isUseTurnDamage(soModuleAccesser* moduleAccesser) { return false; }

bool wnemDamageTransactorImpl::isUseTurn(soModuleAccesser* moduleAccesser) { return false; }

int wnemDamageTransactorImpl::getDamageHeight(soModuleAccesser* moduleAccesser, u8 damageIndex) { return 0; }

bool wnemDamageTransactorImpl::isSlip(soModuleAccesser* moduleAccesser, float slipChance) { return false; }

bool wnemDamageTransactorImpl::isSleepStatus(soModuleAccesser* moduleAccesser) { return false; }

bool wnemDamageTransactorImpl::isApplyTurnDamage(soModuleAccesser* moduleAccesser) { return true; }

bool wnemDamageTransactorImpl::isParalyzeDamage(soModuleAccesser* moduleAccesser) { return false; }

void wnemDamageTransactorImpl::addSleepTime(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog) { }

void wnemDamageTransactorImpl::onFlowerDamage(soModuleAccesser* moduleAccesser, soDamage* damage) { }

void wnemDamageTransactorImpl::onParalyzeDamage(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog) { }

void wnemDamageTransactorImpl::setFlagDownDamage3(soModuleAccesser* moduleAccesser, bool flag) { }

bool wnemDamageTransactorImpl::isCheckGroundDamage(soModuleAccesser* moduleAccesser) { return false; }

void wnemDamageTransactorImpl::onLeaveDamageStop(soModuleAccesser* moduleAccesser) { }

void wnemDamageTransactorImpl::onGroundDamageAfter(soModuleAccesser* moduleAccesser) { }

bool wnemDamageTransactorImpl::onCompositionDamageSpeed(soModuleAccesser* moduleAccesser, soDamage* damage, Vec2f* speed, int level) { return false; }

// MATCH-ONLY: the original emits weak copies of these soDamageTransactorActor members here (inline in its header);
// our so/damage/so_damage_transactor_actor.h declares them out of line, so this unit defines its copies (same bodies
// as sora_melee's so_damage_transactor_actor.cpp), in the original's order.
bool soDamageTransactorActor::checkNoReaction(soModuleAccesser* moduleAccesser, soDamage* damage) { return true; }

float soDamageTransactorActor::getDamageForReaction(float damage, soModuleAccesser* moduleAccesser) { return damage; }

float soDamageTransactorActor::getHitStopMul(soModuleAccesser* moduleAccesser) { return 1.0f; }

void soDamageTransactorActor::checkCheer(float reaction, float angle, soModuleAccesser* moduleAccesser, soDamageLog* damageLog) { }

float soDamageTransactorActor::getDamageMul(soModuleAccesser* moduleAccesser) { return 1.0f; }

float soDamageTransactorActor::getReactionMul(soModuleAccesser* moduleAccesser) { return 1.0f; }

float soDamageTransactorActor::getReactionSub(soModuleAccesser* moduleAccesser) { return 0.0f; }

bool wnemDamageTransactorImpl::isSpeedDamage(soModuleAccesser* moduleAccesser) { return true; }

bool soDamageTransactorActor::isBuryStatus(soModuleAccesser* moduleAccesser) { return false; }

bool soDamageTransactorActor::isBindStatus(soModuleAccesser* moduleAccesser) { return false; }

wnemDamageTransactorImpl::~wnemDamageTransactorImpl() { }

wnemDamageTransactorImpl g_wnemDamageTransactorImpl;
