#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

// Fighter constants use converter category 1 to select Kirby's copied-ability values.
float soModuleAccesser::getConstantFloatKirby(s32 id) {
    return soValueAccesser::getConstantFloat(this, g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(1, id, this, 0), 0);
}

s32 soModuleAccesser::getConstantIntKirby(s32 id) {
    return soValueAccesser::getConstantInt(this, g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(1, id, this, 0), 0);
}
