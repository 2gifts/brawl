#include <revolution/GX/GXGeometry.h>
#include <nw4r/math/math_types.h>
#include <types.h>
// nw4r ef_drawsmoothstripestrategy.cpp (main.dol 0x8017E24C-0x80182760). Partially reconstructed.
#include <nw4r/ef.h>

// HYPOTHESIS: constant tables in .bss (lbl_804A4408 is 0x18 bytes, lbl_804A4420
// is 0xC bytes). Their values are set at run time, so they are only declared.
extern nw4r::math::VEC3 lbl_804A4408[2];
extern nw4r::math::VEC3 lbl_804A4420;

namespace nw4r {
namespace ef {

DrawSmoothStripeStrategy::DrawSmoothStripeStrategy() {}

u16 DrawSmoothStripeStrategy::GetDrawOrder(
    const EmitterDrawSetting& rSetting) const {
    return rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER;
}

u8 DrawSmoothStripeStrategy::GetStripeTexmapType(
    const EmitterDrawSetting& rSetting) const {
    return rSetting.typeOption2 & 0xC0;
}

math::VEC3 DrawSmoothStripeStrategy::GetInitialPrevAxis(
    const EmitterDrawSetting& rSetting, const AheadContextStripe& rContext) {

    math::VEC3 axis;

    switch (rSetting.typeOption2 & 0x38) {
    case 0x08: {
        math::VEC3TransformNormal(&axis, &rContext.mCommon.mEmitterMtx,
                                  &lbl_804A4408[0]);
        math::VEC3TransformNormal(&axis,
                                  &rContext.mCommon.mParticleManagerMtxInv,
                                  &axis);
        break;
    }

    case 0x00:
    default: {
        axis = rContext.mCommon.mEmitterAxisY;
        return axis;
    }

    case 0x10: {
        math::VEC3TransformNormal(&axis, &rContext.mCommon.mEmitterMtx,
                                  &lbl_804A4420);
        math::VEC3TransformNormal(&axis,
                                  &rContext.mCommon.mParticleManagerMtxInv,
                                  &axis);
        break;
    }

    case 0x18: {
        math::VEC3 unit(1.0f, 1.0f, 1.0f);
        math::VEC3TransformNormal(&axis, &rContext.mCommon.mEmitterMtx, &unit);
        math::VEC3TransformNormal(&axis,
                                  &rContext.mCommon.mParticleManagerMtxInv,
                                  &axis);
        break;
    }
    }

    if (!Normalize(&axis)) {
        axis = rContext.mCommon.mEmitterAxisY;
    }

    return axis;
}

void DrawSmoothStripeStrategy::CalcAhead_Particle_Stripe(
    math::VEC3* pAxisY, AheadContextStripe* pContext, Particle* pParticle) {

    Particle* pYounger =
        GetYoungerDrawParticle(pContext->mCommon.mParticleManager, pParticle);

    if (pYounger != NULL) {
        math::VEC3Sub(pAxisY, &pParticle->mParameter.mPosition,
                      &pYounger->mParameter.mPosition);
    } else {
        math::VEC3Sub(pAxisY, &pParticle->mParameter.mPosition,
                      &pContext->mCommon.mEmitterCenter);
    }

    if (!Normalize(pAxisY)) {
        *pAxisY = pContext->mCommon.mEmitterAxisY;
    }
}

DrawStrategyImpl::CalcAheadFunc
DrawSmoothStripeStrategy::GetCalcAheadFunc(ParticleManager* pManager) {

    const EmitterDrawSetting& rSetting =
        *pManager->mResource->GetEmitterDrawSetting();

    switch (rSetting.typeDir) {
    case EmitterDrawSetting::AHEAD_BB_SPEED: {
        return CalcAhead_Speed;
    }

    case EmitterDrawSetting::AHEAD_BB_EMITTER_CENTER: {
        return CalcAhead_EmitterCenter;
    }

    case EmitterDrawSetting::AHEAD_BB_EMITTER_DESIGN: {
        return CalcAhead_EmitterDesign;
    }

    case EmitterDrawSetting::AHEAD_BB_PARTICLE: {
        return reinterpret_cast<CalcAheadFunc>(CalcAhead_Particle_Stripe);
    }

    case EmitterDrawSetting::AHEAD_BB_PARTICLE_BOTH: {
        return CalcAhead_ParticleBoth;
    }

    case EmitterDrawSetting::AHEAD_CMN_NODESIGN: {
        switch (rSetting.typeOption2 & 7) {
        case 1: {
            return reinterpret_cast<CalcAheadFunc>(CalcAhead_ParticleBoth_Ring);
        }

        case 2: {
            return reinterpret_cast<CalcAheadFunc>(CalcAhead_ParticleBoth_Origin);
        }

        default: {
            return reinterpret_cast<CalcAheadFunc>(CalcAhead_ParticleBoth_Stripe);
        }
        }
    }

    case 6:
    default: {
        return CalcAhead_Speed;
    }
    }
}

void DrawSmoothStripeStrategy::CalcAhead_ParticleBoth_Stripe(
    math::VEC3* pAxisY, AheadContextStripe* pContext, Particle* pParticle) {

    Particle* pElder =
        GetElderDrawParticle(pContext->mCommon.mParticleManager, pParticle);

    Particle* pYounger =
        GetYoungerDrawParticle(pContext->mCommon.mParticleManager, pParticle);

    math::VEC3 elderPos(0.0f, 0.0f, 0.0f);

    if (pElder != NULL) {
        math::VEC3Sub(&elderPos, &pElder->mParameter.mPosition,
                      &pParticle->mParameter.mPosition);

        if (!Normalize(&elderPos)) {
            elderPos = math::VEC3(0.0f, 0.0f, 0.0f);
        }
    }

    math::VEC3 youngerPos(0.0f, 0.0f, 0.0f);

    if (pYounger != NULL) {
        math::VEC3Sub(&youngerPos, &pYounger->mParameter.mPosition,
                      &pParticle->mParameter.mPosition);

        if (!Normalize(&youngerPos)) {
            youngerPos = math::VEC3(0.0f, 0.0f, 0.0f);
        }
    }

    math::VEC3Sub(pAxisY, &elderPos, &youngerPos);

    if (!Normalize(pAxisY)) {
        *pAxisY = pContext->mCommon.mEmitterAxisY;
    }
}

void DrawSmoothStripeStrategy::CalcAhead_ParticleBoth_Ring(
    math::VEC3* pAxisY, AheadContextStripe* pContext, Particle* pParticle) {

    Particle* pElder = GetElderDrawParticle(pContext->mCommon.mParticleManager, pParticle);
    if (pElder == NULL) {
        pElder = GetYoungestDrawParticle(pContext->mCommon.mParticleManager);
    }

    Particle* pYounger = GetYoungerDrawParticle(pContext->mCommon.mParticleManager, pParticle);
    if (pYounger == NULL) {
        pYounger = GetOldestDrawParticle(pContext->mCommon.mParticleManager);
    }

    math::VEC3 elderPos;
    math::VEC3Sub(&elderPos, &pElder->mParameter.mPosition,
                  &pParticle->mParameter.mPosition);

    if (!Normalize(&elderPos)) {
        elderPos = math::VEC3(0.0f, 0.0f, 0.0f);
    }

    math::VEC3 youngerPos;
    math::VEC3Sub(&youngerPos, &pYounger->mParameter.mPosition,
                  &pParticle->mParameter.mPosition);

    if (!Normalize(&youngerPos)) {
        youngerPos = math::VEC3(0.0f, 0.0f, 0.0f);
    }

    math::VEC3Sub(pAxisY, &elderPos, &youngerPos);

    if (!Normalize(pAxisY)) {
        *pAxisY = pContext->mCommon.mEmitterAxisY;
    }
}

void DrawSmoothStripeStrategy::CalcAhead_ParticleBoth_Origin(
    math::VEC3* pAxisY, AheadContextStripe* pContext, Particle* pParticle) {

    Particle* pElder = GetElderDrawParticle(pContext->mCommon.mParticleManager, pParticle);
    Particle* pYounger = GetYoungerDrawParticle(pContext->mCommon.mParticleManager, pParticle);

    math::VEC3 elderPos(0.0f, 0.0f, 0.0f);

    if (pElder != NULL) {
        math::VEC3Sub(&elderPos, &pElder->mParameter.mPosition,
                      &pParticle->mParameter.mPosition);
        Normalize(&elderPos);
    }

    math::VEC3 youngerPos;

    if (pYounger != NULL) {
        math::VEC3Sub(&youngerPos, &pYounger->mParameter.mPosition,
                      &pParticle->mParameter.mPosition);
    } else {
        math::VEC3Sub(&youngerPos, &pContext->mCommon.mEmitterCenter,
                      &pParticle->mParameter.mPosition);
    }
    Normalize(&youngerPos);

    math::VEC3Sub(pAxisY, &elderPos, &youngerPos);

    if (!Normalize(pAxisY)) {
        *pAxisY = pContext->mCommon.mEmitterAxisY;
    }
}

} // namespace ef
} // namespace nw4r

