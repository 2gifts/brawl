#pragma once

class soModuleAccesser;

class soKineticTransactor {
public:
    template <typename E>
    static void updateEnergy(E* energy, soModuleAccesser* moduleAccesser);
};
