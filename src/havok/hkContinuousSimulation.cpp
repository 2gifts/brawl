// Havok translation unit hkContinuousSimulation.o (main.dol 0x80325FDC-0x8032BBE8).
// Not yet decompiled. Functions in address order:
//   0x80325FDC   156  __ct
//   0x80326078    32  __dl
//   0x80326124   208  __dt
//   0x803261F4   736  handleAllToisTill
//   0x803264D4   600  advanceTime
//   0x8032672C   500  reintegrateAndRecollideEntities
//   0x80326920   120  resetCollisionInformationForEntities
//   0x80326998     4  assertThereIsNoCollisionInformationForEntities
//   0x8032699C   196  removeCollisionInformationForAgent
//   0x80326A60     4  assertThereIsNoCollisionInformationForAgent
//   0x80326A64    60  warpTime
//   0x80326AA0  1100  collideInternal
//   0x80326EEC  2432  collideEntitiesBroadPhaseContinuous
//   0x8032786C   752  collideIslandNarrowPhaseContinuous
//   0x80327B5C    68  collideEntitiesNarrowPhaseContinuous
//   0x80327BA0   316  processAgentCollideContinuous
//   0x80327CDC   272  addToiEvent
//   0x80327DEC   500  removeToiEventsOfEntities
//   0x80327FE0   324  removeToiEventsOfEntity
//   0x80328124  2900  simulateToi
//   0x80328E50  1228  hkLs_doSimpleCollisionResponse
//   0x8032931C   660  hkLs_updateEntityManifolds
//   0x803295B0   108  hkLs_areVelocitiesOk
//   0x8032961C  1076  hkLs_toiCheckValidityOfConstraints
//   0x80329A50   976  hkLs_toiActivateEntitiesAndCheckConstraints
//   0x80329E20   296  hkLs_toiActivateConstraintsLinkingToFixedAndKeyframedEnti
//   0x80329F48   192  hkLs_toiActivateConstraintsLinkingActivatedEntities
//   0x8032A008   228  hkLs_toiResetVelocityAccumulatorsForEntities
//   0x8032A0EC   320  hkLs_backstepAndFreezeEntireIsland
//   0x8032A22C   320  hkLs_restoreTransformOnBodiesWithUpdatedTransform
//   0x8032A36C   392  hkLs_toiCheckFinalValidityOfCriticalConstraints
//   0x8032A4F4  4468  hkLs_localizedSolveToi
//   0x8032B8DC   672  insertAt
//   0x8032BB7C    96  insertAt
//   0x8032BBDC    12  __sinit_\hkContinuousSimulation_cpp

#include <havok/hkContinuousSimulation.h>
#include <havok/hkProcessCollisionOutput.h>
#include <havok/hkAgentNnMachine.h>
#include <havok/hkLinkedCollidable.h>
#include <havok/hkEntity.h>
#include <havok/hkCollisionAgent.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkVelocityAccumulator.h>
#include <havok/hkRotation.h>
#include <havok/hkPointerMapBase.h>
#include <havok/hkSimulationIsland.h>
#include <havok/hkSweptTransformUtil.h>
#include <havok/hkWorld.h>
#include <havok/hkStepInfo.h>
#include <havok/hkRigidMotionUtil.h>
#include <havok/hkWorldOperationQueue.h>
#include <havok/hkDefaultToiResourceMgr.h>
#include <havok/hkWorld.h>
#include <havok/hkWorldCallbackUtil.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkCollisionInput.h>

hkContinuousSimulation::hkContinuousSimulation(hkWorld* world) : hkSimulation(world) {
    m_unk0C = 1;
    hkDefaultToiResourceMgr* mgr = new hkDefaultToiResourceMgr();
    m_toiResourceMgr = mgr;
    m_unk38 = 0;
}

hkContinuousSimulation::~hkContinuousSimulation() {
    delete m_toiResourceMgr;
}

