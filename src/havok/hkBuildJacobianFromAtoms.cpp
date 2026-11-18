#include <types.h>
// Havok translation unit hkBuildJacobianFromAtoms.o (main.dol 0x80289D0C-0x8028D8A0): the jacobian builders of the
// constraint atoms, each after its atom's numSolverResults.
// Not yet decompiled. Functions in address order:
//   0x80289D0C     8  numSolverResults (hkSetLocalTranslationsConstraintAtom)
//   0x80289D14   812  setLocalRotationsFromAtom
//   0x8028A040     8  numSolverResults (hkSetLocalRotationsConstraintAtom)
//   0x8028A048  1252  buildJacobianFromBallSocketAtom
//   0x8028A52C     8  numSolverResults
//   0x8028A534   444  fn_8028A534
//   0x8028A6F0     8  numSolverResults
//   0x8028A6F8   168  fn_8028A6F8
//   0x8028A7A0     8  numSolverResults
//   0x8028A7A8   184  buildJacobianFromLinSoftAtom
//   0x8028A860     8  numSolverResults
//   0x8028A868   172  buildJacobianFromLinLimitAtom
//   0x8028A914     8  numSolverResults
//   0x8028A91C   204  buildJacobianFromLinFrictionAtom
//   0x8028A9E8    32  hkSkipSolverResults
//   0x8028AA08     8  numSolverResults
//   0x8028AA10   456  buildJacobianFromLinMotorAtom
//   0x8028ABD8     8  numSolverResults
//   0x8028ABE0   320  buildJacobianFrom2dAngAtom
//   0x8028AD20     8  numSolverResults
//   0x8028AD28   268  buildJacobianFromAngAtom
//   0x8028AE34     8  numSolverResults
//   0x8028AE3C   684  buildJacobianFromAngLimitAtom
//   0x8028B0E8     8  numSolverResults
//   0x8028B0F0   568  buildJacobianFromConeLimitAtom
//   0x8028B328     8  numSolverResults
//   0x8028B330   680  bulidJacobianFromTwistLimitAtom
//   0x8028B5D8     8  numSolverResults
//   0x8028B5E0   140  buildJacobianFromAngFrictionAtom
//   0x8028B66C     8  numSolverResults
//   0x8028B674  1620  buildJacobianFromAngMotorAtom
//   0x8028BCC8     8  numSolverResults
//   0x8028BCD0  2472  buildJacobianFromRagdollMotorAtom
//   0x8028C678     8  numSolverResults
//   0x8028C680  2340  buildJacobianFromPulleyAtom
//   0x8028CFA4     8  numSolverResults
//   0x8028CFAC   472  buildJacobianFromSoftContactModifier
//   0x8028D184     8  numSolverResults (hkSoftContactModifierConstraintAtom)
//   0x8028D18C   736  fn_8028D18C
//   0x8028D46C     8  numSolverResults (hkMassChangerModifierConstraintAtom)
//   0x8028D474   340  fn_8028D474
//   0x8028D5C8     8  fn_8028D5C8
//   0x8028D5D0   712  fn_8028D5D0
//   0x8028D898     8  fn_8028D898

#include <havok/hkSetLocalTranslationsConstraintAtom.h>
#include <havok/hkSetLocalRotationsConstraintAtom.h>
#include <havok/hkLinSoftConstraintAtom.h>
#include <havok/hkConstraintAtom.h>
#include <havok/hkConstraintQueryIn.h>
#include <havok/hkJacobianSchema.h>
#include <havok/hkRotation.h>

int hkSetLocalTranslationsConstraintAtom::numSolverResults() const {
    return 0;
}

int hkSetLocalRotationsConstraintAtom::numSolverResults() const {
    return 0;
}

// Helper in another TU (not yet named): takes the scratch block, the caller's context and a pass-through pointer.
extern "C" void fn_8028E8E8(void* scratch, void* ctx, void* pass);

struct hkLinSoftAtomScratch {
    hkVector4 m_a; // 0x00
    hkVector4 m_b; // 0x10
    hkVector4 m_c; // 0x20
    float m_d;     // 0x30
    float m_e;     // 0x34
};

void hkLinSoftConstraintAtom::buildJacobianFromLinSoftAtom(void* a, const hkVector4* b, const hkVector4* table, void* d) {
    hkLinSoftAtomScratch s;
    s.m_a = b[3];
    s.m_b = table[3];
    s.m_c = table[m_axisIndex];
    s.m_d = m_tau;
    s.m_e = m_damping;
    fn_8028E8E8(&s, a, d);
}

// Schema that makes the solver step over solver results nobody writes (8 bytes).
// HYPOTHESIS: the names; 0x13 is the schema type and 2 a flag of the header word.
struct hkJacobianSkipSchema : hkJacobianSchema {
    u8 m_numSolverResults; // 0x04
};

