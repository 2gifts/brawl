// Havok translation unit hkFakeRigidBodyDeactivator.o (main.dol 0x802E2094-0x802E22C4).
// Not yet decompiled. Functions in address order:
//   0x802E2094    32  finishLoadedObjecthkFakeRigidBodyDeactivator
//   0x802E20B4    92  __dt
//   0x802E2110    92  __dt
//   0x802E216C    20  cleanupLoadedObjecthkFakeRigidBodyDeactivator
//   0x802E2180    92  __dt
//   0x802E21DC    60  getVtablehkFakeRigidBodyDeactivator
//   0x802E2218     8  getRigidBodyDeactivatorType
//   0x802E2220     8  shouldDeactivateHighFrequency
//   0x802E2228     8  shouldDeactivateLowFrequency
//   0x802E2230   148  __sinit_\hkFakeRigidBodyDeactivator_cpp
#include <new>
#include <havok/hkFakeRigidBodyDeactivator.h>
#include <havok/hkVector4.h>

static hkRigidBodyDeactivatorTypeInfo s_hkFakeRigidBodyDeactivatorTypeInfo("hkFakeRigidBodyDeactivator",
    hkFakeRigidBodyDeactivator::finishLoadedObjecthkFakeRigidBodyDeactivator,
    hkFakeRigidBodyDeactivator::cleanupLoadedObjecthkFakeRigidBodyDeactivator,
    hkFakeRigidBodyDeactivator::getVtablehkFakeRigidBodyDeactivator());

static hkFakeRigidBodyDeactivator s_hkFakeRigidBodyDeactivator;

void hkFakeRigidBodyDeactivator::finishLoadedObjecthkFakeRigidBodyDeactivator(void* p) {
    new (p) hkFakeRigidBodyDeactivator(hkFinishLoadedObjectFlag());
}

void hkFakeRigidBodyDeactivator::cleanupLoadedObjecthkFakeRigidBodyDeactivator(void* p) {
    ((hkFakeRigidBodyDeactivator*)p)->~hkFakeRigidBodyDeactivator();
}

hkFakeRigidBodyDeactivator::~hkFakeRigidBodyDeactivator() {}

const void* hkFakeRigidBodyDeactivator::getVtablehkFakeRigidBodyDeactivator() {
    hkVector4 buf[1]; // placed object storage (16-byte aligned)
    new (buf) hkFakeRigidBodyDeactivator(hkFinishLoadedObjectFlag());
    return *(const void**)buf;
}

int hkFakeRigidBodyDeactivator::getRigidBodyDeactivatorType() const {
    return DEACTIVATOR_NEVER;
}

hkBool hkFakeRigidBodyDeactivator::shouldDeactivateHighFrequency(hkEntity* entity) {
    return hkBool(false);
}

hkBool hkFakeRigidBodyDeactivator::shouldDeactivateLowFrequency(hkEntity* entity) {
    return hkBool(false);
}
