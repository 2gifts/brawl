#pragma once

#include <havok/hkGskManifold.h>

// Manifold helpers of the GSK agents. The map names are hkGskManifoldUtil__hkGskManifold_* (not all of them recovered).
struct hkGskManifoldUtil {
    // Calls removePoint(key) on arg for every live entry, then clears the three counts.
    static void hkGskManifold_cleanup(hkGskManifold* manifold, void* arg);
    // Drops the vertex ids no point refers to and renumbers the points' vertex offsets.
    static void hkGskManifold_garbageCollect(hkGskManifold* manifold);
    // HYPOTHESIS: removes the entry at index from the manifold (not recovered yet).
    static void hkGskManifold_removePoint(hkGskManifold* manifold, int index);
    // Moves the cache's point to the front when it exists (1), otherwise turns a matching single-vertex point into a
    // zombie (0).
    static int hkGskManifold_doesPointExistAndResort(hkGskManifold* manifold, const struct hkGskCache& cache);
};
