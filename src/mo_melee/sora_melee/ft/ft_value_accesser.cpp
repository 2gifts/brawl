#include <ft/ft_value_accesser.h>
#include <types.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

extern int g_soValueVariation;

ftValueAccesser::ftValueAccesser() : soValueAccesser(0) { }

ftValueAccesser::~ftValueAccesser() { }

void* ftValueAccesser::getConstantIndefiniteCore(soModuleAccesser* moduleAccesser, u32 id, u32 index) {
    if ((s32)id <= 42999)
        return soValueAccesser::getConstantIndefiniteCore(moduleAccesser, id, index);
    if ((s32)id < 43025)
        return moduleAccesser->getParamCustomizeModule().getParamIndefinite(g_soValueVariation, id);
    return g_ftCommonDataAccesser.getParamCommonIndefinite(g_soValueVariation, id);
}

bool ftValueAccesser::isControllerValueCore(soModuleAccesser* moduleAccesser, u32 id, u32 index) {
    if (soValueAccesser::isControllerValueCore(moduleAccesser, id, index) == true)
        return true;
    if ((s32)id > 999 && (s32)id < 1037) {
        if ((s32)id >= 1010 && (s32)id <= 1028)
            return true;
    } else if ((s32)id > 20999 && (s32)id < 21030) {
        if ((s32)id >= 21001 && (s32)id <= 21021)
            return true;
    }
    return false;
}
