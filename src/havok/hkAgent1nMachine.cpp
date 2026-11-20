// TODO: clean up - raw offset accesses kept to get a match; define the struct with named fields and replace the *(T*)(p + 0x..) casts with readable member accesses
// Havok translation unit hkAgent1nMachine.o (main.dol 0x802FC6AC-0x802FEDA0).
// Not yet decompiled. Functions in address order:
//   0x802FC6AC   376  hkAgent1nMachine_Destroy
//   0x802FC824   356  hkAgent1nMachine_InvalidateTim
//   0x802FC988   396  hkAgent1nMachine_WarpTime
//   0x802FCB14   152  hkAgent1nMachine_Create
//   0x802FCBAC   232  hkAgent1nMachine_initInputAtTime
//   0x802FCC94    92  hkAgent1nMachine_flipInput
//   0x802FCCF0  4120  hkAgent1nMachine_Process
//   0x802FDD08  2232  hkAgent1nMachine_Weld
//   0x802FE5C0   956  hkAgent1nMachine_VisitAllAgents
//   0x802FE97C   216  hkAgent1nMachine_UpdateShapeCollectionFilterVisitor
//   0x802FEA54    12  hkAgent1nMachine_UpdateShapeCollectionFilter
//   0x802FEA60   232  calc1nStatistics
//   0x802FEB48   392  calcNnStatistics
//   0x802FECD0   208  calcNnStatisticsContactMgrsOnly

#include <havok/hkAgent1nTrack.h>

// Function body below is taken from https://github.com/jj-link/brawl (src/havok/fn_802FEA54.cpp).
// hkAgent1nMachine_UpdateShapeCollectionFilter = VisitAllAgents(track, input, UpdateShapeCollectionFilterVisitor)
extern "C" void fn_802FE5C0(void*, void*, void*);
extern "C" void fn_802FE97C(void);
// TODO: cleanup (shape from jj-link/brawl fn_802FEA54)
extern "C" void fn_802FEA54(void* a, void* b) {
    fn_802FE5C0(a, b, (void*)&fn_802FE97C);
}


// Shape from https://github.com/jj-link/brawl src/havok/fn_802FCC94.cpp.
// hkAgent1nMachine_flipInput (map name)
// TODO: cleanup (shape from jj-link/brawl fn_802FCC94)
extern "C" void fn_802FCC94(hkAgent3ProcessInput& in, hkAgent3ProcessInput& out) {
    out.m_bodyA = in.m_bodyB;
    out.m_bodyB = in.m_bodyA;
    out.m_input = in.m_input;
    out.m_contactMgr = in.m_contactMgr;
    out.m_linearTimInfo(0) = -in.m_linearTimInfo(0);
    out.m_linearTimInfo(1) = -in.m_linearTimInfo(1);
    out.m_linearTimInfo(2) = -in.m_linearTimInfo(2);
    out.m_linearTimInfo(3) = in.m_linearTimInfo(3);
    out.m_aTb.setInverse(in.m_aTb);
}

// fn_802FE97C: body from https://github.com/jj-link/brawl src/havok/fn_802FE97C.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802FE97C {
class Class_802FE97C_Vf {
public:
    virtual u32 vf0(void* a, void* b, void* c, void* d, void* e);
};

// TODO: cleanup (shape from jj-link/brawl fn_802FE97C)
extern "C" void* fn_802FE97C(u8* self, u8* rec, u32 arg2) {
    Class_802FE97C_Vf* sub = *(Class_802FE97C_Vf**)(*(u8**)(self + 0xC) + 4);
    u32 ret = sub->vf0(*(void**)(self + 0xC), *(void**)self, *(void**)(self + 4), *(void**)(self + 8), *(void**)(rec + 8));
    if ((s8)(ret >> 24) != 0 && rec[0] != 0) {
        return rec + rec[3];
    }
    if (rec[0] == 0) {
        return rec + rec[3];
    }
    u8* base = *(u8**)(*(u8**)(self + 0xC));
    void (*fp)(void*, u32, u32) = *(void (**)(void*, u32, u32))(base + rec[1] * 0x34 + 0x1698);
    fp(rec, arg2, *(u32*)(self + 0x10));
    return rec;
}
} // namespace jj_fn_802FE97C

