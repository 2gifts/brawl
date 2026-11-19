// Havok translation unit hkConstrainedSystemFilter.o (main.dol 0x8032BD74-0x8032C304).
// Functions in address order. Only the functions listed below are
// written so far; the four isCollisionEnabled overloads are not done yet.
//   0x8032BDE4    72  finishLoadedObjecthkConstrainedSystemFilter
//   0x8032BE2C    20  cleanupLoadedObjecthkConstrainedSystemFilter
//   0x8032BE40   100  getVtablehkConstrainedSystemFilter
//   0x8032BEA4   100  __ct
//   0x8032BF08   228  __dt
//   0x8032C24C   100  constraintAddedCallback
//   0x8032C2B0     4  constraintRemovedCallback
//   0x8032C2B4    80  __sinit_\hkConstrainedSystemFilter_cpp

#include <havok/hkConstrainedSystemFilter.h>
#include <havok/hkRegistry.h>
#include <havok/hkConstraintInstance.h>
#include <havok/hkConstraintData.h>
#include <havok/hkEntity.h>
#include <havok/hkAgentNnMachine.h>
#include <havok/hkWorldAgentUtil.h>

void hkConstrainedSystemFilter::finishLoadedObjecthkConstrainedSystemFilter(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkConstrainedSystemFilter(flag);
}

void hkConstrainedSystemFilter::cleanupLoadedObjecthkConstrainedSystemFilter(void* p) {
    ((hkConstrainedSystemFilter*)p)->~hkConstrainedSystemFilter();
}

#pragma auto_inline off
const void* hkConstrainedSystemFilter::getVtablehkConstrainedSystemFilter() {
    // Temporary object on the stack; only its vtable pointer is read back.
    __attribute__((aligned(16))) char buf[0x20];
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    ::new (buf) hkConstrainedSystemFilter(flag);
    return *(const void**)buf;
}
#pragma auto_inline reset

hkConstrainedSystemFilter::hkConstrainedSystemFilter(hkCollisionFilter* childFilter) {
    m_childFilter = childFilter;
    if (childFilter != 0) {
        childFilter->addReference();
    }
}

hkConstrainedSystemFilter::~hkConstrainedSystemFilter() {
    if (m_childFilter != 0) {
        m_childFilter->removeReference();
    }
}

// A new non-contact constraint removes the agent between its two bodies (and the agent's TOI events).
void hkConstrainedSystemFilter::constraintAddedCallback(const void* constraint) {
    hkConstraintInstance* instance = (hkConstraintInstance*)constraint;
    if (instance->getData()->getType() != 11) {
        hkAgentNnEntry* entry = hkAgentNnMachine_FindAgent(&instance->m_entities[0]->m_collidable,
                                                           &instance->m_entities[1]->m_collidable);
        if (entry != 0) {
            hkWorldAgentUtil::removeAgentAndItsToiEvents(entry);
        }
    }
}

void hkConstrainedSystemFilter::constraintRemovedCallback(const void* constraint) {}

hkBool hkConstrainedSystemFilter::isCollisionEnabled(const hkCollisionInput& input, const hkCdBody& a,
                                                     const hkCdBody& b, const hkShapeCollection& bContainer,
                                                     u32 bKey) const {
    hkBool enabled = true;
    hkCollisionFilter* child = m_childFilter;
    if (child != 0) {
        if (child->isCollisionEnabled(input, a, b, bContainer, bKey)) {
        } else {
            enabled = false;
        }
    }
    return enabled;
}

hkBool hkConstrainedSystemFilter::isCollisionEnabled(const hkShapeRayCastInput& aInput,
                                                     const hkShapeCollection& bContainer, u32 bKey) const {
    hkBool enabled = true;
    hkCollisionFilter* child = m_childFilter;
    if (child != 0) {
        if (child->isCollisionEnabled(aInput, bContainer, bKey)) {
        } else {
            enabled = false;
        }
    }
    return enabled;
}

