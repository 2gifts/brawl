#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

// HYPOTHESIS: shared parameter table and getters used by many fighter statuses (names unverified); the object holds
// only these two functions (the neighbouring CaptureDamageYoshi status emits its destructor and static initializer first).
extern u8 g_ftKirbyCopyAbilityIdConverter[];
extern "C" int fn_27_30FA2C(void* table, int kind, int id, soModuleAccesser* acc, int);

float soModuleAccesser::getConstantFloatKirby(s32 id) {
    return soValueAccesser::getConstantFloat(this, fn_27_30FA2C(g_ftKirbyCopyAbilityIdConverter, 1, id, this, 0), 0);
}

s32 soModuleAccesser::getConstantIntKirby(s32 id) {
    return soValueAccesser::getConstantInt(this, fn_27_30FA2C(g_ftKirbyCopyAbilityIdConverter, 1, id, this, 0), 0);
}
