#pragma once

#include <mt/mt_vector.h>
#include <types.h>

class soModuleAccesser;

// Helpers shared by the Pikmin statuses and the troops: ground re-attachment and distances to the target or owner.
// HYPOTHESIS: the class name and the meaning of the distance mode (0 horizontal, 1 and 2 vertical, 3 straight).
class wnPikminPikminUtility {
public:
    static bool isOwnerConditionGround(int condition);
    static void tryAttachGround(soModuleAccesser* moduleAccesser);
    static float getTargetDist(soModuleAccesser* moduleAccesser, int mode);
    static float getOwnerDist(soModuleAccesser* moduleAccesser, int mode);
    static float getDist(Vec2f* from, Vec2f* to, int mode);
};
