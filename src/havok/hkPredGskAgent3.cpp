// Havok translation unit hkPredGskAgent3.o (main.dol 0x803007FC-0x803020E0).
// Not yet decompiled. Functions in address order:
//   0x803007FC   192  registerAgent3
//   0x803008BC   188  create
//   0x80300978    36  getTotalSizeInBytes
//   0x8030099C     8  hkAddByteOffset<v>
//   0x803009A4   396  sepNormal
//   0x80300B30   124  cleanup
//   0x80300BAC    60  removePoint
//   0x80300BE8    72  commitPotential
//   0x80300C30    60  createZombie
//   0x80300C6C    16  destroy
//   0x80300C7C  1128  process
//   0x803010E4    24  getRootCollidable
//   0x803010FC     4  __ct
//   0x80301100  1656  hkGskManifold_init
//   0x80301778     8  getTolerance
//   0x80301780  2400  hkGskAgentUtil_processCollisionNoTim

#include <havok/hkPredGskAgent3.h>
#include <havok/hkGskManifoldUtil.h>
#include <types.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkAgent1nTrack.h>
#include <havok/hkCollidable.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkProcessCollisionOutput.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkMath.h>
#include <havok/hkConvexShape.h>
#include <havok/hkShapeType.h>
#include <havok/hkGskCache.h>

// The agent3 functions of the predictive GSK agent; predictive, with a symmetric version.
void hkPredGskAgent3::registerAgent3(hkCollisionDispatcher* dispatcher, int typeA, int typeB) {
    hkAgent3Funcs f;
    f.m_createFunc = (hkAgentFunc)create;
    f.m_processFunc = (hkAgentFunc)process;
    f.m_sepNormalFunc = (hkAgentFunc)sepNormal;
    f.m_cleanupFunc = (hkAgentFunc)cleanup;
    f.m_removePointFunc = (hkAgentFunc)removePoint;
    f.m_commitPotentialFunc = (hkAgentFunc)commitPotential;
    f.m_createZombieFunc = (hkAgentFunc)createZombie;
    f.m_destroyFunc = (hkAgentFunc)destroy;
    f.m_isPredictive = true;
    dispatcher->registerAgent3(f, typeA, typeB);
}


// The agent data is the GSK cache followed by an empty manifold; a triangle as shape B has its own cache setup.
void* hkPredGskAgent3::create(const hkAgent3Input& input, hkAgentEntry* entry, void* agentData) {
    const hkConvexShape* shapeA = (const hkConvexShape*)input.m_bodyA->m_shape;
    const hkConvexShape* shapeB = (const hkConvexShape*)input.m_bodyB->m_shape;
    hkGskCache* gskCache = (hkGskCache*)agentData;
    if (shapeB->getType() == HK_SHAPE_TRIANGLE) {
        gskCache->initTriangle(shapeA, shapeB, input.m_aTb);
    } else {
        gskCache->init(shapeA, shapeB, input.m_aTb);
    }
    hkGskManifold* gskManifold = (hkGskManifold*)(gskCache + 1);
    entry->m_numContactPoints = 0;
    *(u32*)gskManifold = 0; // the four header counts
    return hkAddByteOffset(agentData, (gskManifold->getTotalSizeInBytes() + sizeof(hkGskCache) + 15) & ~15);
}

void* hkPredGskAgent3::cleanup(hkGskManifold* dst, hkPredGskAgent3* agent, void* arg) {
    hkGskManifold* manifold = &agent->m_manifold;
    hkGskManifoldUtil::hkGskManifold_cleanup(manifold, arg);
    dst->m_countC = manifold->m_countC;
    u8 a = manifold->m_countA;
    u8 b = manifold->m_countB;
    u8 c = manifold->m_countC;
    u32 size = ((a + b) << 1) + (c << 3);
    return (u8*)agent + ((size + 0x1f) & ~0xf);
}