// Handles the pending time-of-impact events in time order, up to minTime: each step takes the earliest event out
// of the list, moves the time to it and simulates it with the world locked. Stops with result 3 when the memory
// system ran out during an event.
// HYPOTHESIS: the meaning of result 3 (the step failed inside the TOI pass); m_unk38 counts the handled events.
int hkContinuousSimulation::handleAllToisTill(hkReal minTime) {
    while (true) {
        hkReal earliestTime = minTime;
        int earliest = -1;
        for (int i = 0; i < m_toiEvents.getSize(); i++) {
            if (m_toiEvents[i].unk00 < earliestTime) {
                earliest = i;
                earliestTime = m_toiEvents[i].unk00;
            }
        }
        if (earliest < 0) {
            return 0;
        }
        hkToiEvent event = m_toiEvents[earliest];
        m_toiEvents.removeAt(earliest);
        m_unk10 = event.unk00;
        hkStepInfo stepInfo(m_unk10, m_unk14);
        m_world->m_dynamicsStepInfo.m_stepInfo = stepInfo;
        m_world->m_collisionInput->m_stepInfo = stepInfo;
        m_world->m_lockCount++;
        simulateToi(m_world, event, m_unk18);
        m_world->unlockAndAttemptToExecutePendingOperations();
        m_unk38++;
        if (hkMemory::getInstance().unk4 == 1) {
            return 3;
        }
    }
}

// Advances the time like hkSimulation::advanceTime, after re-colliding a failed step and handling the pending
// time-of-impact events up to the time to advance to (both inside the "Simulate" timer). A failure in either
// returns its result; a failed TOI pass is also kept as the step result.
int hkContinuousSimulation::advanceTime() {
    {
        HK_TIME_CODE_BLOCK("Simulate");
        if (m_unk24 != 0) {
            int result = reCollideAfterStepFailure();
            if (result != 0) {
                return result;
            }
        }
        hkReal timeToAdvanceTo = snapSimulateTimeAndGetTimeToAdvanceTo();
        if (m_toiEvents.getSize() != 0) {
            int result = handleAllToisTill(timeToAdvanceTo);
            if (result != 0) {
                m_unk24 = result;
                return result;
            }
        }
        m_unk10 = timeToAdvanceTo;
    }
    if (m_unk10 >= m_unk1C) {
        if (m_world->m_postSimulationListeners.m_size != 0) {
            HK_TIMER_BEGIN("PostSimCb");
            hkWorldCallbackUtil::firePostSimulationCallback(m_world);
            HK_TIMER_END();
        }
    }
    m_unk24 = 0;
    return 0;
}

// Debug-only consistency checks: empty in this build.
void hkContinuousSimulation::assertThereIsNoCollisionInformationForEntities(const void* entities) {}

void hkContinuousSimulation::assertThereIsNoCollisionInformationForAgent(const void* agent) {}

// Adds dt to the time of every pending TOI event of the continuous simulation owned by the world.
void hkContinuousSimulation::warpTime(hkReal dt) {
    hkContinuousSimulation* sim = (hkContinuousSimulation*)m_world->m_simulation;
    for (int i = 0; i < sim->m_toiEvents.m_size; i++) {
        sim->m_toiEvents.begin()[i].unk00 += dt;
    }
}

// Removes every TOI event whose key (0x18) matches the agent key (0x10 of the agent). Walks backwards and
// replaces a removed event with the last one. HYPOTHESIS: agent key is the word at 0x10.
void hkContinuousSimulation::removeCollisionInformationForAgent(const void* agent) {
    for (int i = m_toiEvents.m_size - 1; i >= 0; i--) {
        hkToiEvent* events = m_toiEvents.begin();
        if (events[i].unk18 == ((const u32*)agent)[4]) {
            m_toiEvents.m_size--;
            events[i] = events[m_toiEvents.m_size];
        }
    }
}

// Unnamed static flag (lbl_805A0F30); initialised through hkBool(bool) so the TU gets a __sinit.
// HYPOTHESIS: a debug/config switch of the unit; nothing in the written code reads it yet.
static hkBool s_hkContinuousSimulation_unk(false);

// Shape from https://github.com/jj-link/brawl src/havok/fn_8032BB7C.cpp.
// hkArray<hkEntity*> wrapping a single caller-owned entity pointer (map: insertAt1); the dtor frees only owned storage.
extern "C" void fn_8032B8DC(int, int, hkArray<hkEntity*>*);
// TODO: cleanup (shape from jj-link/brawl fn_8032BB7C)
extern "C" void fn_8032BB7C(int arg0, int arg1, hkEntity** entity) {
    hkArray<hkEntity*> entities(entity, 1, 1);
    fn_8032B8DC(arg0, arg1, &entities);
}

