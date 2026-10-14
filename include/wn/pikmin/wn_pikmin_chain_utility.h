#pragma once

#include <so/so_module_accesser.h>

// Shared helper of the chain article and its statuses.
class wnPikminChainUtility {
public:
    // Keeps the number of visible Pikmin of the chain in step with its current reach and tells the parent link.
    static void updatePikminVisibility(soModuleAccesser* moduleAccesser);
};
