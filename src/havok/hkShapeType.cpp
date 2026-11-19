// Havok translation unit hkShapeType.o (main.dol 0x802D4EC0-0x802D51F4).
// Functions in address order:
//   0x802D4EC0   292  hkRegisterAlternateShapeTypes
//   0x802D4FE4   528  hkGetShapeTypeName

#include <havok/hkShapeType.h>
#include <havok/hkCollisionDispatcher.h>

// Tells the dispatcher which shape types can also be handled as a more general type (every convex shape as
// HK_SHAPE_CONVEX, the list and mesh shapes as collections, the MOPP shape as a bv tree, ...). Name from the
// DebugMode map.
void hkRegisterAlternateShapeTypes(hkCollisionDispatcher* dis) {
    dis->registerAlternateShapeType(HK_SHAPE_SPHERE, HK_SHAPE_CONVEX);
    dis->registerAlternateShapeType(HK_SHAPE_TRIANGLE, HK_SHAPE_CONVEX);
    dis->registerAlternateShapeType(HK_SHAPE_BOX, HK_SHAPE_CONVEX);
    dis->registerAlternateShapeType(HK_SHAPE_CAPSULE, HK_SHAPE_CONVEX);
    dis->registerAlternateShapeType(HK_SHAPE_CYLINDER, HK_SHAPE_CONVEX);
    dis->registerAlternateShapeType(HK_SHAPE_CONVEX_VERTICES, HK_SHAPE_CONVEX);
    dis->registerAlternateShapeType(HK_SHAPE_CONVEX_TRANSLATE, HK_SHAPE_CONVEX);
    dis->registerAlternateShapeType(HK_SHAPE_CONVEX_TRANSFORM, HK_SHAPE_CONVEX);
    dis->registerAlternateShapeType(HK_SHAPE_CONVEX_PIECE, HK_SHAPE_CONVEX);
    dis->registerAlternateShapeType(HK_SHAPE_TRIANGLE_COLLECTION, HK_SHAPE_COLLECTION);
    dis->registerAlternateShapeType(HK_SHAPE_LIST, HK_SHAPE_COLLECTION);
    dis->registerAlternateShapeType(HK_SHAPE_CONVEX_LIST, HK_SHAPE_LIST);
    dis->registerAlternateShapeType(HK_SHAPE_MOPP, HK_SHAPE_BV_TREE);
    dis->registerAlternateShapeType(HK_SHAPE_CONVEX, HK_SHAPE_SPHERE_REP);
    dis->registerAlternateShapeType(HK_SHAPE_PLANE, HK_SHAPE_HEIGHT_FIELD);
    dis->registerAlternateShapeType(HK_SHAPE_SAMPLED_HEIGHT_FIELD, HK_SHAPE_HEIGHT_FIELD);
}

const char* hkGetShapeTypeName(int type) {
    switch (type) {
    case HK_SHAPE_ALL:
        return "HK_SHAPE_ALL";
    case HK_SHAPE_CONVEX:
        return "HK_SHAPE_CONVEX";
    case HK_SHAPE_COLLECTION:
        return "HK_SHAPE_COLLECTION";
    case HK_SHAPE_BV_TREE:
        return "HK_SHAPE_BV_TREE";
    case HK_SHAPE_SPHERE:
        return "HK_SHAPE_SPHERE";
    case HK_SHAPE_TRIANGLE:
        return "HK_SHAPE_TRIANGLE";
    case HK_SHAPE_BOX:
        return "HK_SHAPE_BOX";
    case HK_SHAPE_CAPSULE:
        return "HK_SHAPE_CAPSULE";
    case HK_SHAPE_CYLINDER:
        return "HK_SHAPE_CYLINDER";
    case HK_SHAPE_CONVEX_VERTICES:
        return "HK_SHAPE_CONVEX_VERTICES";
    case HK_SHAPE_CONVEX_PIECE:
        return "HK_SHAPE_CONVEX_PIECE";
    case HK_SHAPE_MULTI_SPHERE:
        return "HK_SHAPE_MULTI_SPHERE";
    case HK_SHAPE_LIST:
        return "HK_SHAPE_LIST";
    case HK_SHAPE_CONVEX_LIST:
        return "HK_SHAPE_CONVEX_LIST";
    case HK_SHAPE_TRIANGLE_COLLECTION:
        return "HK_SHAPE_TRIANGLE_COLLECTION";
    case HK_SHAPE_MULTI_RAY:
        return "HK_SHAPE_MULTI_RAY";
    case HK_SHAPE_HEIGHT_FIELD:
        return "HK_SHAPE_HEIGHT_FIELD";
    case HK_SHAPE_SAMPLED_HEIGHT_FIELD:
        return "HK_SHAPE_SAMPLED_HEIGHT_FIELD";
    case HK_SHAPE_SPHERE_REP:
        return "HK_SHAPE_SPHERE_REP";
    case HK_SHAPE_TRI_PATCH:
        return "HK_SHAPE_TRI_PATCH";
    case HK_SHAPE_BV:
        return "HK_SHAPE_BV";
    case HK_SHAPE_PLANE:
        return "HK_SHAPE_PLANE";
    case HK_SHAPE_MOPP:
        return "HK_SHAPE_MOPP";
    case HK_SHAPE_TRANSFORM:
        return "HK_SHAPE_TRANSFORM";
    case HK_SHAPE_CONVEX_TRANSLATE:
        return "HK_SHAPE_CONVEX_TRANSLATE";
    case HK_SHAPE_CONVEX_TRANSFORM:
        return "HK_SHAPE_CONVEX_TRANSFORM";
    case HK_SHAPE_PHANTOM_CALLBACK:
        return "HK_SHAPE_PHANTOM_CALLBACK";
    case HK_SHAPE_USER0:
        return "HK_SHAPE_USER0";
    case HK_SHAPE_USER1:
        return "HK_SHAPE_USER1";
    case HK_SHAPE_USER2:
        return "HK_SHAPE_USER2";
    default:
        return "unknown";
    }
}
