// Havok translation unit hkBallSocketChainData.o (main.dol 0x802DA49C-0x802DAA68).
// Functions in address order (method names from the Havok TU map):
//   0x802DA49C    52  finishLoadedObjecthkBallSocketChainData   [map: hkBallSocketChainData__finishLoadedObjecthkBallSocketChainData]
//   0x802DA4D0    20  cleanupLoadedObjecthkBallSocketChainData   [map: hkBallSocketChainData__cleanupLoadedObjecthkBallSocketChainData]
//   0x802DA4E4    72  getVtablehkBallSocketChainData   [map: hkBallSocketChainData__getVtablehkBallSocketChainData]
//   0x802DA52C    88  __ct   [map: hkBallSocketChainData____ct]
//   0x802DA584   148  __dt   [map: hkBallSocketChainData____dt]
//   0x802DA618     8  getType   [map: hkBallSocketChainData__getType]
//   0x802DA620   100  getConstraintInfo   [map: hkBallSocketChainData__getConstraintInfo]
//   0x802DA684    56  getRuntimeInfo   [map: hkBallSocketChainData__getRuntimeInfo]
//   0x802DA6BC   940  buildJacobian   [map: hkBallSocketChainData__buildJacobian]
#include <havok/hkBallSocketChainData.h>

void hkBallSocketChainData::finishLoadedObjecthkBallSocketChainData(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkBallSocketChainData(flag);
}

const void* hkBallSocketChainData::getVtablehkBallSocketChainData() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x40] __attribute__((aligned(16)));
    ::new (buf) hkBallSocketChainData(flag);
    return *(const void**)buf;
}

hkBallSocketChainData::hkBallSocketChainData(hkFinishLoadedObjectFlag flag) : hkConstraintData(flag), m_chain(flag) {
    m_bridgeAtom.init(m_bridgeAtom.m_constraintData);
    m_bridgeAtom.init(this);
}

hkBallSocketChainData::~hkBallSocketChainData() {
}

void cleanupLoadedObjecthkBallSocketChainData(hkBallSocketChainData* self) {
    ((hkBaseObject*)self)->~hkBaseObject();
}

int hkBallSocketChainData::getType() const {
    return 101; // CONSTRAINT_TYPE_BALL_SOCKET_CHAIN
}

// Chain info block: six words. HYPOTHESIS: unk10 points at the chain header block (this + 0xC, 0xC bytes long).
void hkBallSocketChainData::getConstraintInfo(hkConstraintInfo* info) const {
    ((hkConstraintInfoWithAtoms*)info)->unk10 = (void*)&m_bridgeAtom;
    ((hkConstraintInfoWithAtoms*)info)->unk14 = (u32)((u8*)&m_chain - (u8*)&m_bridgeAtom);
    info->m_maxSizeOfJacobians = 0;
    info->m_sizeOfJacobians = 0;
    info->m_numSolverResults = 0;
    info->m_sizeOfSchemas = 0x18;
    s32 n = m_chain.m_size;
    info->m_numSolverResults = 3 * n;
    info->m_sizeOfSchemas = 4 * (n + 1) + 0x28;
    info->m_sizeOfJacobians = 16 * (n + 1) + 0x90 * n;
}

void hkBallSocketChainData::getRuntimeInfo(const hkBool* hasChain, hkConstraintRuntimeInfo* out) const {
    if (hasChain->m_bool != 0) {
        s32 n = m_chain.m_size;
        s32 t = 3 * n;
        out->unk04 = t;
        out->unk00 = t << 3;
    } else {
        out->unk04 = 0;
        out->unk00 = 0;
    }
}

// Not yet decompiled in this unit:
//   0x802DA49C    52  finishLoadedObjecthkBallSocketChainData   [map: hkBallSocketChainData__finishLoadedObjecthkBallSocketChainData]
//   0x802DA4E4    72  getVtablehkBallSocketChainData   [map: hkBallSocketChainData__getVtablehkBallSocketChainData]
//   0x802DA52C    88  __ct   [map: hkBallSocketChainData____ct]
//   0x802DA584   148  __dt   [map: hkBallSocketChainData____dt]
//   0x802DA6BC   940  buildJacobian   [map: hkBallSocketChainData__buildJacobian]

#include <types.h>
#include <havok/hkMemory.h>
#include <havok/hkThreadMemory.h>
#include <havok/hkArray.h>
namespace jj_fn_802DAA68 {
class SoHeapAllocator {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void free(void*, unsigned short, int);
};


struct Array {
    void* data;
    int size;
    unsigned int raw;

    bool isOwned() const {
        return (raw & 0x80000000) == 0;
    }

    void dealloc() {
        hkThreadMemory::s_instance->deallocateChunk(data, raw << 2, 0x15);
    }

    ~Array() {
        if (isOwned())
            dealloc();
    }
};

// TODO: cleanup (shape from jj-link/brawl fn_802DAA68)
extern "C" Array* fn_802DAA68(Array* arg1, int arg2) {
    if (arg1) {
        if (arg1) {
            if (arg1->isOwned())
                arg1->dealloc();
        }
        if (arg2 > 0)
            ((SoHeapAllocator*)hkMemory::s_instance)->free(arg1, 0x8C, 0x15);
    }
    return arg1;
}
} // namespace jj_fn_802DAA68

#include <types.h>
#include <havok/hkBallSocketChainData.h>
void cleanupLoadedObjecthkBallSocketChainData(hkBallSocketChainData* self);
#include <havok/hkConstraintAtom.h>

// fn_802DAAF8: body from https://github.com/jj-link/brawl src/havok/fn_802DAAF8.cpp (namespaced).
namespace jj_fn_802DAAF8 {
// TODO: cleanup (shape from jj-link/brawl fn_802DAAF8)
extern "C" u32 fn_802DAAF8(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x1c);
}
} // namespace jj_fn_802DAAF8

// fn_802DAB00: body from https://github.com/jj-link/brawl src/havok/fn_802DAB00.cpp (namespaced).
namespace jj_fn_802DAB00 {
typedef signed long s32;

// TODO: cleanup (shape from jj-link/brawl fn_802DAB00)
extern "C" s32 fn_802DAB00(void) {
    return 0x01000000;
}
} // namespace jj_fn_802DAB00

// fn_802DAB08: body from https://github.com/jj-link/brawl src/havok/fn_802DAB08.cpp (namespaced).
namespace jj_fn_802DAB08 {
extern "C" {
    extern char lbl_804121D8[];
}

struct Holder {
    void* unk0;
    void* unk4;
    void* unk8;
    int unkC;
};

extern "C" Holder lbl_80532E40;

// TODO: cleanup (shape from jj-link/brawl fn_802DAB08)
extern "C" void fn_802DAB08() {
    int temp_r3;

    temp_r3 = (int)hkBallSocketChainData::getVtablehkBallSocketChainData();
    lbl_80532E40.unk0 = lbl_804121D8;
    lbl_80532E40.unk4 = (void*)hkBallSocketChainData::finishLoadedObjecthkBallSocketChainData;
    lbl_80532E40.unk8 = (void*)::cleanupLoadedObjecthkBallSocketChainData;
    lbl_80532E40.unkC = temp_r3;
}

__declspec(section ".ctors") extern void* const fn_802DAB08_ctor = (void*)fn_802DAB08;
} // namespace jj_fn_802DAB08
