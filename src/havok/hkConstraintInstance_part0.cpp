// Havok translation unit hkConstraintInstance_part0.o (main.dol 0x802DE6F4-0x802DE99C): the loaded-object support,
// constructor, destructor and entity callbacks of hkConstraintInstance (the rest of the class follows
// hkConstraintInstanceClass.o in the image).

#include <havok/hkConstraintInstance.h>
#include <havok/hkConstraintInternal.h>
#include <havok/hkConstraintData.h>
#include <havok/hkWorldOperationUtil.h>
#include <havok/hkWorldObject.h>
#include <havok/hkEntity.h>
#include <havok/hkWorld.h>
#include <havok/hkRegistry.h>

void hkConstraintInstance::finishLoadedObjecthkConstraintInstance(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkConstraintInstance(flag);
}

void hkConstraintInstance::cleanupLoadedObjecthkConstraintInstance(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

#pragma push
#pragma auto_inline off
const void* hkConstraintInstance::getVtablehkConstraintInstance() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[sizeof(hkConstraintInstance)] __attribute__((aligned(16)));
    hkConstraintInstance* p = ::new (buf) hkConstraintInstance(flag);
    return *(const void**)p;
}
#pragma pop

hkConstraintInstance::hkConstraintInstance(ConstraintPriority priority) {
    m_owner = NULL;
    m_constraintModifiers = NULL;
    m_priority = priority;
    m_wantRuntime = false;
    m_name = NULL;
    m_userData = 0;
    m_internal = NULL;
}

// The internal record keeps its own copy of the priority.
void hkConstraintInstance::setPriority(ConstraintPriority priority) {
    m_priority = priority;
    if (m_internal != NULL) {
        ((hkConstraintInternal*)m_internal)->m_unk12 = priority;
    }
}

void hkConstraintInstance::entityAddedCallback(hkEntity* entity) {}

void hkConstraintInstance::entityDeletedCallback(hkEntity* entity) {}

// A constraint still in the world goes with either of its entities.
void hkConstraintInstance::entityRemovedCallback(hkEntity* entity) {
    if (m_owner != NULL) {
        hkWorldOperationUtil::removeConstraintImmediately(entity->m_world, this, true);
    }
}

hkConstraintInstance::~hkConstraintInstance() {
    if (m_entities[0] != NULL) {
        m_entities[0]->removeReference();
    }
    if (m_entities[1] != NULL) {
        m_entities[1]->removeReference();
    }
    if (m_data != NULL) {
        m_data->removeReference();
    }
}

// A constraint with one entity missing is attached to the world's fixed body on that side.
// HYPOTHESIS: the name (hkWorld::addConstraint calls it first).
void hkConstraintInstance::pointNullsToFixedRigidBody() {
    for (int i = 0; i < 2; i++) {
        // MATCH-ONLY: the missing side is held as an int, which keeps the xor order of the target.
        int entity = (int)m_entities[i];
        if (entity == 0) {
            // the side that is set (the missing one is zero)
            int setSide = entity ^ (u32)m_entities[0];
            hkEntity* other = (hkEntity*)((u32)m_entities[1] ^ setSide);
            if (other != NULL && other->m_world != NULL) {
                m_entities[i] = (hkEntity*)other->m_world->m_fixedRigidBody;
                m_entities[i]->addReference();
            }
        }
    }
}

hkConstraintInstance::InstanceType hkConstraintInstance::getType() const {
    return TYPE_NORMAL;
}

static hkStaticTypeInfo hkConstraintInstanceTypeInfo("hkConstraintInstance",
                                                     hkConstraintInstance::finishLoadedObjecthkConstraintInstance,
                                                     hkConstraintInstance::cleanupLoadedObjecthkConstraintInstance,
                                                     hkConstraintInstance::getVtablehkConstraintInstance());
