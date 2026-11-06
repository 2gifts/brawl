#include <types.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>

// HYPOTHESIS: per-index tables of (orig id, correct id) pairs terminated by a zero orig id.
extern int* lbl_27_bss_7790[4][55];

int ftKirbyCopyAbilityIdConverter::convertId(int index, int statusKind, int value, bool toOrig, bool unk2)
{
    int* entry;
    if (index <= -1 || index >= 4) {
        entry = 0;
    } else if (value <= -1 || value >= 55) {
        entry = 0;
    } else {
        entry = lbl_27_bss_7790[index][value];
    }
    if (entry == 0) {
        return statusKind;
    }
    for (; entry[0] != 0; entry += 2) {
        if (!toOrig) {
            if (entry[0] == statusKind) {
                return entry[1];
            }
        } else if (entry[1] == statusKind) {
            return entry[0];
        }
    }
    return statusKind;
}

int ftKirbyCopyAbilityIdConverter::convOrigToCorrectId(int unk, int statusKind, soModuleAccesser* moduleAccesser, bool unk2)
{
    if (moduleAccesser->getStageObject().soGetSubKind() == 5) {
        int value = moduleAccesser->getWorkManageModule().getInt(0x10000048);
        return convertId(unk, statusKind, value, false, unk2);
    }
    return statusKind;
}

int ftKirbyCopyAbilityIdConverter::convCorrectToOrigId(int unk, int statusKind, soModuleAccesser* moduleAccesser, bool unk2)
{
    if (moduleAccesser->getStageObject().soGetSubKind() == 5) {
        int value = moduleAccesser->getWorkManageModule().getInt(0x10000048);
        return convertId(unk, statusKind, value, true, unk2);
    }
    return statusKind;
}