// Shape from https://github.com/jj-link/brawl src/havok/fn_803295B0.cpp.
// hkLs_areVelocitiesOk (map name, owner hkConstraintSchemaInfo): index of the largest |value| * scale above the
// limit m_8 * dispatcher table entry, or -1.
// TODO: cleanup (shape from jj-link/brawl fn_803295B0)
extern "C" int fn_803295B0(const char* self, const float* values, int count, char* const* data, float scale) {
    int result = -1;
    float max = *(const float*)(self + 8) *
                *(const float*)(*data + *(const signed char*)(*(const char* const*)self + 0x1c) * 0x3c + 0x1a58);
    for (int i = 0; i < count; i++) {
        float value = scale * (float)__fabs(*values);
        if (value > max) {
            max = value;
            result = i;
        }
        values++;
    }
    return result;
}

// fn_80329E20: body from https://github.com/jj-link/brawl src/havok/fn_80329E20.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_80329E20 {
struct Flag { s8 value; Flag(bool b) : value(b) {} operator s8() const { return value; } };
struct Object {
    char pad[0xA8]; u8 type;
    Flag is7() const { return type == 7; }
    Flag is6() const { return type == 6; }
    bool valid() const { return is7() || is6(); }
};
struct Pair { char pad[4]; Object* a; Object* b; bool valid() const { return a->valid() || b->valid(); } };
struct Item { char pad[0x28]; Pair* pair; };
struct Entry { Item* item; int x; float y; };
struct Array { Entry* entries; int count; };
// TODO: cleanup (shape from jj-link/brawl fn_80329E20)
extern "C" void fn_80329E20(Array* self, int* out, int n) {
    Entry* e;
    for (int i = self->count - n; i < self->count; i++) {
        e = &self->entries[i];
        if (e->item->pair->valid()) {
            int index = (*out)++;
            Entry* dst = &self->entries[index];
            Entry tmp = *dst;
            *dst = *e;
            *e = tmp;
        }
    }
}
} // namespace jj_fn_80329E20

// fn_8032A36C: body from https://github.com/jj-link/brawl src/havok/fn_8032A36C.cpp (namespaced; memory singletons rebound to our names).
#include <types.h>
#include <havok/hkMemory.h>
#include <havok/hkThreadMemory.h>
#include <havok/hkArray.h>
namespace jj_fn_8032A36C {
extern "C" int fn_802A04EC(int, int, int, int, int, void*);
extern "C" int fn_803295B0(void*, void*, int, int, float);
struct Hdr { unsigned char pad[6]; unsigned short h6; };
class Object_vptr_0 {
public:
 virtual void slot0();
 virtual void slot1();
 virtual void slot2();
 virtual int slot3();
 unsigned char pad[0x28];
 Hdr* hdr;
};
struct Obj { int a0,a4,a8; Object_vptr_0* sub; unsigned char pad[0xC]; char type; };
struct Entry { Obj* obj; int b,c; };
struct List { Entry* data; int count; };
struct Arr { Entry* data; int count,cap; };
struct Inner { unsigned char pad[8]; float f8; };
struct Ctx { Inner* inner; int a4,a8,aC,a10,a14,a18,a1C; };
struct Result { unsigned char value; Result(int v) : value(v) {} };
// TODO: cleanup (shape from jj-link/brawl fn_8032A36C)
extern "C" Result fn_8032A36C(Ctx* arg0, List* arg1, int arg2, Arr* arg3) {
 char sp8[0x400];
 Entry* out;
 Entry* e;
 int off;
 int i;
 Result ret(1);
 int r31;
 i=0; off=0;
 for (; i<arg1->count; off+=12,i++) {
  e=(Entry*)((char*)arg1->data+off);
  if(e->obj->type==4) {
   r31=fn_803295B0(e,sp8,fn_802A04EC(arg0->a4,e->b,arg0->aC,arg0->a1C,0x100,sp8),arg2,arg0->inner->f8);
   if(r31>=0) {
    if(!arg3) return Result(0);
    ret.value=0;
    if(arg3->count==(arg3->cap&0x3FFFFFFF)) hkArrayUtil::_reserveMore((hkArrayBase*)(arg3),12);
    int n=arg3->count;
    out=arg3->data+n;
    arg3->count=n+1;
    out->obj=e->obj;
    if(e->obj->sub->slot3()==11) {
     Object_vptr_0* s=e->obj->sub;
     out->b=(((int)s->hdr+0x37)&~0xF)+(r31<<5);
     out->c=((((int)s->hdr+0x37)&~0xF)+(s->hdr->h6<<5))+r31*20;
    } else { out->b=0; out->c=0; }
   }
  }
 }
 return ret;
}
} // namespace jj_fn_8032A36C

