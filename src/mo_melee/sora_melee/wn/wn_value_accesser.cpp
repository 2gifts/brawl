#include <types.h>
#include <wn/wn_value_accesser.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

wnValueAccesser::wnValueAccesser() : soValueAccesser(2) {}

int wnValueAccesser::getVariableIntCore(soModuleAccesser* moduleAccesser, u32 value, u32 extra) {
    if ((s32)value <= 20999) {
        return soValueAccesser::getVariableIntCore(moduleAccesser, value, extra);
    }
    return 0;
}

float wnValueAccesser::getConstantFloatCore(soModuleAccesser* moduleAccesser, u32 value, u32 extra) {
    if ((s32)value <= 2999) {
        return soValueAccesser::getConstantFloatCore(moduleAccesser, value, extra);
    }
    return 0.0f;
}

int wnValueAccesser::getConstantIntCore(soModuleAccesser* moduleAccesser, u32 value, u32 extra) {
    if ((s32)value <= 22999) {
        return soValueAccesser::getConstantIntCore(moduleAccesser, value, extra);
    }
    return 0;
}
