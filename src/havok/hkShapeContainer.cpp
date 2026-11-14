// Havok translation unit hkShapeContainer.o (main.dol 0x802D4CE4-0x802D4E0C).
// Functions in address order:
//   0x802D4CE4    24  finishLoadedObjecthkSingleShapeContainer
//   0x802D4CFC    20  cleanupLoadedObjecthkSingleShapeContainer
//   0x802D4D10    52  getVtablehkSingleShapeContainer
//   0x802D4D44   120  getNumChildShapes
//   0x802D4DBC    80  __sinit_\hkShapeContainer_cpp

#include <havok/hkShapeContainer.h>
#include <havok/hkRegistry.h>

void hkSingleShapeContainer::finishLoadedObjecthkSingleShapeContainer(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkSingleShapeContainer(flag);
}

void hkSingleShapeContainer::cleanupLoadedObjecthkSingleShapeContainer(void* p) {
    ((hkShapeContainer*)p)->~hkShapeContainer();
}

#pragma push
#pragma auto_inline off
const void* hkSingleShapeContainer::getVtablehkSingleShapeContainer() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[8] __attribute__((aligned(16)));
    hkSingleShapeContainer* p = ::new (buf) hkSingleShapeContainer(flag);
    return *(const void**)p;
}
#pragma pop

int hkShapeContainer::getNumChildShapes() const {
    int count = 0;
    for (unsigned int key = getFirstKey(); key != 0xffffffff; key = getNextKey(key)) {
        count++;
    }
    return count;
}

static hkStaticTypeInfo hkSingleShapeContainerTypeInfo("hkSingleShapeContainer", hkSingleShapeContainer::finishLoadedObjecthkSingleShapeContainer, hkSingleShapeContainer::cleanupLoadedObjecthkSingleShapeContainer,
                                      hkSingleShapeContainer::getVtablehkSingleShapeContainer());