// fn_8032A22C: body from https://github.com/jj-link/brawl src/havok/fn_8032A22C.cpp (namespaced; callee rebound to our C++ name).
#include <types.h>
#include <havok/hkRotation.h>
namespace jj_fn_8032A22C {
#pragma fp_contract on
extern "C" float lbl_805A4800;
struct Vec4 { float x,y,z,w; } __attribute__((aligned(16)));
struct Data { Vec4 a,b,c,d,e,f,g,h,i; };
struct Entry { char pad[0xb0]; Data data; };
struct Container { char pad[0x38]; Entry** entries; int count; };
struct Flags { unsigned char* values; };
// TODO: cleanup (shape from jj-link/brawl fn_8032A22C)
extern "C" void fn_8032A22C(Container* self, Flags* flags) {
    float zero = lbl_805A4800;
    int offset;
    int i = 0;
    offset = 0;
    for (; i < self->count; ++i, offset += 4) {
        if (flags->values[i] != 1 && flags->values[i] != 2) continue;
        {
            Data* p = &(*(Entry**)((char*)self->entries + offset))->data;
            ((hkRotation*)(p))->set(*(const hkQuaternion*)(&p->h));
            Vec4 v;
            float nz = p->i.z, ny = p->i.y, nx = p->i.x;
            v.x = nx * p->a.x + ny * p->b.x + nz * p->c.x;
            v.y = nx * p->a.y + ny * p->b.y + nz * p->c.y;
            v.z = nx * p->a.z + ny * p->b.z + nz * p->c.z;
            v.w = zero;
            p->d.x = p->f.x - v.x;
            p->d.y = p->f.y - v.y;
            p->d.z = p->f.z - v.z;
            p->d.w = p->f.w - v.w;
        }
    }
}
} // namespace jj_fn_8032A22C

// TODO: cleanup (shape from jj-link/brawl fn_80326920)
// fn_80326920: body from https://github.com/jj-link/brawl src/havok/fn_80326920.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
#include <havok/hkSimulation.h>
namespace jj_fn_80326920 {
extern "C" {

void fn_80326920(s32 arg0, s32 *arg1, s32 arg2, unsigned char *arg3, const unsigned char *arg4) {
    s8 flag = *arg4;
    ((hkSimulation*)(arg0))->hkSimulation::resetCollisionInformationForEntities((hkEntity**)((s32)arg1), (int)(arg2), (hkWorld*)(arg3), *(hkBool*)(&flag));
    if (arg2 != 1) {
        ((hkContinuousSimulation*)arg0)->removeToiEventsOfEntities((hkEntity**)arg1, arg2);
    } else {
        ((hkContinuousSimulation*)arg0)->removeToiEventsOfEntity((hkEntity*)*arg1);
    }
}
}
} // namespace jj_fn_80326920

// TODO: cleanup (shape from jj-link/brawl fn_80327B5C)
// fn_80327B5C: body from https://github.com/jj-link/brawl src/havok/fn_80327B5C.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
#include <havok/hkSimulation.h>
namespace jj_fn_80327B5C {
typedef struct {
    int a;
    int b;
    int c;
} Tmp;

extern "C" Tmp lbl_80488CE8;

extern "C" void fn_80327B5C(int p1, int p2, int p3, int p4) {
    ((hkSimulation*)(p1))->processAgentsOfEntities((hkEntity**)(p2), (int)(p3), *(hkProcessCollisionInput*)(p4), *(hkSimulation::AgentEntryProcessFunction*)(&lbl_80488CE8), 0);
}
} // namespace jj_fn_80327B5C

// The property words of a time-of-impact event by field (the event copies them as two words).
struct hkToiEventProperties {
    u32 m_properties;
    u16 m_propertiesB;
    u8 m_propertiesC;
    u8 m_propertiesD;
};