// fn_802FEA60: body from https://github.com/jj-link/brawl src/havok/fn_802FEA60.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802FEA60 {
#pragma use_lmw_stmw on
extern "C" char lbl_80414180[];
class Object_vptr_0 {
public:
 virtual void slot0();
 virtual void slot1();
 virtual void slot2(const char*, s32, void*, s32, s32, void*);
 virtual void slot3(const char*, s32, void*);
};
// TODO: cleanup (shape from jj-link/brawl fn_802FEA60)
extern "C" void fn_802FEA60(void* arg0, Object_vptr_0* arg1) {
 u8* entry;
 u8* end;
 s32 offset = 0;
 u8* cursor;
 s32 size;
 goto setup;
scan:
 switch (*cursor) {
 case 1: goto finish;
 case 6:
  arg1->slot3(lbl_80414180, 8, *(void**)(cursor + 4));
  cursor += cursor[3];
  break;
 case 0: case 2: case 3: case 4: case 5:
  cursor += cursor[3];
  break;
 }
 if (cursor < end) goto scan;
setup:
 entry = *(u8**)(*(u8**)arg0 + offset);
 size = *(s32*)entry;
 cursor = entry + 0x10;
 end = entry + size + 0x10;
 arg1->slot2(lbl_80414180 + 6, 8, entry, size + 0x10, 0x200, entry + size);
 offset += 4;
 goto scan;
finish:;
}
} // namespace jj_fn_802FEA60

// hkAgent1nMachine_Destroy (map name): runs every agent's destroy function (dispatcher table entry 0x1698), frees
// each sector after its last entry and finally the sector directory.
// TODO: clean up (raw sector/entry views in the style of the WarpTime shape below; 64%: register numbering)
namespace hkAgent1nMachineDestroy {
typedef void (*DestroyFunc)(void*, void*, void*);

extern "C" void fn_802FC6AC(hkAgent1nTrack& track, char* dispatcher, void* contactMgr) {
    unsigned char* blk = (unsigned char*)track.m_sectors[0].m_sector;
    int idx = 4;
    unsigned char* entry = blk + 0x10;
    unsigned char* end = (unsigned char*)((int)blk + *(int*)blk + 0x10);

    for (;;) {
        unsigned char* e = entry;
        unsigned char* dat;
        DestroyFunc fn;

        switch (e[0]) {
        case 2:
        case 3:
        case 6:
            dat = entry + 0x10;
            entry += e[3];
            fn = *(DestroyFunc*)(dispatcher + e[1] * 0x34 + 0x1698);
            fn(e, dat, contactMgr);
            break;
        case 4:
        case 5:
            dat = entry + 0x20;
            entry += e[3];
            fn = *(DestroyFunc*)(dispatcher + e[1] * 0x34 + 0x1698);
            fn(e, dat, contactMgr);
            break;
        case 1:
            goto done;
        case 0:
            entry += e[3];
            break;
        }

        if (entry >= end) {
            hkThreadMemory::s_instance->deallocateChunk(blk, 0x200, 0x1D);
            blk = (unsigned char*)*(void**)((char*)track.m_sectors.m_data + idx);
            idx += 4;
            entry = blk + 0x10;
            end = (unsigned char*)((int)blk + *(int*)blk + 0x10);
        }
    }
done:
    hkThreadMemory::s_instance->deallocateChunk(blk, 0x200, 0x1D);
    track.m_sectors.clearAndDeallocate();
}
} // namespace hkAgent1nMachineDestroy

// hkAgent1nMachine_InvalidateTim (map name): walks the track like WarpTime above; the agents with a TIM entry
// (commands 4/5) get their TIM distance reset and their linear TIM info cleared before the agent's invalidateTim
// function (dispatcher table entry 0x16B0) runs.
// TODO: clean up (raw sector/entry views in the style of the WarpTime shape above; 80%: register numbering)
namespace hkAgent1nMachineInvalidateTim {
extern "C" const float lbl_805A4560;
extern "C" const float lbl_805A4564;

typedef void (*InvalidateTimFunc)(void*, void*, void*);

extern "C" void fn_802FC824(void*** track, void** input) {
    float zero = lbl_805A4564;
    float invalidTim = lbl_805A4560;
    unsigned char* blk = (unsigned char*)**track;
    unsigned char* entry = blk + 0x10;
    unsigned char* end = (unsigned char*)((int)blk + *(int*)blk + 0x10);
    int idx = 4;

    for (;;) {
        unsigned char* e = entry;
        unsigned char* dat;
        InvalidateTimFunc fn;

        switch (e[0]) {
        case 2:
        case 3:
        case 6:
            dat = entry + 0x10;
            entry += e[3];
            fn = *(InvalidateTimFunc*)((char*)*input + e[1] * 0x34 + 0x16b0);
            if (fn != 0) {
                fn(e, dat, input);
            }
            break;
        case 4:
        case 5: {
            *(float*)(e + 0xc) = invalidTim;
            dat = entry + 0x20;
            float* timInfo = (float*)(e + 0x10);
            timInfo[0] = timInfo[1] = timInfo[2] = timInfo[3] = zero;
            entry += e[3];
            fn = *(InvalidateTimFunc*)((char*)*input + e[1] * 0x34 + 0x16b0);
            if (fn != 0) {
                fn(e, dat, input);
            }
            break;
        }
        case 1:
            goto done;
        case 0:
            entry += e[3];
            break;
        }

        if (entry >= end) {
            blk = (unsigned char*)*(void**)((char*)*track + idx);
            idx += 4;
            entry = blk + 0x10;
            blk += *(int*)blk;
            end = blk + 0x10;
        }
    }
done:
    return;
}
} // namespace hkAgent1nMachineInvalidateTim