void hkPredGskAgent3::destroy(void* ctx, hkPredGskAgent3* agent, void* arg) {
    hkGskManifoldUtil::hkGskManifold_cleanup(&agent->m_manifold, arg);
}

void hkPredGskAgent3::removePoint(void* ctx, hkPredGskAgent3* agent, u16 key) {
    hkGskManifold* manifold = &agent->m_manifold;
    u8* p = (u8*)manifold;
    for (int i = 0; i < manifold->m_countC; i++) {
        if (key == *(u16*)(p + 6)) {
            hkGskManifoldUtil::hkGskManifold_removePoint(manifold, i);
            return;
        }
        p += 8;
    }
}

void hkPredGskAgent3::commitPotential(void* ctx, hkPredGskAgent3* agent, u16 key) {
    u8* p;
    hkGskManifold* manifold = &agent->m_manifold;
    int i;
    p = (u8*)manifold;
    for (i = 0; i < manifold->m_countC; i++) {
        if (*(u16*)(p + 6) == 0xFFFF) {
            *(u16*)((u8*)manifold + i * 8 + 6) = key;
            return;
        }
        p += 8;
    }
}

void hkPredGskAgent3::createZombie(void* ctx, hkPredGskAgent3* agent, u16 key) {
    hkGskManifold* manifold = &agent->m_manifold;
    hkGskManifoldEntry* entry = manifold->getEntries();
    for (int i = 0; i < manifold->m_countC; i++) {
        if (entry->key == key) {
            entry->unk0 = 0;
            entry->unk1 = 0;
            return;
        }
        entry++;
    }
}

// BYTES: the whole function is compiled without inlining; every accessor is a call to its out-of-line copy.
#pragma push
#pragma dont_inline on
void* hkPredGskAgent3::process(const hkAgent3ProcessInput& input, hkAgentEntry* entry, void* agentData,
                               hkVector4* separatingNormal, hkProcessCollisionOutput& output) {
    HK_TIMER_BEGIN_ACCESSORS("PredGskf3");
    HK_TIMER_BEGIN_LIST_ACCESSORS("intern", "init");

    hkGskCache* gskCache = (hkGskCache*)agentData;
    hkGskManifold* gskManifold = (hkGskManifold*)(gskCache + 1);
    hkReal distAtT1 = input.m_distAtT1;
    {
        const hkCollisionQualityInfo& qi = *input.m_input->m_collisionQualityInfo;
        if (qi.unk10) {
            hkReal radiusA = ((const hkCollidable*)input.m_bodyA->getRootCollidable())->m_allowedPenetrationDepth;
            hkReal radiusB = ((const hkCollidable*)input.m_bodyB->getRootCollidable())->m_allowedPenetrationDepth;
            hkReal allowedPenetrationDepth = hkMath::min2(radiusA, radiusB);
            hkReal distAtT0 = (*separatingNormal)(3);
            hkReal minSeparation = hkMath::min2(qi.unk14[0] * allowedPenetrationDepth,
                                                distAtT0 + qi.unk14[1] * allowedPenetrationDepth);
            if (distAtT1 >= minSeparation) {
                goto QUICK_VERIFY_MANIFOLD;
            }
            hkReal toiSeparation = hkMath::min2(qi.unk14[4] * allowedPenetrationDepth,
                                                distAtT0 + qi.unk14[5] * allowedPenetrationDepth);
            HK_TIMER_SPLIT_LIST_ACCESSORS("toi");
            hk4dGskCollideCalcToi(input, allowedPenetrationDepth, minSeparation, toiSeparation, *gskCache,
                                  *separatingNormal, output);
            goto PROCESS_AT_T1;
        }
    QUICK_VERIFY_MANIFOLD:
        if (distAtT1 > qi.unk0[3]) {
            (*separatingNormal)(3) = distAtT1;
            if (gskManifold->m_countC) {
                HK_TIMER_SPLIT_LIST_ACCESSORS("getPoints");
                hkGskManifoldWork work;
                hkGskManifold_init(*gskManifold, *separatingNormal, *input.m_bodyA, *input.m_bodyB,
                                   input.m_input->getTolerance(), work);
                hkGskManifold_verifyAndGetPoints(*gskManifold, work, 0, output, input.m_contactMgr);
                if (gskManifold->m_countC && output.m_potentialContacts) {
                    *output.m_potentialContacts->m_firstFreeRepresentativeContact++ =
                        output.m_firstFreeContactPoint - gskManifold->m_countC;
                }
            }
            goto END;
        }
    }
PROCESS_AT_T1:
    HK_TIMER_SPLIT_LIST_ACCESSORS("process");
    hkGskAgentUtil_processCollisionNoTim(input, entry, agentData, *gskCache, *gskManifold, *separatingNormal, output);
END:
    entry->m_numContactPoints = gskManifold->m_countC;
    HK_TIMER_END_LIST_ACCESSORS();
    HK_TIMER_END_ACCESSORS();
    return hkAddByteOffset(agentData, (gskManifold->getTotalSizeInBytes() + sizeof(hkGskCache) + 15) & ~15);
}
#pragma pop

