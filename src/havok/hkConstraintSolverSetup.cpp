// Havok translation unit hkConstraintSolverSetup.o (main.dol 0x802DF6E4-0x802E0E40).
// Not yet decompiled. Functions in address order:
//   0x802DF6E4    28  integrate
//   0x802DF700   192  oneStepIntegrate
//   0x802DF7C0  2528  solve
//   0x802E01A0   356  initializeSolverState
//   0x802E0304     4  shutdownSolver
//   0x802E0308   916  internalAddAccumulators
//   0x802E069C   828  internalAddJacobianElements
//   0x802E09D8    60  internalIsMemoryOkForNewAccumulators
//   0x802E0A14   880  internalIsMemoryOkForNewJacobianElements
//   0x802E0D84   188  subSolve

#include <havok/hkConstraintSolverSetup.h>
#include <havok/hkRigidMotionUtil.h>
#include <havok/hkEntity.h>

void hkConstraintSolverSetup::integrate(void* a, void* b, void* c) {
    hkRigidMotionUtil::hkRigidMotionUtilApplyAccumulators(*(const hkStepInfo*)this,
                                                          (hkVelocityAccumulator*)((u8*)c + 0x80), (hkEntity**)a,
                                                          (int)b, 0xa0, a);
}

void hkConstraintSolverSetup::oneStepIntegrate(hkEntity** entities, int count, u8* base) {
    for (int i = 0; i < count; i++) {
        hkEntity* entity = entities[i];
        u8* state = base + entity->m_solverData;
        float* src = (float*)(state + 0x10);
        float* dst = (float*)(state + 0x40);
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        dst[4] = src[4];
        dst[5] = src[5];
        dst[6] = src[6];
        dst[7] = src[7];
        void* record[2];
        record[0] = (u8*)entity + 0xa0;
        hkRigidMotionUtil::hkRigidMotionUtilApplyAccumulators(*(const hkStepInfo*)this, (hkVelocityAccumulator*)state,
                                                              (hkEntity**)record, 1, 0, (u8*)entity + 0xa0);
    }
}

void hkConstraintSolverSetup::shutdownSolver() {}

// Function body below is taken from https://github.com/jj-link/brawl (src/havok/fn_802E09D8.cpp).
// hkConstraintSolverSetup::internalIsMemoryOkForNewAccumulators
// TODO: cleanup (shape from jj-link/brawl fn_802E09D8)
extern "C" int fn_802E09D8(const char* o, int, int n) {
    unsigned int a = *(const unsigned int*)(o + 0x4C);
    unsigned int b = *(const unsigned int*)(o + 0xC);
    int d = *(const int*)(o + 0x10) - (int)a;
    if (a == b) n++;
    if ((n << 7) > d - 0x10) return 0;
    return 0x1000000;
}

// fn_802E0D84: body from https://github.com/jj-link/brawl src/havok/fn_802E0D84.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802E0D84 {
extern "C" void fn_802987AC(void*, u32, u32, u32, u32);
extern "C" f32 lbl_805A4310;

struct Fn802E0D84Obj {
    u8 pad0[0x4];
    void* unk4;
    u8 pad8[0x4];
    u32 unkC;
    u8 pad10[0xC];
    u32 unk1C;
    u8 pad20[0x4];
    u32 unk24;
    u8 pad28[0x4];
    u32 unk2C;
    u8 pad30[0x4];
    u32 unk34;
    u32 unk38;
    u32 unk3C;
    u8 pad40[0x4];
    u32 unk44;
    u32 unk48;
    u32 unk4C;
    u32 unk50;
    f32* unk54;
    f32* unk58;
};

// TODO: cleanup (shape from jj-link/brawl fn_802E0D84)
extern "C" void fn_802E0D84(Fn802E0D84Obj* self, s32 arg1) {
    f32* p;
    f32* end;
    f32 val;

    if (arg1 == 0) {
        self->unk38 = self->unk2C;
        self->unk48 = self->unk3C;
    }
    p = self->unk58;
    val = lbl_805A4310;
    while (p < (end = self->unk54)) {
        *p = val;
        p++;
    }
    self->unk58 = end;
    if (self->unk38 != self->unk34) {
        fn_802987AC(self->unk4, self->unk38, self->unkC, self->unk1C, self->unk24);
    }
    if (self->unk48 != self->unk44) {
        fn_802987AC(self->unk4, self->unk48, self->unkC, self->unk1C, self->unk24);
    }
    self->unk38 = self->unk34;
    self->unk48 = self->unk44;
}
} // namespace jj_fn_802E0D84

static inline char* hkSolverAlign16(char* p) {
    return (char*)(((u32)p + 15) & ~15);
}

// Splits the solver buffer into its regions (see hkConstraintSolverResources) and resets the write cursors.
// Draft: same instructions as the target, scheduling and register choice still differ.
void hkConstraintSolverSetup::initializeSolverState(const hkStepInfo& stepInfo, const hkSolverInfo& solverInfo,
                                                    const hkConstraintQueryIn& constraintQueryIn, char* buffer,
                                                    int bufferSize, hkConstraintSolverResources& s) {
    int size = bufferSize - bufferSize % 16;
    char* p = hkSolverAlign16(buffer);
    s.m_region0Begin = p;
    s.m_stepInfo = &stepInfo;
    s.m_solverInfo = &solverInfo;
    s.m_constraintQueryIn = &constraintQueryIn;
    p = hkSolverAlign16(p + size * 10 / 64);
    s.m_region0End = p;
    s.m_region1Begin = p;
    p = hkSolverAlign16(p + size * 4 / 64);
    s.m_region1End = p;
    s.m_region2Begin = p;
    p = hkSolverAlign16(p + size * 30 / 64);
    s.m_region2End = p;
    s.m_region3Begin = p;
    p = hkSolverAlign16(p + size * 10 / 64);
    s.m_region3End = p;
    s.m_region4Begin = p;
    p = hkSolverAlign16(p + (u32)(0.6f * (size * 10) / 64.0f));
    s.m_region4End = p;
    s.m_region5Begin = p;
    s.m_region5End = buffer + size;
    s.m_region0Cursor = s.m_region0Begin;
    s.m_region2Cursor = s.m_region2Begin;
    s.m_region3Cursor = s.m_region3Begin;
    s.m_region3Cursor2 = s.m_region3Begin;
    s.m_region4Cursor = s.m_region4Begin;
    s.m_region4Cursor2 = s.m_region4Begin;
    s.m_region5Cursor = s.m_region5Begin;
    s.m_region5Cursor2 = s.m_region5Begin;
}