enum {
    HK_JACOBIAN_SKIP_SCHEMA_TAG = 0x13020008,
};

void hkSkipSolverResults(int numSolverResults, hkConstraintQueryOut& out) {
    hkJacobianSkipSchema* schema = (hkJacobianSkipSchema*)(hkJacobianSchema*)out.m_jacobianSchemas;
    schema->m_tag = HK_JACOBIAN_SKIP_SCHEMA_TAG;
    schema->m_numSolverResults = numSolverResults;
    out.m_jacobianSchemas = (hkJacobianSchema*)(schema + 1);
}

// Input of the angular friction rows (built on the stack by the atom builder).
// HYPOTHESIS: the names.
struct hk1dAngularFrictionInfo {
    const hkVector4* m_constrainedDofW; // 0x00 first friction axis (a column of base A)
    void* m_solverResults;              // 0x04 from the query input
    hkReal m_maxFrictionTorque;         // 0x08
    int m_numFriction;                  // 0x0C
};

// Builds the friction rows of the angular friction axes (fn_8028DC9C). HYPOTHESIS: the name.
void hk1dAngularFrictionBuildJacobian(const hk1dAngularFrictionInfo& info, const hkConstraintQueryIn& in,
                                      hkConstraintQueryOut& out);

// Without friction the atom's solver results are skipped.
void hkAngFrictionConstraintAtom::buildJacobianFromAngFrictionAtom(const hkConstraintQueryIn& in,
                                                                   const hkRotation& baseA, const hkRotation& baseB,
                                                                   hkConstraintQueryOut& out) const {
    if (m_isEnabled && m_maxFrictionTorque != 0.0f) {
        hk1dAngularFrictionInfo info;
        info.m_constrainedDofW = &(&baseA.m_col0)[m_firstFrictionAxis];
        info.m_maxFrictionTorque = m_maxFrictionTorque;
        info.m_numFriction = m_numFrictionAxes;
        info.m_solverResults = in.unk44;
        hk1dAngularFrictionBuildJacobian(info, in, out);
    } else {
        hkSkipSolverResults(m_numFrictionAxes, out);
    }
}

int hkLinSoftConstraintAtom::numSolverResults() const {
    return 1;
}

int hkLinLimitConstraintAtom::numSolverResults() const {
    return 1;
}

int hkLinFrictionConstraintAtom::numSolverResults() const {
    return 1;
}

int hkLinMotorConstraintAtom::numSolverResults() const {
    return 1;
}

int hk2dAngConstraintAtom::numSolverResults() const {
    return 2;
}

int hkAngConstraintAtom::numSolverResults() const {
    return m_numConstrainedAxes;
}

int hkAngLimitConstraintAtom::numSolverResults() const {
    return 1;
}

int hkConeLimitConstraintAtom::numSolverResults() const {
    return 1;
}

int hkTwistLimitConstraintAtom::numSolverResults() const {
    return 1;
}

int hkAngFrictionConstraintAtom::numSolverResults() const {
    return m_numFrictionAxes;
}

int hkAngMotorConstraintAtom::numSolverResults() const {
    return 1;
}

int hkRagdollMotorConstraintAtom::numSolverResults() const {
    return 3;
}

int hkPulleyConstraintAtom::numSolverResults() const {
    return 1;
}

int hkSoftContactModifierConstraintAtom::numSolverResults() const {
    return 0;
}

int hkMassChangerModifierConstraintAtom::numSolverResults() const {
    return 0;
}

// TODO: clean up, give variables real names
extern "C" int fn_8028A52C(void) {
    return 3;
}

extern "C" int fn_8028A6F0(void) {
    return 1;
}

extern "C" int fn_8028A7A0(void) {
    return 0x1;
}

extern "C" int fn_8028D5C8(void) {
    return 0;
}

extern "C" int fn_8028D898(void) {
    return 0;
}

