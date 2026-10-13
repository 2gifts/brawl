#pragma once

class soModuleAccesser;

// HYPOTHESIS: the native policy accepts an opaque energy-pool pointer; its
// dispatch currently uses only the kinetic kind and module accesser.
class wnKineticTransactor {
public:
    static void changeKinetic(int kineticType, void* pools, soModuleAccesser* acc);
#ifdef WN_KINETIC_GENERIC_BUILDER
    template <typename E>
    static void updateEnergy(E* energy, soModuleAccesser* acc) {
        if (energy->isEnable() != true || energy->isSuspend()) return;
        energy->updateEnergy(acc);
    }
#endif
};
