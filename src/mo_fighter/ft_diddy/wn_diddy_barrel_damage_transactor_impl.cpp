#define WN_DAMAGE_ACTOR_INTERFACE
#include <types.h>
#include <wn/diddy/wn_diddy_barrel_damage_transactor.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/damage/so_damage_value_accesser.h>

extern "C" {
extern char lbl_115_data_A800[];
extern const float lbl_115_rodata_138;
extern const float lbl_115_rodata_13C;
}

int wnDiddyBarrelDamageTransactorImpl::getDamageValueParam(soModuleAccesser*) {
    // HYPOTHESIS: the formal return type is pointer-sized; the pointed-to type is unresolved.
    return (int)lbl_115_data_A800;
}

bool wnDiddyBarrelDamageTransactorImpl::onDamageChangeStatusRequest(
    int, soModuleAccesser* moduleAccesser, soDamageLog* damageLog
) {
    float damage = moduleAccesser->getWorkManageModule().getFloat(0x21000003) - damageLog->m_damageAdd;
    moduleAccesser->getWorkManageModule().setFloat(damage, 0x21000003);
    if (damage <= lbl_115_rodata_138) {
        return moduleAccesser->getStatusModule().changeStatusRequest(2, moduleAccesser);
    }
    return false;
}

int wnDiddyBarrelDamageTransactorImpl::getDamageStatusKind(soModuleAccesser* moduleAccesser) {
    return soDamageValueAccesser::getConstantDamageStatusKind(moduleAccesser, moduleAccesser->getStatusModule().getStatusKind());
}

bool wnDiddyBarrelDamageTransactorImpl::isUseTurnDamage(soModuleAccesser*) { return false; }

bool wnDiddyBarrelDamageTransactorImpl::isUseTurn(soModuleAccesser*) { return true; }

bool wnDiddyBarrelDamageTransactorImpl::isApplyTurnDamage(soModuleAccesser*) { return true; }

int wnDiddyBarrelDamageTransactorImpl::getDamageHeight(soModuleAccesser*, u8) { return 0; }

float wnDiddyBarrelDamageTransactorImpl::getHitStopMul(soModuleAccesser*) { return lbl_115_rodata_13C; }

bool wnDiddyBarrelDamageTransactorImpl::isSlip(soModuleAccesser*, float) { return false; }

bool wnDiddyBarrelDamageTransactorImpl::isSleepStatus(soModuleAccesser*) { return false; }

bool wnDiddyBarrelDamageTransactorImpl::isParalyzeDamage(soModuleAccesser*) { return false; }

void wnDiddyBarrelDamageTransactorImpl::addSleepTime(soModuleAccesser*, soDamage*, soDamageLog*) { }

void wnDiddyBarrelDamageTransactorImpl::onFlowerDamage(soModuleAccesser*, soDamage*) { }

void wnDiddyBarrelDamageTransactorImpl::onParalyzeDamage(soModuleAccesser*, soDamage*, soDamageLog*) { }

void wnDiddyBarrelDamageTransactorImpl::setFlagDownDamage3(soModuleAccesser*, bool) { }

bool wnDiddyBarrelDamageTransactorImpl::isCheckGroundDamage(soModuleAccesser*) { return false; }

void wnDiddyBarrelDamageTransactorImpl::onLeaveDamageStop(soModuleAccesser*) { }

void wnDiddyBarrelDamageTransactorImpl::onGroundDamageAfter(soModuleAccesser*) { }

bool wnDiddyBarrelDamageTransactorImpl::onCompositionDamageSpeed(soModuleAccesser*, soDamage*, Vec2f*, int) { return false; }

wnDiddyBarrelDamageTransactorImpl g_wnDiddyBarrelDamageTransactorImpl;