hkBool hkConstrainedSystemFilter::isCollisionEnabled(const hkWorldRayCastInput& a,
                                                     const hkCollidable& collidableB) const {
    hkBool enabled = true;
    hkCollisionFilter* child = m_childFilter;
    if (child != 0) {
        if (child->isCollisionEnabled(a, collidableB)) {
        } else {
            enabled = false;
        }
    }
    return enabled;
}

static hkStaticTypeInfo hkConstrainedSystemFilterTypeInfo("hkConstrainedSystemFilter", hkConstrainedSystemFilter::finishLoadedObjecthkConstrainedSystemFilter,
                                                    hkConstrainedSystemFilter::cleanupLoadedObjecthkConstrainedSystemFilter,
                                                    hkConstrainedSystemFilter::getVtablehkConstrainedSystemFilter());

// fn_8032BD74: body from https://github.com/jj-link/brawl src/havok/fn_8032BD74.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_8032BD74 {
extern "C" {
extern "C" const float lbl_805A4840;
extern "C" const float lbl_805A4844;
}
// TODO: cleanup (shape from jj-link/brawl fn_8032BD74)
extern "C" void fn_8032BD74(float *p)
{
    float arr[4] __attribute__((aligned(16)));
    float zero = lbl_805A4840;
    float one = lbl_805A4844;
    p[0] = zero;
    p[5] = zero;
    p[10] = zero;
    p[1] = zero;
    p[2] = zero;
    p[3] = zero;
    p[4] = zero;
    p[6] = zero;
    p[7] = zero;
    p[8] = zero;
    p[9] = zero;
    p[11] = zero;
    arr[3] = zero;
    arr[2] = zero;
    arr[1] = zero;
    arr[0] = zero;
    p[0] = one;
    p[5] = one;
    p[10] = one;
}
} // namespace jj_fn_8032BD74

// TODO: clean up, give variables real names
namespace jj_8032BFEC {
namespace {
class Obj {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual s32 slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual u32 slot18();
};

struct Node {
    u8 pad0[0xc];
    Obj* obj;
    u32 f10;
    u32 f14;
    u32 f18;
};

struct Ref {
    u8 pad0[0x10];
    s32 off;
    u8 pad1[4];
    s8 kind;
};

struct Owner {
    u8 pad0[0x1c];
    Obj* obj;
};
} // namespace

extern "C" {
s32 getNumConstraints__8hkEntityCFv(void* p);
Node* getConstraint__8hkEntityCFi(void* p, s32 idx);

s32 isCollisionEnabled__25hkConstrainedSystemFilterCFRC12hkCollidableRC12hkCollidable(Owner* self, Ref* a, Ref* b) {
    s32 var_r30;
    s32 temp_r30;
    Node* var_r29;
    Node* var_r28;
    Node* var_r31;
    s32 var_r27;
    Obj* obj;

    if (a->kind == 1) {
        var_r28 = (Node*)((u8*)a + a->off);
    } else {
        var_r28 = 0;
    }
    if (b->kind == 1) {
        var_r31 = (Node*)((u8*)b + b->off);
    } else {
        var_r31 = 0;
    }
    var_r30 = 0;
    obj = self->obj;
    if (obj != 0) {
        if ((s8)(obj->slot18() >> 24) == 0) {
            var_r30 = 1;
        }
    }
    if (var_r30 != 0) {
        return 0;
    }
    if (var_r28 == 0 || var_r31 == 0) {
        return 0x01000000;
    }
    temp_r30 = getNumConstraints__8hkEntityCFv(var_r31);
    if (getNumConstraints__8hkEntityCFv(var_r28) > temp_r30) {
        var_r29 = var_r31;
    } else {
        var_r29 = var_r28;
        var_r28 = var_r31;
    }
    temp_r30 = getNumConstraints__8hkEntityCFv(var_r29);
    for (var_r27 = 0; var_r27 < temp_r30; var_r27++) {
        Node* cur = getConstraint__8hkEntityCFi(var_r29, var_r27);
        if (cur != 0) {
            if (cur->obj->slot3() != 0xb) {
                if (cur->f14 == (u32)var_r28 || cur->f18 == (u32)var_r28) {
                    return 0;
                }
            }
        }
    }
    return 0x01000000;
}
}
}

