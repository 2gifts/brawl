#pragma once
#include <types.h>

class soModuleAccesser;

// Shared utility called by the Fox and Falco Illusion articles.
class wnFoxIllusionUtil {
public:
    static void activate(soModuleAccesser* moduleAccesser, u8 unk0, s32 unk1);
    static void processUpdate(soModuleAccesser* moduleAccesser);
};
