#pragma once

#include <types.h>

class soModelModule;
class newCollision;
class Vec3f;

class soCollisionUtil {
public:
    // HYPOTHESIS: shape and node parameters follow the dispatch and model calls.
    static void renewCollision(soModelModule* model, newCollision* collision, int shape,
                               u32 nodeId, const Vec3f* offset0, const Vec3f* offset1);
};
