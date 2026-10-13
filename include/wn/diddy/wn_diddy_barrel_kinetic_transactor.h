#pragma once

#include <types.h>
class soModuleAccesser;

class wnDiddyBarrelKineticTransactor {
    // HYPOTHESIS: a zeroed one-byte temporary is observed; its field meaning
    // is unknown, and the helper never reads the implicit this pointer.
    u8 unk0;
public:
    void changeKineticSub(void* pools, soModuleAccesser* moduleAccesser);
    static void changeKinetic(int mode, void* pools, soModuleAccesser* moduleAccesser);
    static void addSpeed(void* speed, void* pools, soModuleAccesser* moduleAccesser);
    static void addSpeedOutside(int type, void* speed, void* pools, soModuleAccesser* moduleAccesser);
};
