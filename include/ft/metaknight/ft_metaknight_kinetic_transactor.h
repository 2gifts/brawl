#pragma once

#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>

// HYPOTHESIS: name of the inline type/mode selection layer. The native calls
// specialize three energy types; all other combinations update the energy normally.
template <int Mode, typename Energy> struct ftMetaknightEnergyModeUpdate;

class ftMetaknightKineticTransactor {
    // MATCH-ONLY: retain the native write even when a generic mode hook ignores its result.
    static void initializeResult(bool* result) {
        volatile bool* storage = result;
        *storage = false;
    }

    // MATCH-ONLY: native mode hooks retain an otherwise unused false result.
    template <int Mode, typename Energy>
    static void updateMode(Energy* energy, soModuleAccesser* acc) {
        bool result;
        initializeResult(&result);
        ftMetaknightEnergyModeUpdate<Mode, Energy>::update(energy, &result, acc);
    }

public:
    static void changeKinetic(int mode, void* pools, soModuleAccesser* moduleAccesser);
    static void updateEnergySub(ftKineticEnergyGravity*, bool*, soModuleAccesser*);
    static void updateEnergySub1(ftKineticEnergyController*, bool*, soModuleAccesser*);
    static void updateEnergySub2(ftKineticEnergyController*, bool*, soModuleAccesser*);
    static void updateEnergySub3(ftKineticEnergyMotion*, bool*, soModuleAccesser*);
    static void updateEnergySub4(ftKineticEnergyController*, bool*, soModuleAccesser*);
    static void updateEnergySub5(ftKineticEnergyMotion*, bool*, soModuleAccesser*);

    // MATCH-ONLY: updateMode retains the native dead result slots.
    template <typename Energy>
    static void updateEnergy(Energy* energy, soModuleAccesser* moduleAccesser) {
        if (moduleAccesser->getKineticModule().getKineticType() <= 0x63) {
            if (energy->isEnable() != true) {
                return;
            }
            if (energy->isSuspend()) {
                return;
            }
            energy->updateEnergy(moduleAccesser);
            return;
        }
        switch (moduleAccesser->getKineticModule().getKineticType()) {
        case 0x64: {
            updateMode<0x64>(energy, moduleAccesser);
            break;
        }
        case 0x65: {
            updateMode<0x65>(energy, moduleAccesser);
            break;
        }
        case 0x66: {
            updateMode<0x66>(energy, moduleAccesser);
            break;
        }
        case 0x67: {
            updateMode<0x67>(energy, moduleAccesser);
            break;
        }
        case 0x68: {
            updateMode<0x68>(energy, moduleAccesser);
            break;
        }
        case 0x69: {
            updateMode<0x69>(energy, moduleAccesser);
            break;
        }
        case 0x6a: {
            updateMode<0x6a>(energy, moduleAccesser);
            break;
        }
        case 0x6b: {
            updateMode<0x6b>(energy, moduleAccesser);
            break;
        }
        }
    }
};

template <int Mode, typename Energy>
struct ftMetaknightEnergyModeUpdate {
    static void update(Energy* energy, bool*, soModuleAccesser* moduleAccesser) {
        energy->updateEnergy(moduleAccesser);
    }
};

template <> struct ftMetaknightEnergyModeUpdate<0x65, ftKineticEnergyGravity> {
    static void update(ftKineticEnergyGravity* energy, bool* result, soModuleAccesser* moduleAccesser) {
        bool handled = false;
        ftMetaknightKineticTransactor::updateEnergySub(energy, &handled, moduleAccesser);
    }
};

template <> struct ftMetaknightEnergyModeUpdate<0x64, ftKineticEnergyController> {
    static void update(ftKineticEnergyController* energy, bool* result, soModuleAccesser* moduleAccesser) {
        bool handled = false;
        ftMetaknightKineticTransactor::updateEnergySub1(energy, &handled, moduleAccesser);
    }
};

template <> struct ftMetaknightEnergyModeUpdate<0x65, ftKineticEnergyController> {
    static void update(ftKineticEnergyController* energy, bool* result, soModuleAccesser* moduleAccesser) {
        bool handled = false;
        ftMetaknightKineticTransactor::updateEnergySub2(energy, &handled, moduleAccesser);
    }
};

template <> struct ftMetaknightEnergyModeUpdate<0x68, ftKineticEnergyMotion> {
    static void update(ftKineticEnergyMotion* energy, bool* result, soModuleAccesser* moduleAccesser) {
        bool handled = false;
        ftMetaknightKineticTransactor::updateEnergySub3(energy, &handled, moduleAccesser);
    }
};

template <> struct ftMetaknightEnergyModeUpdate<0x6a, ftKineticEnergyController> {
    static void update(ftKineticEnergyController* energy, bool* result, soModuleAccesser* moduleAccesser) {
        bool handled = false;
        ftMetaknightKineticTransactor::updateEnergySub4(energy, &handled, moduleAccesser);
    }
};

template <> struct ftMetaknightEnergyModeUpdate<0x6b, ftKineticEnergyMotion> {
    static void update(ftKineticEnergyMotion* energy, bool* result, soModuleAccesser* moduleAccesser) {
        ftMetaknightKineticTransactor::updateEnergySub5(energy, result, moduleAccesser);
    }
};
