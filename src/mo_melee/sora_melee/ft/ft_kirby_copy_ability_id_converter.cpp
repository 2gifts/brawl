#include <types.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>

extern "C" int fn_27_30FB6C(ftKirbyCopyAbilityIdConverter* self, int unk, int statusKind, int value, bool flag, bool unk2);

int ftKirbyCopyAbilityIdConverter::convCorrectToOrigId(int unk, int statusKind, soModuleAccesser* moduleAccesser, bool unk2)
{
    if (moduleAccesser->getStageObject().soGetSubKind() == 5) {
        int value = moduleAccesser->getWorkManageModule().getInt(0x10000048);
        return fn_27_30FB6C(this, unk, statusKind, value, true, unk2);
    }
    return statusKind;
}
