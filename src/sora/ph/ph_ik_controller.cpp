#include <types.h>
// Brawl physics wrapper translation unit ph_ik_controller.o (main.dol 0x800966EC-0x800985D8).
// Not yet decompiled. Functions in address order with their map names:
//   0x800966EC   136  __ct
//   0x80096774   600  __dt
//   0x800969CC    20  setEnable
//   0x800969E0  1488  calacCallBackTimingCForIKCharacter
//   0x80096FB0  2112  createIKSolverInit
//   0x800977F0  1188  updatePose
//   0x80097C94   512  setHeelPoint
//   0x80097E94   512  setToePoint
//   0x80098094   204  setNormal
//   0x80098160    40  setStressReferencePoint
//   0x80098188   512  setGain
//   0x80098388   496  setToeAngle
//   0x80098578    16  resetToeAngle
//   0x80098588    80  setSolverParamData
#include <ph/phIkController.h>
#include <ph/phIkTwoJointSolver.h>

void phIkController::setEnable(int idx, bool enable) {
    m_solvers[idx]->m_enable = enable;
}

void phIkController::setStressReferencePoint(int idx, const f32* pos) {
    phIkTwoJointSolver* solver = m_solvers[idx];
    solver->m_stressRef[0] = pos[0];
    solver->m_stressRef[1] = pos[1];
    solver->m_stressRef[2] = pos[2];
}

void phIkController::resetToeAngle(int idx) {
    m_solvers[idx]->applyParam();
}

void phIkController::setSolverParamData(int idx, const f32* data) {
    m_solvers[idx]->setSolverParamData(data);
    m_solvers[idx]->savedParam();
}

// TODO: clean up, give variables real names
namespace jj_800966EC {
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void* fn_800800D8(void*);
extern "C" void fn_80080118(void*, int);

extern "C" void* fn_800966EC(void* self, int arg) {
    void* p;
    *(unsigned char*)((char*)self + 0x0) = 1;
    *(int*)((char*)self + 0x4) = 0;
    *(int*)((char*)self + 0x8) = 0;
    *(int*)((char*)self + 0xC) = 0;
    *(unsigned int*)((char*)self + 0x10) = 0x80000000;
    *(int*)((char*)self + 0x14) = 0;
    *(int*)((char*)self + 0x18) = 0;
    *(unsigned int*)((char*)self + 0x1C) = 0x80000000;
    p = __nw__FUlQ25Heaps8HeapType(0x40, 0xD);
    if (p != 0) {
        p = fn_800800D8(p);
    }
    *(void**)((char*)self + 0x4) = p;
    fn_80080118(p, arg);
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_80098188 {
struct S {
    int a;
    short b, c, d, e;
    float f0 __attribute__((aligned(16)));
    float f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19;
    S() {}
    S(const S& o) { *this = o; }
    S& operator=(const S& o) {
        a = o.a; b = o.b; c = o.c; d = o.d; e = o.e;
        f0 = o.f0; f1 = o.f1; f2 = o.f2; f3 = o.f3; f4 = o.f4; f5 = o.f5;
        f6 = o.f6; f7 = o.f7; f8 = o.f8; f9 = o.f9; f10 = o.f10; f11 = o.f11;
        f12 = o.f12; f13 = o.f13; f14 = o.f14; f15 = o.f15; f16 = o.f16;
        f17 = o.f17; f18 = o.f18; f19 = o.f19;
        return *this;
    }
};
struct Obj { char pad[0x10]; S s; };
struct Mgr { char pad[8]; Obj** objs; };

extern "C" void fn_80098188(Mgr* m, int i, float v) {
    Obj* o = m->objs[i];
    S tmp = o->s;
    tmp.f6 = v;
    tmp.f7 = v;
    o->s = tmp;
}
}

