#include <so/kinetic/so_kinetic_energy.h>
#include <types.h>

// HYPOTHESIS: the original returns a constant true; the callers in ft_builder_kinetic.h ignore the result.
class soKineticTransactHelper {
public:
    static bool checkClearSpeed(soKineticEnergy* energy);
};

bool soKineticTransactHelper::checkClearSpeed(soKineticEnergy* energy) {
    if (!energy->isEnable()) {
        energy->clearSpeed();
    }
    return true;
}