// Queues the time of impact the agent of entry reported: when, the contact, both entities, the impact's properties
// and the contact manager (removeCollisionInformationForAgent finds the agent's events by it).
// Called out of line by the narrow phase below.
#pragma push
#pragma auto_inline off
void hkContinuousSimulation::addToiEvent(const hkProcessCollisionOutput& output, const hkAgentNnEntry& entry) {
    hkToiEvent& event = m_toiEvents.expandOne();
    event.unk00 = output.m_toi.m_time;
    event.unk0C = output.m_toi.unk24;
    event.unk20 = output.m_toi.m_contact.m_position;
    event.unk30 = output.m_toi.m_contact.m_normal;
    event.unk04[0] = (int)((char*)entry.m_collidable[0] + entry.m_collidable[0]->m_ownerOffset);
    event.unk04[1] = (int)((char*)entry.m_collidable[1] + entry.m_collidable[1]->m_ownerOffset);
    hkToiEventProperties* properties = (hkToiEventProperties*)event.unk10;
    properties->m_properties = output.m_toi.m_properties;
    properties->m_propertiesB = output.m_toi.m_propertiesB;
    properties->m_propertiesC = output.m_toi.m_propertiesC;
    properties->m_propertiesD = output.m_toi.m_propertiesD;
    event.unk18 = (u32)entry.m_contactMgr;
}
#pragma pop

// One agent of the continuous narrow phase: runs it with its own collision quality, hands new contact points to its
// contact manager and queues a reported time of impact (skipped once memory ran out).
void hkContinuousSimulation::processAgentCollideContinuous(hkAgentNnEntry* entry, hkProcessCollisionInput& input,
                                                           hkProcessCollisionOutput& output) {
    output.reset();
    input.m_collisionQualityInfo = &input.m_dispatcher->m_collisionQualityInfo[entry->m_collisionQualityIndex];
    input.m_createPredictiveAgents = input.m_collisionQualityInfo->unk10;
    hkAgentNnMachine_ProcessAgent(entry, input, output);
    if (hkMemory::getAvailableMemory() == 0) {
        hkMemory::getInstance().unk4 = 1;
    }
    if (hkMemory::getInstance().unk4 != 1) {
        if (output.isEmpty() == false) {
            entry->m_contactMgr->processContact(*entry->m_collidable[0], *entry->m_collidable[1], input, output);
        }
        if (hkBool(output.m_toi.m_time != HK_REAL_MAX)) {
            addToiEvent(output, *entry);
        }
    }
}

// Drops every pending time-of-impact event of the entity; each event's contact manager is told first.
#pragma push
#pragma auto_inline off
void hkContinuousSimulation::removeToiEventsOfEntity(hkEntity* entity) {
    for (int i = 0; i < m_toiEvents.getSize(); i++) {
        if ((hkEntity*)m_toiEvents[i].unk04[0] == entity || (hkEntity*)m_toiEvents[i].unk04[1] == entity) {
            // MATCH-ONLY: the aligned pointer turns copy propagation off as in the original (the event is reloaded).
            hkToiEvent* event __attribute__((aligned(16))) = &m_toiEvents[i];
            ((hkContactMgr*)event->unk18)
                ->removeToi(((hkEntity*)event->unk04[0])->m_collidable, ((hkEntity*)event->unk04[1])->m_collidable,
                            *(hkContactPointMaterial*)&event->unk10);
            m_toiEvents.removeAt(i);
            i--;
        }
    }
}
#pragma pop

// Drops the pending time-of-impact events of a set of entities (through a pointer map of them); each event's
// contact manager is told first.
#pragma push
#pragma auto_inline off
void hkContinuousSimulation::removeToiEventsOfEntities(hkEntity** entities, int numEntities) {
    if (m_toiEvents.getSize() != 0) {
        hkPointerMapBase<hkUlong> entitySet;
        entitySet.reserve(numEntities);
        for (int i = 0; i < numEntities; i++) {
            entitySet.insert((hkUlong)entities[i], 0);
        }
        for (int i = 0; i < m_toiEvents.getSize(); i++) {
            hkToiEvent* event __attribute__((aligned(16))) = &m_toiEvents[i];
            int keyA = entitySet.findKey((hkUlong)event->unk04[0]);
            int keyB = entitySet.findKey((hkUlong)event->unk04[1]);
            bool remove = entitySet.isValid(keyA) || entitySet.isValid(keyB);
            if (remove) {
                ((hkContactMgr*)event->unk18)
                    ->removeToi(((hkEntity*)event->unk04[0])->m_collidable, ((hkEntity*)event->unk04[1])->m_collidable,
                                *(hkContactPointMaterial*)&event->unk10);
                m_toiEvents.removeAt(i);
                i--;
            }
        }
    }
}
#pragma pop

