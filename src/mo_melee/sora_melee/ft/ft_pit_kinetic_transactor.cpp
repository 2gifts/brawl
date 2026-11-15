#include <ft/builder/ft_builder_kinetic.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_kinetic_energy_controller.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>

// TODO: changeKinetic (modes 0x64..0x6d).

// Pit's kinetic transactor (the declaration of include/ft/pit/ft_pit.h pulls in the whole fighter builder).
class ftPitKineticTransactor : public ftKineticTransactor {
public:
    static void changeKinetic(int mode, void* pools, soModuleAccesser* acc);
    template <typename E>
    static void updateEnergy(E* energy, soModuleAccesser* acc);
};

// In mode 0x67 a flick of the stick cancels the controller's limits on that axis.
// HYPOTHESIS: m_unk40 and m_unk48 are the horizontal and vertical limits.
template <>
void ftPitKineticTransactor::updateEnergy<ftKineticEnergyController>(ftKineticEnergyController* energy, soModuleAccesser* acc) {
    switch (acc->getKineticModule().getKineticType()) {
    case 0x67: {
        float x = acc->getControllerModule().getFlickX() == 0 ? soValueAccesser::getConstantFloat(acc, 0xfb2, 0) : 0.0f;
        float y = acc->getControllerModule().getFlickY() == 0 ? soValueAccesser::getConstantFloat(acc, 0xfb2, 0) : 0.0f;
        energy->m_unk40 = x;
        energy->m_unk48 = y;
        break;
    }
    }
    energy->updateEnergy(acc);
}
