#include <wn/wn_kinetic_transactor.h>
#include <so/so_module_accesser.h>
#include <types.h>

// Only kind 0 does anything: it turns every energy off. The other common kinds have no setup for articles.
void wnKineticTransactor::changeKinetic(int kineticType, void* pools, soModuleAccesser* acc) {
    if (kineticType <= -1 || kineticType >= 2) {
        return;
    }
    switch (kineticType) {
    case 0: acc->getKineticModule().unableEnergyAll(); return;
    case 1: return;
    case 2: return;
    case 3: return;
    case 4: return;
    case 5: return;
    case 6: return;
    case 7: return;
    case 8: return;
    case 9: return;
    case 10: return;
    case 11: return;
    case 12: return;
    case 13: return;
    case 14: return;
    case 15: return;
    case 16: return;
    case 17: return;
    case 18: return;
    case 19: return;
    case 20: return;
    case 21: return;
    case 22: return;
    case 23: return;
    case 24: return;
    case 25: return;
    case 26: return;
    case 27: return;
    case 28: return;
    case 29: return;
    case 30: return;
    case 31: return;
    }
}

void wnKineticTransactor::addSpeed(void* speed, void* pools, soModuleAccesser* acc) { }

void wnKineticTransactor::addSpeedOutside(int type, void* speed, void* pools, soModuleAccesser* acc) { }
