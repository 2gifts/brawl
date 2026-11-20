// Havok translation unit hkWorldObject.o (main.dol 0x802F0438-0x802F06A8).
// Functions in address order:
//   0x802F0438   116  __ct
//   0x802F04AC   188  __ct
//   0x802F0568     8  setShape
//   0x802F0570   220  calcStatistics
//   0x802F064C    28  addReference
//   0x802F0668    64  removeReference

#include <havok/hkWorldObject.h>
#include <havok/hkShape.h>

hkWorldObject::hkWorldObject(hkFinishLoadedObjectFlag flag) : m_collidable(flag), m_properties(flag) {
    if (flag.m_finishing != 0) {
        m_collidable.m_ownerOffset = (int)((u8*)this - (u8*)&m_collidable);
    }
}

// The entity constructor passes BROAD_PHASE_ENTITY.
hkWorldObject::hkWorldObject(hkShape* shape, BroadPhaseType type)
    : m_world(0), m_userData(0), m_name(0), m_collidable(shape, type) {
    m_collidable.m_ownerOffset = (int)((u8*)this - (u8*)&m_collidable);
    if (shape != 0) {
        shape->addReference();
    }
}

bool hkWorldObject::setShape(hkShape* shape) {
    return true;
}

#include <havok/hkStatisticsCollector.h>

void hkWorldObject::calcStatistics(hkStatisticsCollector* collector) const {
    collector->addChildObject("Shape", 1, m_collidable.m_shape);
    collector->addArrayStorage("CollAgtPtr", 8, m_collidable.m_collisionEntries, sizeof(hkLinkedCollidable::CollisionEntry));
    collector->addArrayStorage("Properties", 4, m_properties, sizeof(hkProperty));
}

void hkWorldObject::addReference() {
    if (m_memSizeAndFlags != 0) {
        m_referenceCount++;
    }
}

void hkWorldObject::removeReference() {
    if (m_memSizeAndFlags != 0) {
        if (--m_referenceCount == 0) {
            delete this;
        }
    }
}