// TODO: clean up, give variables real names
extern "C" void fn_8017F894(void) {}

// TODO: clean up, give variables real names
namespace jj_8017EDB8 {

extern "C" void fn_8017EDB8(nw4r::math::VEC3* pOut, const nw4r::math::VEC3* pA,
                            const nw4r::math::VEC3* pB) {
    nw4r::math::VEC3Add(pOut, pA, pB);
}
}

namespace jj_8018058C {
// TODO: clean up, give variables real names
extern "C" void fn_8018058C(unsigned int* dst, const unsigned int* src) {
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
    dst[4] = src[4];
    dst[5] = src[5];
    *reinterpret_cast<float*>(dst + 6) = *reinterpret_cast<const float*>(src + 6);
}
}

// TODO: clean up, give variables real names
namespace jj_801811DC {

extern "C" {
void fn_80162384(float*, float*, float);
void fn_801805C8(void*, void*, int, int);
void fn_801808C0(void*, void*, int, int);
void fn_80180C74(void*, void*, int, int);

extern const float lbl_805A3074;
extern const float lbl_805A3078;
extern const float lbl_805A30A4;
extern const float lbl_805A30A8;
extern const double lbl_805A3098;
}

struct Pair {
    float x;
    float y;
};

// TODO: clean up, give variables real names
inline void setCirclePoint(Pair* p, int i, float step, float k) {
    float fi = (float)i;
    fn_80162384(&p->y, &p->x, k * (step * fi));
}

struct Inner {
    unsigned char pad[0x140];
    unsigned char count;
    unsigned char pad2;
    unsigned char mode;
};

struct Obj {
    unsigned char pad[0x24];
    Inner* inner;
};

struct Node {
    Obj* obj;
    unsigned char pad[0xd0];
    Pair* arr;
};

// TODO: clean up, give variables real names
extern "C" void fn_801811DC(void* a0, Node* a1, int a2, int a3) {
    Inner* in = a1->obj->inner;
    int n = in->count;

    if (n >= 3) {
        Pair* p;
        Pair* arr = (Pair*)__alloca((n + 1) * 8);

        if (arr) {
            float fn_ = (float)n;
            float f29 = lbl_805A30A4 / fn_;
            arr[0].x = lbl_805A3078;
            arr[0].y = lbl_805A3074;
            p = arr + 1;
            int i;

            for (i = 1; i < n; p++, i++) {
                setCirclePoint(p, i, f29, lbl_805A30A8);
            }

            arr[n].x = lbl_805A3078;
            arr[n].y = lbl_805A3074;
            a1->arr = arr;

            switch (in->mode & 7) {
            case 0:
            default:
                GXSetCullMode(GX_CULL_FRONT);
                fn_801805C8(a0, a1, a2, a3);
                GXSetCullMode(GX_CULL_BACK);
                fn_801805C8(a0, a1, a2, a3);
                break;

            case 1:
                GXSetCullMode(GX_CULL_FRONT);
                fn_801808C0(a0, a1, a2, a3);
                GXSetCullMode(GX_CULL_BACK);
                fn_801808C0(a0, a1, a2, a3);
                break;

            case 2:
                GXSetCullMode(GX_CULL_FRONT);
                fn_80180C74(a0, a1, a2, a3);
                GXSetCullMode(GX_CULL_BACK);
                fn_80180C74(a0, a1, a2, a3);
                break;
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_8017FBFC {
extern "C" void __ct__Q44nw4r2ef24DrawSmoothStripeStrategy6VertexFv(void) {}
}

// TODO: clean up, give variables real names
namespace jj_8017FC00 {
#pragma fp_contract off

extern "C" {
extern const double lbl_805A3098;
extern const float lbl_805A3090;
extern const float lbl_805A30A0;
void* GetEmitterDrawSetting__Q34nw4r2ef15EmitterResourceFv(void*);
s32 GetNumDrawParticle__Q34nw4r2ef16DrawStrategyImplFPQ34nw4r2ef15ParticleManager(void*, void*);
s32 GetDrawOrder__Q34nw4r2ef24DrawSmoothStripeStrategyCFRCQ34nw4r2ef18EmitterDrawSetting(void*, void*);
s32 GetStripeTexmapType__Q34nw4r2ef24DrawSmoothStripeStrategyCFRCQ34nw4r2ef18EmitterDrawSetting(void*, void*);
void __ct__Q44nw4r2ef24DrawSmoothStripeStrategy6VertexFv(void*);
void __ct__Q34nw4r4math4VEC3Fv(void*);
void CalcVertex__Q34nw4r2ef24DrawSmoothStripeStrategyFPQ44nw4r2ef24DrawSmoothStripeStrategy6VertexPQ44nw4r2ef24DrawSmoothStripeStrategy18AheadContextStripeiPQ34nw4r2ef8ParticlePFPQ34nw4r4math4VEC3PQ44nw4r2ef24DrawSmoothStripeStrategy18AheadContextStripePQ34nw4r2ef8Particle_vRCQ34nw4r4math4VEC3RCQ34nw4r4math4VEC3RCQ34nw4r4math4VEC3ff(void*, void*, void*, s32, void*, s32, void*, s32, f32, f32, s32);
void fn_8017EB30(void*, s32, void*, void*, void*, s32);
void fn_8017EDDC(void*, void*, void*, void*, s32);
void fn_8017F894(void);
void GXBegin(u8, u8, u16);
}

typedef void* (*Fn1)(void*);
typedef void* (*Fn2)(void*, void*);

struct VT {
    virtual void v0();
    virtual void v1();
    virtual Fn1 v2(s32);
    virtual Fn2 v3(s32);
    virtual s32 v4(void*);
};

struct Obj {
    VT* vt;
    char p0[0x20];
    void* p24;
};

struct Q {
    char p0[0x9D];
    s8 b9D;
};

extern "C" void DrawSmoothStripe_BSpline_ConnectRing__Q34nw4r2ef24DrawSmoothStripeStrategyFPQ44nw4r2ef24DrawSmoothStripeStrategy18AheadContextStripeiiRCQ34nw4r4math4VEC3RCQ34nw4r4math4VEC3(VT* self, void** arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    char blk[5][0x1C];
    char c1C[0xC];
    char c10[0xC];
    s32 r25;
    s32 r24;
    Q* r23;
    Obj* r22;
    s32 r21;
    Fn1 r20;
    Fn2 r19;
    s32 prev;
    void* cur;
    char* p24;
    s32 r18;
    s32 r17;
    s32 i16;
    s32 i15;
    f32 f30;
    f32 f29;

    r22 = (Obj*)*arg1;
    r23 = (Q*)GetEmitterDrawSetting__Q34nw4r2ef15EmitterResourceFv(r22->p24);
    r21 = self->v4(r22);
    r24 = GetDrawOrder__Q34nw4r2ef24DrawSmoothStripeStrategyCFRCQ34nw4r2ef18EmitterDrawSetting(self, r23);
    r20 = self->v2(r24);
    r19 = self->v3(r24);
    r25 = GetNumDrawParticle__Q34nw4r2ef16DrawStrategyImplFPQ34nw4r2ef15ParticleManager(self, r22);

    f32 f1 = (f32)r23->b9D;
    f30 = lbl_805A30A0 * f1;
    GXBegin(0x98, 0, (u16)((r25 * arg2 + 1) * 2));

    switch (GetStripeTexmapType__Q34nw4r2ef24DrawSmoothStripeStrategyCFRCQ34nw4r2ef18EmitterDrawSetting(self, r23)) {
    case 0:
    default:
        f29 = lbl_805A3090 / (f32)r25;
        break;
    case 0x40:
        f29 = lbl_805A3090;
        break;
    }

    r18 = -1;
    r17 = 2;
    if (r24 != 0) {
        r17 = -2;
        r18 = (r25 * 2) + 1;
    }

    __ct__Q44nw4r2ef24DrawSmoothStripeStrategy6VertexFv(blk[0]);
    __ct__Q44nw4r2ef24DrawSmoothStripeStrategy6VertexFv(blk[1]);
    __ct__Q44nw4r2ef24DrawSmoothStripeStrategy6VertexFv(blk[2]);
    __ct__Q44nw4r2ef24DrawSmoothStripeStrategy6VertexFv(blk[3]);
    __ct__Q44nw4r2ef24DrawSmoothStripeStrategy6VertexFv(blk[4]);
    __ct__Q34nw4r4math4VEC3Fv(c1C);
    __ct__Q34nw4r4math4VEC3Fv(c10);

    void* obj = r20(r22);
    CalcVertex__Q34nw4r2ef24DrawSmoothStripeStrategyFPQ44nw4r2ef24DrawSmoothStripeStrategy6VertexPQ44nw4r2ef24DrawSmoothStripeStrategy18AheadContextStripeiPQ34nw4r2ef8ParticlePFPQ34nw4r4math4VEC3PQ44nw4r2ef24DrawSmoothStripeStrategy18AheadContextStripePQ34nw4r2ef8Particle_vRCQ34nw4r4math4VEC3RCQ34nw4r4math4VEC3RCQ34nw4r4math4VEC3ff(self, blk[3], arg1, arg3, obj, r21, (char*)obj + 0xAC, arg4, f30, f29 * (f32)r18, arg5);

    obj = r19(r22, obj);
    r18 += r17;
    CalcVertex__Q34nw4r2ef24DrawSmoothStripeStrategyFPQ44nw4r2ef24DrawSmoothStripeStrategy6VertexPQ44nw4r2ef24DrawSmoothStripeStrategy18AheadContextStripeiPQ34nw4r2ef8ParticlePFPQ34nw4r4math4VEC3PQ44nw4r2ef24DrawSmoothStripeStrategy18AheadContextStripePQ34nw4r2ef8Particle_vRCQ34nw4r4math4VEC3RCQ34nw4r4math4VEC3RCQ34nw4r4math4VEC3ff(self, blk[4], arg1, arg3, obj, r21, (char*)obj + 0xAC, arg4, f30, f29 * (f32)r18, arg5);

    cur = r19(r22, obj);
    r18 += r17;
    i16 = 3;
    i15 = 4;

    while (cur != 0) {
        prev = i16;
        i16 = i15;
        i15 = (i15 + 1) % 3;
        p24 = (char*)blk + i15 * 0x1C;

        CalcVertex__Q34nw4r2ef24DrawSmoothStripeStrategyFPQ44nw4r2ef24DrawSmoothStripeStrategy6VertexPQ44nw4r2ef24DrawSmoothStripeStrategy18AheadContextStripeiPQ34nw4r2ef8ParticlePFPQ34nw4r4math4VEC3PQ44nw4r2ef24DrawSmoothStripeStrategy18AheadContextStripePQ34nw4r2ef8Particle_vRCQ34nw4r4math4VEC3RCQ34nw4r4math4VEC3RCQ34nw4r4math4VEC3ff(self, p24, arg1, arg3, cur, r21, (char*)cur + 0xAC, arg4, f30, f29 * (f32)r18, arg5);
        fn_8017EB30(self, arg2, (char*)blk + prev * 0x1C, (char*)blk + i16 * 0x1C, p24, arg3);

        cur = r19(r22, cur);
        r18 += r17;
    }

    char* p15 = (char*)blk + i15 * 0x1C;
    *(f32*)(blk[3] + 0x18) = f29 * (f32)r18;
    fn_8017EB30(self, arg2, (char*)blk + i16 * 0x1C, p15, blk[3], arg3);

    r18 += r17;
    *(f32*)(blk[4] + 0x18) = f29 * (f32)r18;
    fn_8017EB30(self, arg2, p15, blk[3], blk[4], arg3);
    fn_8017EDDC(self, p15, blk[3], blk[4], arg3);
    fn_8017F894();
}
}

// TODO: clean up, give variables real names
namespace jj_801804C4 {
extern "C" void* GetYoungestDrawParticle__Q34nw4r2ef16DrawStrategyImplFPQ34nw4r2ef15ParticleManager(char* self) {
    unsigned short off;
    char* r5 = *(char**)(self + 0x3C);
    while (r5 != 0 && *(int*)(r5 + 0xC) != 1) {
        off = *(unsigned short*)(self + 0x42);
        r5 = *(char**)(r5 + off);
        while (r5 != 0 && *(int*)(r5 + 0xC) != 1) {
            r5 = *(char**)(r5 + off);
        }
    }
    return r5;
}
}

// TODO: clean up, give variables real names
namespace jj_8018050C {

extern "C" void __mi__Q34nw4r4math4VEC3CFRCQ34nw4r4math4VEC3(nw4r::math::VEC3* pOut, const nw4r::math::VEC3* pA, const nw4r::math::VEC3* pB) {
    nw4r::math::VEC3Sub(pOut, pA, pB);
}
}

// TODO: clean up, give variables real names
namespace jj_80180530 {

extern "C" void __apl__Q34nw4r4math4VEC3FRCQ34nw4r4math4VEC3(nw4r::math::VEC3* pOut, const nw4r::math::VEC3* pB) {
    nw4r::math::VEC3Add(pOut, pOut, pB);
}
}

// TODO: clean up, give variables real names
namespace jj_80180554 {
typedef unsigned int u32;

extern "C" void __ct__Q44nw4r2ef24DrawSmoothStripeStrategy6VertexFRCQ34nw4r4math4VEC3RCQ34nw4r4math4VEC3f(u32* dst, const u32* a, const u32* b, float f) {
    dst[0] = a[0];
    dst[1] = a[1];
    dst[2] = a[2];
    dst[3] = b[0];
    dst[4] = b[1];
    dst[5] = b[2];
    *reinterpret_cast<float*>(dst + 6) = f;
}
}