u32 hkGskManifold::getTotalSizeInBytes() const {
    const u8* p = &m_countA;
    int a = *p++;
    int b = *p++;
    int c = *p;
    return (a + b) * 2 + c * 8 + 4;
}

// TODO: clean up, give variables real names
namespace jj_803009A4 {
#pragma fp_contract on

struct Vec { float x, y, z, w; } __attribute__((aligned(16)));
struct Matrix { float m[4][4]; } __attribute__((aligned(16)));
struct State {
    int countA, countB, limitA, limitB;
    char flag; char pad[3]; int changed; char pad1[8];
    Vec a[4], unused[4], b[4], localB[4];
    Vec dir; float pad3[16]; int status;
    State() : flag(0) {}
};
class Object_vptr_0 {
public:
    virtual void slot0(); virtual void slot1(); virtual void slot2(); virtual void slot3();
    virtual void slot4(); virtual void slot5(); virtual void slot6(); virtual void slot7();
    virtual void slot8(); virtual void slot9(); virtual void slot10();
    virtual void slot11(void*, int, Vec*);
    char pad[8]; float radius;
};
struct Node { Object_vptr_0* shape; void* key; Matrix* transform; };
struct Pair { Node* a; Node* b; char pad[8]; Matrix matrix; };
struct Input { short indices[4]; signed char countA, countB, limitA, limitB; };
extern "C" void fn_803198C8(State*, Object_vptr_0*, Object_vptr_0*, Matrix*, Vec*);
extern "C" void fn_8031AB44(State*, void*);
extern const float lbl_805A45A8;

extern "C" void fn_803009A4(Object_vptr_0* sa, Input* input, Pair* pair) {
    Object_vptr_0* sb;
    Vec* out = (Vec*)pair;
    pair = (Pair*)sa;
    sa = pair->a->shape;
    sb = pair->b->shape;
    State state;
    state.countA = input->countA;
    state.countB = input->countB;
    state.limitA = input->limitA;
    state.limitB = input->limitB;
    state.changed = 0;
    sa->slot11(input->indices, state.countA, state.a);
    sb->slot11(input->indices + state.countA, state.countB, state.localB);
    Vec v;
    fn_803198C8(&state, sa, sb, &pair->matrix, &v);
    if (state.changed) fn_8031AB44(&state, input);
    Matrix* m = pair->a->transform;
    float x = v.x, y = v.y, z = v.z;
    out->x = z * m->m[2][0] + (x * m->m[0][0] + y * m->m[1][0]);
    out->y = z * m->m[2][1] + (x * m->m[0][1] + y * m->m[1][1]);
    out->z = z * m->m[2][2] + (x * m->m[0][2] + y * m->m[1][2]);
    out->w = lbl_805A45A8;
    float ra = sa->radius;
    float rb = sb->radius;
    out->w = (v.w - ra) - rb;
}
}