namespace fn_80329F48_types {
struct BodyPair { char pad[4]; hkEntity* a; hkEntity* b; };
struct Item { char pad[0x28]; BodyPair* pair; };
struct Entry { Item* item; int x; float y; };
struct Array { Entry* entries; int count; };
// Moves the entries from *out on whose two bodies are both marked 8 (activated) in the state table to the front
// (at *out, which is advanced). HYPOTHESIS: the types (the shapes of fn_80329E20's).
extern "C" void fn_80329F48(Array* self, int* out, u8** entityState) {
    for (int i = *out; i < self->count; i++) {
        Entry* e = &self->entries[i];
        BodyPair* pair = e->item->pair;
        if ((*entityState)[pair->a->m_storageIndex] == 8 && (*entityState)[pair->b->m_storageIndex] == 8) {
            int index = (*out)++;
            Entry* dst = &self->entries[index];
            Entry tmp = *dst;
            *dst = *e;
            *e = tmp;
        }
    }
}
} // namespace fn_80329F48_types

// The part of the local solver's state read here: the base of its velocity accumulators. HYPOTHESIS: the type.
struct hkLsSolverState {
    u8 unk00[0xC];        // 0x00
    char* m_accumulators; // 0x0C
};

// hkLs_setAccumulatorVelocities: copies each entity's velocities into its solver accumulator; the angular velocity
// is rotated into the accumulator's frame unless that frame is the identity. HYPOTHESIS: the parameter types.
extern "C" void fn_8032A008(hkLsSolverState* state, hkArray<hkEntity*>& entities) {
    for (int i = 0; i < entities.getSize(); i++) {
        hkEntity* entity = entities[i];
        // MATCH-ONLY: the aligned pointer turns copy propagation off as in the original.
        hkVelocityAccumulator* acc __attribute__((aligned(16))) =
            (hkVelocityAccumulator*)(state->m_accumulators + entity->m_solverData);
        acc->m_linearVel = entity->m_motion.m_linearVelocity;
        if (acc->m_matrixIsIdentity) {
            acc->m_angularVel = entity->m_motion.m_angularVelocity;
        } else {
            hkRotation& coreFromWorld = *(hkRotation*)&acc->unk50;
            acc->m_angularVel.setRotatedDir(coreFromWorld, entity->m_motion.m_angularVelocity);
        }
    }
}

// hkLs_backstepAndFreezeEntireIsland: moves every entity of the island back to the given time and freezes its swept
// transform there (start = end, the step running from that time to the simulation's end time); entities not yet
// marked 8 (activated) in the state table are added to the activated list and marked. HYPOTHESIS: parameter types.
extern "C" void fn_8032A0EC(hkSimulationIsland* island, u8** entityState, hkArray<hkEntity*>& activated, hkReal time) {
    for (int i = 0; i < island->m_entities.getSize(); i++) {
        hkEntity* entity = island->m_entities[i];
        hkMotionState& ms = entity->m_motion.m_motionState;
        hkSweptTransformUtil::backStepMotionState(time, ms);
        if ((*entityState)[i] != 8) {
            activated.pushBack(entity);
            (*entityState)[i] = 8;
        }
        hkSweptTransform& st = ms.m_sweptTransform;
        st.m_centerOfMass0(0) = st.m_centerOfMass1(0);
        st.m_centerOfMass0(1) = st.m_centerOfMass1(1);
        st.m_centerOfMass0(2) = st.m_centerOfMass1(2);
        st.m_rotation0 = st.m_rotation1;
        st.m_centerOfMass0(3) = time;
        st.m_centerOfMass1(3) = 1.0f / (island->m_world->m_simulation->m_unk14 - time);
    }
}









