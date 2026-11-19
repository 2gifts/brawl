#include <types.h>
#include <nw4r/math/math_types.h>
// Brawl physics wrapper translation unit ph_2ndary_constraint.o (main.dol 0x80081CE8-0x80082438).
// Functions in address order with their map names:
//   0x80081CE8   220  __dt
//   0x80081DC4   284  updateMatrixForMyPosture
//   0x80081EE0   284  updateMatrixForMotion
//   0x80081FFC   308  updateMatrix
//   0x80082130   776  setLocalSpace (not yet decompiled)
#include <ph/ph_2ndary_constraint.h>

#include <havok/hkThreadMemory.h>

ph2ndaryLine::~ph2ndaryLine() {
    if (m_block != NULL && m_block->m_parent == NULL) {
        delete m_block;
        m_block = NULL;
    }
    if (m_array.mustDeallocate()) {
        hkThreadMemory::s_instance->deallocateChunk(m_array.m_data, (m_array.m_capacityAndFlags & hkArrayBase::CAPACITY_MASK) * 4, 0x15);
    }
    m_array.m_data = NULL;
    m_array.m_size = 0;
    m_array.m_capacityAndFlags = (m_array.m_capacityAndFlags & hkArrayBase::FORCE_SIGN_FLAG) |
                                 hkArrayBase::DONT_DEALLOCATE_FLAG;
}

void ph2ndaryLine::updateMatrixForMyPosture() {
    updateMatrix(&unkDC, &unk34, &unk40);
}

void ph2ndaryLine::updateMatrixForMotion(const nw4r::math::MTX34* mtx) {
    updateMatrix(mtx, &unk1C, &unk28);
}

void ph2ndaryLine::updateMatrix(const nw4r::math::MTX34* mtx, nw4r::math::VEC3* tip, nw4r::math::VEC3* root) {
    nw4r::math::MTX34 tipMtx(1.0f, 0.0f, 0.0f, unk80, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    nw4r::math::MTX34 rootMtx(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    PSMTXConcat(*mtx, tipMtx, tipMtx);
    PSMTXConcat(*mtx, rootMtx, rootMtx);
    tip->x = tipMtx._03;
    tip->y = tipMtx._13;
    tip->z = tipMtx._23;
    root->x = rootMtx._03;
    root->y = rootMtx._13;
    root->z = rootMtx._23;
    unk84 = nw4r::math::VEC3Len(&(*tip - *root));
}

// TODO: clean up, give variables real names
namespace jj_80082130 {
using namespace nw4r::math;
extern "C" void fn_801EC158(void*);
extern "C" void fn_801EC41C(const void*, void*);
extern "C" void fn_801EC1B8(const void*, const void*, void*);
extern float lbl_805A1BB4;
typedef union { float f; unsigned int i; } FloatBits;
#define IS_NAN(u, x) ((u.f = (x)), ((u.i >> 23 & 255) == 255) && (u.i & 0x7fffff))
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_80082130(char* self) {
    FloatBits u11, u10, u9, u8, u7, u6, u5, u4, u3, u2, u1, u0;
    _MTX34 ident;
    _MTX34 m;
    fn_801EC158(&ident);
    if (lbl_805A1BB4 != *(float*)(self + 0xb0)) {
        fn_801EC41C(*(char**)(*(char**)(self + 0x1f8)) + 0x13c, &m);
        int bad = IS_NAN(u0, m.a[0]);
        if (!bad) bad = IS_NAN(u1, m.a[1]);
        if (!bad) bad = IS_NAN(u2, m.a[2]);
        if (!bad) bad = IS_NAN(u3, m.a[3]);
        if (!bad) bad = IS_NAN(u4, m.a[4]);
        if (!bad) bad = IS_NAN(u5, m.a[5]);
        if (!bad) bad = IS_NAN(u6, m.a[6]);
        if (!bad) bad = IS_NAN(u7, m.a[7]);
        if (!bad) bad = IS_NAN(u8, m.a[8]);
        if (!bad) bad = IS_NAN(u9, m.a[9]);
        if (!bad) bad = IS_NAN(u10, m.a[10]);
        if (!bad) bad = IS_NAN(u11, m.a[11]);
        if (bad) {
            *(_MTX34*)(self + 0x16c) = ident;
        } else {
            fn_801EC1B8(&m, self + 0x13c, self + 0x16c);
        }
    }
}
}