// fn_802FC988: body from https://github.com/jj-link/brawl src/havok/fn_802FC988.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802FC988 {
extern "C" {
extern "C" const float lbl_805A4560;
}

typedef void (*EntryFunc)(void*, void*, void*, float, float);

// TODO: cleanup (shape from jj-link/brawl fn_802FC988)
extern "C" void fn_802FC988(void*** arg0, void** arg1, float farg1, float farg2) {
    float neg = lbl_805A4560;
    int idx = 4;
    unsigned char* blk = (unsigned char*)**arg0;
    unsigned char* entry = blk + 0x10;
    unsigned char* end = (unsigned char*)((int)blk + *(int*)blk + 0x10);

    for (;;) {
        unsigned char* e = entry;
        unsigned char* dat;
        EntryFunc fn;

        switch (e[0]) {
        case 2:
        case 3:
        case 6:
            dat = entry + 0x10;
            entry += e[3];
            fn = *(EntryFunc*)((char*)*arg1 + e[1] * 0x34 + 0x16b4);
            if (fn != 0) {
                fn(e, dat, arg1, farg1, farg2);
            }
            break;
        case 4:
        case 5:
            dat = entry + 0x20;
            if (*(float*)(e + 0xc) == farg1) {
                *(float*)(e + 0xc) = farg2;
            } else {
                *(float*)(e + 0xc) = neg;
            }
            entry += e[3];
            fn = *(EntryFunc*)((char*)*arg1 + e[1] * 0x34 + 0x16b4);
            if (fn != 0) {
                fn(e, dat, arg1, farg1, farg2);
            }
            break;
        case 1:
            goto done;
        case 0:
            entry += e[3];
            break;
        }

        if (entry >= end) {
            blk = (unsigned char*)*(void**)((char*)*arg0 + idx);
            idx += 4;
            entry = blk + 0x10;
            blk += *(int*)blk;
            end = blk + 0x10;
        }
    }
done:
    return;
}
} // namespace jj_fn_802FC988

// fn_802FCB14: body from https://github.com/jj-link/brawl src/havok/fn_802FCB14.cpp (namespaced; memory singletons rebound to our names).
#include <types.h>
#include <havok/hkMemory.h>
#include <havok/hkThreadMemory.h>
#include <havok/hkArray.h>
namespace jj_fn_802FCB14 {

// TODO: cleanup (shape from jj-link/brawl fn_802FCB14)
extern "C" void fn_802FCB14(char* self) {
    *(int*)(self + 0x4) = 0;
    if (*(int*)(self + 0x4) == (int)(*(unsigned int*)(self + 0x8) & 0x3FFFFFFF)) {
        hkArrayUtil::_reserveMore((hkArrayBase*)(self), 4);
    }
    *(int*)(self + 0x4) = *(int*)(self + 0x4) + 1;
    char* p = (char*)hkThreadMemory::s_instance->allocateChunk(0x200, 0x1D);
    if (p != 0) {
        *(int*)p = 0;
    }
    **(void***)self = p;
    p[0x13] = 0x10;
    p[0x10] = 1;
    *(int*)(p + 0x18) = -1;
    *(int*)p = 0x10;
}
} // namespace jj_fn_802FCB14

#include <havok/hkCdBody.h>
#include <havok/hkMotionState.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkSweptTransformUtil.h>

// Bodies and transforms of the pair at the input's time (0xA0 bytes, on the caller's stack). HYPOTHESIS: the name.
struct hkAgent1nMachineTimTemp {
    hkCdBody m_bodyA;          // 0x00
    hkCdBody m_bodyB;          // 0x10
    hkTransform m_transformA;  // 0x20
    hkTransform m_transformB;  // 0x60
};