// hkLs_updateEntityManifolds: re-runs, at the given time, the agents of the entity whose collision quality is at most
// the given one, skipping partners already activated (state 8); a movable partner not yet touched (state 0) is first
// moved to that time and marked 1. The input's step is changed for this and restored at the end.
// HYPOTHESIS: the parameter types and the meaning of the states.
extern "C" void fn_8032931C(hkEntity* entity, int maxQuality, hkProcessCollisionInput& input, u8** entityState,
                            hkReal time) {
    hkStepInfo savedStepInfo = input.m_stepInfo;
    input.m_stepInfo.m_endTime = time;
    input.m_stepInfo.m_deltaTime = time - input.m_stepInfo.m_startTime;
    if (input.m_stepInfo.m_deltaTime > 0.0f) {
        input.m_stepInfo.m_invDeltaTime = 1.0f / input.m_stepInfo.m_deltaTime;
    } else {
        input.m_stepInfo.m_invDeltaTime = 0.0f;
    }
    for (int i = 0; i < entity->m_collidable.m_collisionEntries.getSize(); i++) {
        hkLinkedCollidable::CollisionEntry& entry = entity->m_collidable.m_collisionEntries[i];
        if (hkBool(maxQuality <= (u8)entry.m_agentEntry->m_collisionQualityIndex)) {
            hkEntity* partner = (hkEntity*)((char*)entry.m_partner + entry.m_partner->m_ownerOffset);
            bool process = partner->isFixed() || (*entityState)[partner->m_storageIndex] != 8;
            if (process) {
                if (!(bool)partner->isFixed()) {
                    u8& state = (*entityState)[partner->m_storageIndex];
                    if (state == 0) {
                        state = 1;
                        hkMotionState& ms = partner->m_motion.m_motionState;
                        hkSweptTransformUtil::lerp2(ms.m_sweptTransform, time, ms.m_transform);
                    }
                }
                hkAgentNnEntry* agent = entry.m_agentEntry;
                hkProcessCollisionData output;
                output.reset();
                input.m_collisionQualityInfo = &input.m_dispatcher->m_collisionQualityInfo[5];
                input.m_createPredictiveAgents = input.m_dispatcher->m_collisionQualityInfo[agent->m_collisionQualityIndex].unk10;
                hkAgentNnMachine_ProcessAgent(agent, input, (hkProcessCollisionOutput&)output);
                if (!(bool)output.isEmpty()) {
                    agent->m_contactMgr->processContact(*agent->m_collidable[0], *agent->m_collidable[1], input,
                                                        (hkProcessCollisionOutput&)output);
                }
            }
        }
    }
    input.m_stepInfo = savedStepInfo;
}

// collideEntitiesBroadPhaseContinuous (fn_80326EEC, not written yet). HYPOTHESIS: the parameter types.
extern "C" void fn_80326EEC(hkContinuousSimulation* simulation, hkEntity** entities, int numEntities, hkWorld* world);
extern "C" void fn_80327B5C(int p1, int p2, int p3, int p4);

// Moves the entities back to the simulation time, re-integrates them over the current step and collides them again
// (broad phase, then the continuous narrow phase), with the collision input's step set to this simulation's step for
// the duration. Queued while the world is locked.
void hkContinuousSimulation::reintegrateAndRecollideEntities(hkEntity** entities, int numEntities, hkWorld* world) {
    if (world->areCriticalOperationsLocked()) {
        hkWorldOperation::BatchOperation op;
        op.m_type = hkWorldOperation::ENTITY_BATCH_REINTEGRATE_AND_RECOLLIDE;
        op.m_objects = (void**)entities;
        op.m_count = numEntities;
        world->queueOperation(&op);
        return;
    }
    world->lockCriticalOperations();
    hkStepInfo savedStepInfo = world->m_collisionInput->m_stepInfo;
    hkStepInfo stepInfo(m_unk10, m_unk14);
    world->m_collisionInput->m_stepInfo = stepInfo;
    for (int i = 0; i < numEntities; i++) {
        hkSweptTransformUtil::backStepMotionState(m_unk10, entities[i]->m_motion.m_motionState);
    }
    hkRigidMotionUtil::hkRigidMotionUtilStep(stepInfo, entities, numEntities, HK_OFFSET_OF(hkEntity, m_motion));
    resetCollisionInformationForEntities(entities, numEntities, world, false);
    fn_80326EEC(this, entities, numEntities, world);
    fn_80327B5C((int)this, (int)entities, numEntities, (int)world->m_collisionInput);
    world->unlockAndAttemptToExecutePendingOperations();
    world->m_collisionInput->m_stepInfo = savedStepInfo;
}