// TODO: clean up, give variables real names
namespace jj_8028A91C {
struct Vec4 { f32 x, y, z, w; };
struct Marker { u32 tag; u8 kind; u8 pad[3]; };
struct Buffer { u32 x0; u32 x4; struct Marker *cursor; };
struct A { char pad0[2]; u8 flag; u8 idx; f32 t; };
struct B { char pad[0x44]; void *ptr; };
struct C { char pad[0x30]; struct Vec4 v; };
struct Local { struct Vec4 a; struct Vec4 b; f32 s; void *p; };

extern f32 lbl_805A3D90;
extern void fn_8028E080(struct Local *loc, struct B *b, struct Buffer *e);

extern "C" void fn_8028A91C(struct A *a, struct B *b, struct C *c, struct Vec4 *d, struct Buffer *e) {
    f32 d3, d2, d1, d0, c3, c2, c1, c0, t;
    if (a->flag != 0 && (t = a->t) > lbl_805A3D90) {
        struct Local loc __attribute__((aligned(16)));
        struct Vec4 *v = d + a->idx;
        void *p;
        c0 = c->v.x;
        c1 = c->v.y;
        c2 = c->v.z;
        c3 = c->v.w;
        d0 = v->x;
        d1 = v->y;
        d2 = v->z;
        d3 = v->w;
        p = b->ptr;
        loc.a.x = c0;
        loc.a.y = c1;
        loc.a.z = c2;
        loc.a.w = c3;
        loc.b.x = d0;
        loc.b.y = d1;
        loc.b.z = d2;
        loc.b.w = d3;
        loc.s = t;
        loc.p = p;
        fn_8028E080(&loc, b, e);
        return;
    }
    {
        struct Marker *m = e->cursor;
        m->tag = 0x13020008;
        m->kind = 1;
        e->cursor = m + 1;
    }
}
}

namespace jj_8028AD28 {
typedef struct Vec4 { float x, y, z, w; } Vec4;
typedef struct Triangle { Vec4 b, a, c; } __attribute__((aligned(16))) Triangle;
extern const unsigned char lbl_8040F958[12];
extern void fn_8028D8C0(Triangle *, void *, void *);

extern "C" void fn_8028AD28(unsigned char *ctx, void *arg2, const void *arg3,
                 const void *arg4, void *arg5, int i)
{
    Triangle t;
    const unsigned char *base = lbl_8040F958;
    for (i = ctx[2]; i < ctx[2] + ctx[3]; ++i) {
        const Vec4 *a, *b, *c;
        b = (const Vec4 *)((const char *)arg3 + ((const unsigned char *)(i + (unsigned int)base))[1]);
        c = (const Vec4 *)((const char *)arg4 + ((const unsigned char *)(i + (unsigned int)base))[2]);
        a = (const Vec4 *)((const char *)arg3 + base[i]);
        t.a.x = a->x; t.a.y = a->y; t.a.z = a->z; t.a.w = a->w;
        t.b.x = b->x; t.b.y = b->y; t.b.z = b->z; t.b.w = b->w;
        t.c.x = c->x; t.c.y = c->y; t.c.z = c->z; t.c.w = c->w;
        fn_8028D8C0(&t, arg2, arg5);
    }
}
}

namespace jj_8028D474 {
typedef unsigned short u16;
typedef unsigned int u32;
typedef float f32;

typedef struct Blob {
    f32 f0, f4, f8, fC, f10, f14, f18, f1C, f20, f24;
    u32 v28, v2C, v30, v34;
    f32 f38, f3C;
    u32 v40, v44, v48, v4C, v50;
} Blob;

typedef struct Node {
    u16 kind;
    u16 pad2;
    u16 id;
    u16 pad6;
    struct Node *next;
} Node;

typedef struct Context {
    u32 a, b, c;
} Context;

extern void fn_80295148(void *);
extern void fn_80293CBC(Node *, Blob *, Context *);
extern void fn_80288BC8(Node *, u16, Blob *, Context *);

static inline void copy_blob(Blob *dst, const Blob *src)
{
    dst->f0 = src->f0;
    dst->f4 = src->f4;
    dst->f8 = src->f8;
    dst->fC = src->fC;
    dst->f10 = src->f10;
    dst->f14 = src->f14;
    dst->f18 = src->f18;
    dst->f1C = src->f1C;
    dst->f20 = src->f20;
    dst->f24 = src->f24;
    dst->v28 = src->v28;
    dst->v2C = src->v2C;
    dst->v30 = src->v30;
    dst->v34 = src->v34;
    dst->f38 = src->f38;
    dst->f3C = src->f3C;
    dst->v40 = src->v40;
    dst->v44 = src->v44;
    dst->v48 = src->v48;
    dst->v4C = src->v4C;
    dst->v50 = src->v50;
}

static inline signed char is_kind(Node *p)
{
    return p->kind == 0x16;
}

extern "C" void fn_8028D474(Node * __restrict self, const Blob *src,
                 void *a, void *b, Context *ctx)
{
    Blob local __attribute__((aligned(16)));
    Node *p;
    u16 id;

    ctx->c -= 0x18;
    copy_blob(&local, src);
    p = self->next;

    while ((int)p->kind >= 0x17) {
        p = p->next;
    }

    fn_80295148((char *)p + 8);
    p = self->next;
    id = self->id;

    if (is_kind(p)) {
        fn_80293CBC(p, &local, ctx);
    } else {
        fn_80288BC8(p, id, &local, ctx);
    }
}
}