// hkAgent1nMachine_initInputAtTime (map name): a copy of the process input whose bodies use the transforms
// interpolated at the step's start time. BYTES: the input is not const: CodeWarrior assumes nothing writes through
// other pointers into a const-qualified object, so a const input lets it keep in.m_bodyA etc. in registers across
// the stores into temp/out, which the target does not do.
// TODO: clean up (key/shape locals give the target's two loads before the two stores)
extern "C" void fn_802FCBAC(hkAgent3ProcessInput& in, hkAgent1nMachineTimTemp& temp, hkAgent3ProcessInput& out) {
    hkSweptTransformUtil::lerp2(in.m_bodyA->getMotionState()->m_sweptTransform, in.m_input->m_stepInfo.m_startTime,
                                temp.m_transformA);
    hkSweptTransformUtil::lerp2(in.m_bodyB->getMotionState()->m_sweptTransform, in.m_input->m_stepInfo.m_startTime,
                                temp.m_transformB);
    out.m_bodyA = &temp.m_bodyA;
    out.m_bodyB = &temp.m_bodyB;
    out.m_contactMgr = in.m_contactMgr;
    out.m_input = in.m_input;
    u32 key = in.m_bodyA->m_shapeKey;
    hkShape* shape = in.m_bodyA->m_shape;
    temp.m_bodyA.m_shape = shape;
    temp.m_bodyA.m_shapeKey = key;
    new (&temp.m_bodyA) hkCdBody(in.m_bodyA, &temp.m_transformA);
    new (&temp.m_bodyB) hkCdBody(in.m_bodyB, &temp.m_transformB);
    out.m_aTb.setMulInverseMul(temp.m_transformA, temp.m_transformB);
}

// hkAgentMachineUtil.o (0x802FEA60-0x802FEDA0, carved into this unit): memory statistics of the agent tracks.
#include <havok/hkAgentNnMachine.h>

#include <havok/hkStatisticsCollector.h>

// Draft (both functions): the target dispatches the stream command through a jump table whose four non-agent
// entries go to the advance; every switch spelling here gives a compare chain.
// The sectors of an Nn track, then each agent entry's agent (CALL_AGENT entries) and contact manager.
extern "C" void fn_802FEB48(const hkAgentNnTrack& track, hkStatisticsCollector* collector) {
    int i;
    for (i = 0; i < track.m_sectors.m_size - 1; i++) {
        collector->addArray("Sector", 8, track.m_sectors[i], track.m_sectorSize, track.m_sectorSize);
    }
    if (i < track.m_sectors.m_size) {
        collector->addArray("Sector", 8, track.m_sectors[i], track.m_bytesUsedInLastSector,
                            track.m_sectorSize);
    }
    for (int s = 0; s < track.m_sectors.m_size;) {
        hkAgentNnEntry* entry = (hkAgentNnEntry*)track.m_sectors[s];
        s++;
        hkAgentNnEntry* end = (s == track.m_sectors.m_size)
                                  ? (hkAgentNnEntry*)((u8*)entry + track.m_bytesUsedInLastSector)
                                  : (hkAgentNnEntry*)((u8*)entry + track.m_sectorSize);
        while (entry < end) {
            switch (entry->m_streamCommand) {
            case HK_AGENT_STREAM_CALL_AGENT:
                // TODO: clean up: CALL_AGENT entries keep the agent pointer at 0x04; name it in hkAgentNnEntry.
                collector->addChildObject("Agent", 8, *(void**)entry->unk4);
            case HK_AGENT_STREAM_CALL:
            case HK_AGENT_STREAM_CALL_WITH_TIM:
                collector->addChildObject("ContactMgr", 8, entry->m_contactMgr);
                break;
            case HK_AGENT_STREAM_NULL:
            case HK_AGENT_STREAM_END:
            case HK_AGENT_STREAM_CALL_FLIP:
            case HK_AGENT_STREAM_CALL_WITH_TIM_FLIP:
            default:
                break;
            }
            entry = (hkAgentNnEntry*)((u8*)entry + entry->m_size);
        }
    }
}

// Only the contact managers of an Nn track.
extern "C" void fn_802FECD0(const hkAgentNnTrack& track, hkStatisticsCollector* collector) {
    for (int s = 0; s < track.m_sectors.m_size;) {
        hkAgentNnEntry* entry = (hkAgentNnEntry*)track.m_sectors[s];
        s++;
        hkAgentNnEntry* end = (s == track.m_sectors.m_size)
                                  ? (hkAgentNnEntry*)((u8*)entry + track.m_bytesUsedInLastSector)
                                  : (hkAgentNnEntry*)((u8*)entry + track.m_sectorSize);
        while (entry < end) {
            switch (entry->m_streamCommand) {
            case HK_AGENT_STREAM_CALL:
            case HK_AGENT_STREAM_CALL_WITH_TIM:
            case HK_AGENT_STREAM_CALL_AGENT:
                collector->addChildObject("ContactMgr", 8, entry->m_contactMgr);
                break;
            case HK_AGENT_STREAM_NULL:
            case HK_AGENT_STREAM_END:
            case HK_AGENT_STREAM_CALL_FLIP:
            case HK_AGENT_STREAM_CALL_WITH_TIM_FLIP:
            default:
                break;
            }
            entry = (hkAgentNnEntry*)((u8*)entry + entry->m_size);
        }
    }
}
