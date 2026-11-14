// Havok translation unit hkFastMeshShape.o (main.dol 0x802D46F4-0x802D4A94).
// Functions in address order:
//   0x802D46F4    84  finishLoadedObjecthkFastMeshShape   [map: hkFastMeshShape__finishLoadedObjecthkFastMeshShape]
//   0x802D4748   152  __dt   [map: hkMeshShape____dt]
//   0x802D47E0    20  cleanupLoadedObjecthkFastMeshShape   [map: hkFastMeshShape__cleanupLoadedObjecthkFastMeshShape]
//   0x802D47F4   156  __dt   [map: hkFastMeshShape____dt]
//   0x802D4890    92  getVtablehkFastMeshShape   [map: hkFastMeshShape__getVtablehkFastMeshShape]
//   0x802D48EC   344  getChildShape   [map: hkFastMeshShape__getChildShape]
//   0x802D4A44    80  __sinit_\hkFastMeshShape_cpp   [map: hkFastMeshShapecpp____sinit_]

#include <havok/hkFastMeshShape.h>
#include <havok/hkRegistry.h>
#include <havok/hkShapeType.h>

void hkFastMeshShape::finishLoadedObjecthkFastMeshShape(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkFastMeshShape(hkFinishLoadedObjectFlag(flag));
}

void hkFastMeshShape::cleanupLoadedObjecthkFastMeshShape(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

// The hkMeshShape destructor is emitted in this translation unit (the original has it here, not in hkMeshShape.o).
hkMeshShape::~hkMeshShape() {
}

hkFastMeshShape::~hkFastMeshShape() {
}

#pragma push
#pragma auto_inline off
const void* hkFastMeshShape::getVtablehkFastMeshShape() {
    char buf[0x50] __attribute__((aligned(16)));
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    // BYTES: the flag goes in as a copy, built inside the placement new (after its null check).
    hkFastMeshShape* p = ::new (buf) hkFastMeshShape(hkFinishLoadedObjectFlag(flag));
    return *(const void**)p;
}
#pragma pop

// Not yet decompiled in this unit:
//   0x802D48EC   344  getChildShape   [map: hkFastMeshShape__getChildShape]

static hkStaticTypeInfo hkFastMeshShapeTypeInfo("hkFastMeshShape", hkFastMeshShape::finishLoadedObjecthkFastMeshShape, hkFastMeshShape::cleanupLoadedObjecthkFastMeshShape,
                                      hkFastMeshShape::getVtablehkFastMeshShape());

#include <types.h>

// fn_802D4A94: body from https://github.com/jj-link/brawl src/havok/fn_802D4A94.cpp (namespaced).
namespace jj_fn_802D4A94 {
extern "C" void fn_802D48EC(void*);

// TODO: cleanup (shape from jj-link/brawl fn_802D4A94)
extern "C" void fn_802D4A94(void* self) {
    fn_802D48EC(reinterpret_cast<u8*>(self) - 0xc);
}
} // namespace jj_fn_802D4A94

// fn_802D4A9C: body from https://github.com/jj-link/brawl src/havok/fn_802D4A9C.cpp (namespaced).
namespace jj_fn_802D4A9C {
extern "C" void fn_802D53A4(void*);

// TODO: cleanup (shape from jj-link/brawl fn_802D4A9C)
extern "C" void fn_802D4A9C(void* self) {
    fn_802D53A4(reinterpret_cast<u8*>(self) - 0xc);
}
} // namespace jj_fn_802D4A9C

// fn_802D4AA4: body from https://github.com/jj-link/brawl src/havok/fn_802D4AA4.cpp (namespaced).
namespace jj_fn_802D4AA4 {
extern "C" void fn_802D52FC(void*);

// TODO: cleanup (shape from jj-link/brawl fn_802D4AA4)
extern "C" void fn_802D4AA4(void* self) {
    fn_802D52FC(reinterpret_cast<u8*>(self) - 0xc);
}
} // namespace jj_fn_802D4AA4

// fn_802D4AAC: body from https://github.com/jj-link/brawl src/havok/fn_802D4AAC.cpp (namespaced).
namespace jj_fn_802D4AAC {
// MATCH-ONLY: secondary-base this-adjusting thunk; the callee is bound by its mangled name until the
// owning class (hkShapeContainer base at +0xC) is modelled.
extern "C" void __dt__15hkFastMeshShapeFv(void*);

// TODO: cleanup (shape from jj-link/brawl fn_802D4AAC)
extern "C" void fn_802D4AAC(void* self) {
    __dt__15hkFastMeshShapeFv(reinterpret_cast<u8*>(self) - 0xc);
}
} // namespace jj_fn_802D4AAC
