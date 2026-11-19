#include <types.h>
// nw4r ef_drawstripestrategy.cpp (main.dol 0x8017B400-0x8017E24C). Partially reconstructed.
#include <nw4r/ef.h>

// HYPOTHESIS: constant tables in .bss (lbl_804A4408 is 0x18 bytes, lbl_804A4420
// is 0xC bytes). Their values are set at run time, so they are only declared.
extern nw4r::math::VEC3 lbl_804A4408[2];
extern nw4r::math::VEC3 lbl_804A4420;

namespace nw4r {
namespace ef {

DrawStripeStrategy::DrawStripeStrategy() {}

u8 DrawStripeStrategy::GetStripeTexmapType(
    const EmitterDrawSetting& rSetting) const {
    return rSetting.typeOption2 & 0xC0;
}

math::VEC3 DrawStripeStrategy::GetInitialPrevAxis(
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

void DrawStripeStrategy::CalcAhead_Particle_Stripe(
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
DrawStripeStrategy::GetCalcAheadFunc(ParticleManager* pManager) {

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

void DrawStripeStrategy::CalcAhead_ParticleBoth_Stripe(
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

void DrawStripeStrategy::CalcAhead_ParticleBoth_Ring(
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

void DrawStripeStrategy::CalcAhead_ParticleBoth_Origin(
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

// BYTES: the original calls the header helpers (GetEmitterDrawSetting,
// GetNumDrawParticle, GXEnd) out of line inside DrawStripe.
#pragma dont_inline on
void DrawStripeStrategy::DrawStripe(AheadContextStripe* pContext, int param,
                                    const math::VEC3& rAxisA,
                                    const math::VEC3& rAxisB) {

    ParticleManager* pManager = pContext->mCommon.mParticleManager;

    const EmitterDrawSetting& rSetting =
        *pManager->mResource->GetEmitterDrawSetting();

    CalcAheadFunc pCalcAhead = GetCalcAheadFunc(pManager);

    u16 drawOrder = rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER;

    GetFirstDrawParticleFunc pGetFirst = GetGetFirstDrawParticleFunc(drawOrder);
    GetNextDrawParticleFunc pGetNext = GetGetNextDrawParticleFunc(drawOrder);

    int count = GetNumDrawParticle(pManager);

    f32 pivot = rSetting.pivotX * 0.01f;

    u8 assist = rSetting.typeOption2 & 7;
    bool isCross = (assist == EmitterDrawSetting::ASSIST_ST_CROSS);
    bool isBillboard = (assist == EmitterDrawSetting::ASSIST_ST_BILLBOARD);

    if (isCross || isBillboard) {
        count++;
    }

    GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, count * 2);

    f32 step = 1.0f;
    int texmapType = GetStripeTexmapType(rSetting);
    if (texmapType != 0x40) {
        step = 1.0f / (f32)(count - 1);
    }

    int index;
    int inc;
    if ((rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER) == 0) {
        index = 0;
        inc = 1;
    } else {
        index = count - 1;
        inc = -1;
    }

    if (isBillboard && drawOrder == 0) {
        DrawParticle(pContext, param, NULL, pCalcAhead,
                     pContext->mCommon.mEmitterCenter, rAxisA, rAxisB, pivot,
                     step * index);
        index += inc;
    }

    for (Particle* pIt = pGetFirst(pManager); pIt != NULL;
         pIt = pGetNext(pManager, pIt)) {

        DrawParticle(pContext, param, pIt, pCalcAhead,
                     pIt->mParameter.mPosition, rAxisA, rAxisB, pivot,
                     step * index);
        index += inc;
    }

    if (isCross) {
        Particle* pFirst = pGetFirst(pManager);
        DrawParticle(pContext, param, pFirst, pCalcAhead,
                     pFirst->mParameter.mPosition, rAxisA, rAxisB, pivot,
                     step * index);
    }

    if (isBillboard && drawOrder != 0) {
        DrawParticle(pContext, param, NULL, pCalcAhead,
                     pContext->mCommon.mEmitterCenter, rAxisA, rAxisB, pivot,
                     step * index);
    }

    GXEnd();
}

#pragma dont_inline reset

} // namespace ef
} // namespace nw4r

// TODO: clean up, give variables real names
extern "C" void fn_8017BC34(void) {}

#include <nw4r/math/math_types.h>
#include <revolution/GX/GXVert.h>
#include <revolution/GX/GXGeometry.h>
// TODO: clean up, give variables real names
namespace jj_8017B7A0 {

using namespace nw4r::math;

extern "C" {
void PSVECCrossProduct(const VEC3*, const VEC3*, VEC3*);
void PSMTXMultVec(const MTX34*, s32, VEC3*);
void PSMTXConcat(const MTX34*, const MTX34*, MTX34*, f32, f32, f32, f32, f32);
f32 FrSqrt__Q24nw4r4mathFf(f32);
f32 SinFIdx__Q24nw4r4mathFf(f32);
f32 CosFIdx__Q24nw4r4mathFf(f32);
extern const f32 lbl_805A303C;
extern const f32 lbl_805A3048;
extern const f32 lbl_805A304C;
extern const f32 lbl_805A3050;
extern const f32 lbl_805A3054;
}

static inline VEC3& divBy(VEC3& v, f32 x) {
    return v *= (lbl_805A303C / x);
}

extern "C" void DrawParticle__Q34nw4r2ef18DrawStripeStrategyFPQ44nw4r2ef18DrawStripeStrategy18AheadContextStripeiPQ34nw4r2ef8ParticlePFPQ34nw4r4math4VEC3PQ44nw4r2ef18DrawStripeStrategy18AheadContextStripePQ34nw4r2ef8Particle_vRCQ34nw4r4math4VEC3RCQ34nw4r4math4VEC3RCQ34nw4r4math4VEC3ff(void* unused, void* a1, u32 flags, void* obj,
                            void (*fn)(VEC3*, void*, void*), const VEC3* vecA,
                            s32 idx1, s32 idx2, f32 farg0, f32 farg1) {
    MTX34 m3, m1, m2;
    VEC3 sp44, pA, pB, sp20, sp14, pos;
    f32 f29;

    if (obj == NULL) {
        void* p = *(void**)a1;
        void* node = *(void**)((u8*)p + 0x3C);
        while (node != NULL && *(s32*)((u8*)node + 0xC) != 1) {
            u16 off = *(u16*)((u8*)p + 0x42);
            node = *(void**)((u8*)node + off);
            while (node != NULL && *(s32*)((u8*)node + 0xC) != 1) {
                node = *(void**)((u8*)node + off);
            }
        }
        obj = node;
    }

    fn(&sp44, a1, obj);
    VEC3* pa = &sp44;
    if (!(flags & 2)) {
        PSVECCrossProduct(pa, (const VEC3*)((u8*)obj + 0xCC), &sp14);
    } else {
        PSVECCrossProduct(pa, (const VEC3*)((u8*)a1 + 0xC8), &sp14);
    }

    s32 ok;
    f29 = VEC3Dot(&sp14, &sp14);
    if (f29 < lbl_805A3048) {
        ok = 0;
    } else {
        f32 t;
        if (f29 <= lbl_805A304C) {
            t = lbl_805A304C;
        } else {
            t = f29 * FrSqrt__Q24nw4r4mathFf(f29);
        }
        divBy(sp14, t);
        ok = 1;
    }
    if (ok == 0) {
        sp14 = *(VEC3*)((u8*)a1 + 0xBC);
    }

    PSVECCrossProduct(&sp14, pa, &sp20);
    f29 = VEC3Dot(&sp20, &sp20);
    if (!(f29 < lbl_805A3048)) {
        f32 t;
        if (f29 <= lbl_805A304C) {
            t = lbl_805A304C;
        } else {
            t = f29 * FrSqrt__Q24nw4r4mathFf(f29);
        }
        divBy(sp20, t);
    }

    *(VEC3*)((u8*)obj + 0xCC) = sp20;

    f32 a23 = vecA->z, a22 = sp20.z, a21 = sp44.z, a20 = sp14.z;
    f32 a13 = vecA->y, a12 = sp20.y, a11 = sp44.y, a10 = sp14.y;
    f32 a03 = vecA->x, a02 = sp20.x, a01 = sp44.x, a00 = sp14.x;
    m1.m[0][1] = a01; m1.m[0][0] = a00; m1.m[0][2] = a02; m1.m[0][3] = a03;
    m1.m[1][0] = a10; m1.m[1][1] = a11; m1.m[1][2] = a12; m1.m[1][3] = a13;
    m1.m[2][0] = a20; m1.m[2][1] = a21; m1.m[2][2] = a22; m1.m[2][3] = a23;

    pos.x = *(f32*)((u8*)obj + 0x40);
    pos.y = *(f32*)((u8*)obj + 0x44);
    pos.z = *(f32*)((u8*)obj + 0x48);
    {
        u8 b;
        b = *(u8*)((u8*)obj + 0x9A);
        if (b != 0) {
            pos.x = pos.x + lbl_805A3050 * (f32)b;
        }
        b = *(u8*)((u8*)obj + 0x9B);
        if (b != 0) {
            pos.y = pos.y + lbl_805A3050 * (f32)b;
        }
        b = *(u8*)((u8*)obj + 0x9C);
        if (b != 0) {
            pos.z = pos.z + lbl_805A3050 * (f32)b;
        }
    }

    {
        u8* S = *(u8**)((u8*)obj + 0xC8);
        f32 y = pos.y + *(f32*)(S + 0x64);
        f32 x = pos.x + *(f32*)(S + 0x60);
        f32 z = pos.z + *(f32*)(S + 0x68);
        pos.y = y;
        pos.x = x;
        pos.z = z;
        f32 f6 = *(f32*)((u8*)obj + 0x30) * *(f32*)((u8*)obj + 0x38);
        f32 f28 = f6 * *(f32*)(S + 0x58);
        f29 = f28 * CosFIdx__Q24nw4r4mathFf(lbl_805A3054 * pos.y);
        f32 s = f28 * SinFIdx__Q24nw4r4mathFf(lbl_805A3054 * (f32)(double)pos.y);
        f32 omc = farg0 - f29 * farg0;

        m2.m[0][0] = f29;
        m2.m[0][1] = lbl_805A304C;
        m2.m[0][2] = -s;
        m2.m[0][3] = omc;
        m2.m[1][0] = lbl_805A304C;
        m2.m[1][1] = lbl_805A303C;
        m2.m[1][2] = lbl_805A304C;
        m2.m[1][3] = lbl_805A304C;
        m2.m[2][0] = s;
        m2.m[2][1] = lbl_805A304C;
        m2.m[2][2] = f29;
        m2.m[2][3] = -s * farg0;

        PSMTXConcat(&m1, &m2, &m3, lbl_805A303C, omc, -s, lbl_805A304C, s);
    }

    PSMTXMultVec(&m3, idx1, &pA);
    PSMTXMultVec(&m3, idx2, &pB);

    GXPosition3f32(pA.x, pA.y, pA.z);
    if (flags & 1) {
        GXTexCoord2f32(lbl_805A303C, farg1);
    }
    GXPosition3f32(pB.x, pB.y, pB.z);
    if (flags & 1) {
        GXTexCoord2f32(lbl_805A304C, farg1);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_8017C748 {
struct Particle;
struct Emitter {
    char pad0[0x24];
    char* resource;
    char pad28[0x28];
    u16 count;
};
struct DrawContext {
    Emitter* emitter;
    char pad4[0xd0];
    int value;
};
struct Vertex {
    nw4r::math::VEC3 a, b, c;
    float t;
};
typedef Particle* (*FirstFn)(Emitter*);
typedef Particle* (*NextFn)(Emitter*, Particle*);
typedef void (*PositionFn)();
class Class_804654B0 {
public:
    virtual void __dt__Q34nw4r2ef21DrawBillboardStrategyFv();
    virtual void Draw__Q34nw4r2ef21DrawBillboardStrategyFRCQ34nw4r2ef8DrawInfoPQ34nw4r2ef15ParticleManager();
    virtual FirstFn GetGetFirstDrawParticleFunc__Q34nw4r2ef16DrawStrategyImplFi(int);
    virtual NextFn GetGetNextDrawParticleFunc__Q34nw4r2ef16DrawStrategyImplFi(int);
    virtual PositionFn GetCalcAheadFunc__Q34nw4r2ef21DrawBillboardStrategyFPQ34nw4r2ef15ParticleManager(Emitter*);
};
extern "C" {
extern const float lbl_805A3038;
extern const float lbl_805A303C;
extern const double lbl_805A3040;
void CalcVertexTube__Q34nw4r2ef18DrawStripeStrategyFPQ44nw4r2ef18DrawStripeStrategy10VertexTubePQ44nw4r2ef18DrawStripeStrategy18AheadContextStripeiPQ34nw4r2ef8ParticlePFPQ34nw4r4math4VEC3PQ44nw4r2ef18DrawStripeStrategy18AheadContextStripePQ34nw4r2ef8Particle_vfff(Class_804654B0*, Vertex*, DrawContext*, int, Particle*, PositionFn, float, float, float);
void DrawTubeSegment__Q34nw4r2ef18DrawStripeStrategyFRCQ44nw4r2ef18DrawStripeStrategy10VertexTubeRCQ44nw4r2ef18DrawStripeStrategy10VertexTubeiiPCQ44nw4r2ef18DrawStripeStrategy13Trigonometric(Class_804654B0*, Vertex*, Vertex*, int, u8, int);
}
extern "C" void DrawStripeTube_ConnectRing__Q34nw4r2ef18DrawStripeStrategyFPQ44nw4r2ef18DrawStripeStrategy18AheadContextStripei(Class_804654B0* self, DrawContext* context, int flags) {
    char* resource;
    Particle* link;
    Emitter* emitter = context->emitter;
    resource = emitter->resource;
    PositionFn position = self->GetCalcAheadFunc__Q34nw4r2ef21DrawBillboardStrategyFPQ34nw4r2ef15ParticleManager(emitter);
    int reverse = *(u16*)(resource + 0x9c) & 0x800;
    FirstFn first = self->GetGetFirstDrawParticleFunc__Q34nw4r2ef16DrawStrategyImplFi(reverse);
    NextFn next = self->GetGetNextDrawParticleFunc__Q34nw4r2ef16DrawStrategyImplFi(reverse);
    float x = lbl_805A3038 * (s8)resource[0x139];
    float y = lbl_805A3038 * (s8)resource[0x13a];
    int count = emitter->count;
    float step;
    switch (resource[0x142] & 0xc0) {
    case 0:
    default:
        step = lbl_805A303C / (float)count;
        break;
    case 0x40:
        step = lbl_805A303C;
        break;
    }
    int index = 0;
    int increment = 1;
    if (reverse != 0) {
        index = count;
        increment = -1;
    }
    Particle* particle = first(emitter);
    Vertex current;
    Vertex previous;
    Vertex initial;
    CalcVertexTube__Q34nw4r2ef18DrawStripeStrategyFPQ44nw4r2ef18DrawStripeStrategy10VertexTubePQ44nw4r2ef18DrawStripeStrategy18AheadContextStripeiPQ34nw4r2ef8ParticlePFPQ34nw4r4math4VEC3PQ44nw4r2ef18DrawStripeStrategy18AheadContextStripePQ34nw4r2ef8Particle_vfff(self, &initial, context, flags, particle, position, x, y, step * (float)index);
    index = index + (increment);
    current = initial;
    link = next(emitter, particle);
    while (link) {
        previous = current;
        CalcVertexTube__Q34nw4r2ef18DrawStripeStrategyFPQ44nw4r2ef18DrawStripeStrategy10VertexTubePQ44nw4r2ef18DrawStripeStrategy18AheadContextStripeiPQ34nw4r2ef8ParticlePFPQ34nw4r4math4VEC3PQ44nw4r2ef18DrawStripeStrategy18AheadContextStripePQ34nw4r2ef8Particle_vfff(self, &current, context, flags, link, position, x, y, step * (float)index);
        DrawTubeSegment__Q34nw4r2ef18DrawStripeStrategyFRCQ44nw4r2ef18DrawStripeStrategy10VertexTubeRCQ44nw4r2ef18DrawStripeStrategy10VertexTubeiiPCQ44nw4r2ef18DrawStripeStrategy13Trigonometric(self, &previous, &current, flags, resource[0x140], context->value);
        link = next(emitter, link);
        index += increment;
    }
    initial.t = step * (float)index;
    DrawTubeSegment__Q34nw4r2ef18DrawStripeStrategyFRCQ44nw4r2ef18DrawStripeStrategy10VertexTubeRCQ44nw4r2ef18DrawStripeStrategy10VertexTubeiiPCQ44nw4r2ef18DrawStripeStrategy13Trigonometric(self, &current, &initial, flags, resource[0x140], context->value);
}
}

// TODO: clean up, give variables real names
namespace jj_8017CE40 {

extern "C" {
void SinCosFIdx__Q24nw4r4mathFPfPff(float*, float*, float);
void DrawStripeTube_ConnectNone__Q34nw4r2ef18DrawStripeStrategyFPQ44nw4r2ef18DrawStripeStrategy18AheadContextStripei(void*, void*, int);
void DrawStripeTube_ConnectRing__Q34nw4r2ef18DrawStripeStrategyFPQ44nw4r2ef18DrawStripeStrategy18AheadContextStripei(void*, void*, int);
void DrawStripeTube_ConnectEmitter__Q34nw4r2ef18DrawStripeStrategyFPQ44nw4r2ef18DrawStripeStrategy18AheadContextStripei(void*, void*, int);

extern const float lbl_805A303C;
extern const float lbl_805A304C;
extern const float lbl_805A3060;
extern const float lbl_805A3064;
}

struct Pair {
    float x;
    float y;

    void set(float a, float b) {
        y = b;
        x = a;
    }
};

inline void setCirclePoint(Pair* p, int i, float step) {
    float fi = (float)i;
    SinCosFIdx__Q24nw4r4mathFPfPff(&p->x, &p->y, lbl_805A3064 * (step * fi));
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

extern "C" void DrawStripeTube__Q34nw4r2ef18DrawStripeStrategyFPQ44nw4r2ef18DrawStripeStrategy18AheadContextStripei(void* a0, Node* a1, int a2) {
    Inner* in = a1->obj->inner;
    int n = in->count;

    if (n >= 3) {
        Pair* p;
        Pair* arr = (Pair*)__alloca((n + 1) * 8);

        if (arr) {
            float fn_ = (float)n;
            float f29 = lbl_805A3060 / fn_;
            arr[0].y = lbl_805A303C;
            p = arr + 1;
            int i;

            arr[0].x = lbl_805A304C;

            for (i = 1; i < n; p++, i++) {
                setCirclePoint(p, i, f29);
            }

            arr[n].set(lbl_805A304C, lbl_805A303C);
            a1->arr = arr;

            switch (in->mode & 7) {
            case 0:
            default:
                GXSetCullMode(GX_CULL_FRONT);
                DrawStripeTube_ConnectNone__Q34nw4r2ef18DrawStripeStrategyFPQ44nw4r2ef18DrawStripeStrategy18AheadContextStripei(a0, a1, a2);
                GXSetCullMode(GX_CULL_BACK);
                DrawStripeTube_ConnectNone__Q34nw4r2ef18DrawStripeStrategyFPQ44nw4r2ef18DrawStripeStrategy18AheadContextStripei(a0, a1, a2);
                break;

            case 1:
                GXSetCullMode(GX_CULL_FRONT);
                DrawStripeTube_ConnectRing__Q34nw4r2ef18DrawStripeStrategyFPQ44nw4r2ef18DrawStripeStrategy18AheadContextStripei(a0, a1, a2);
                GXSetCullMode(GX_CULL_BACK);
                DrawStripeTube_ConnectRing__Q34nw4r2ef18DrawStripeStrategyFPQ44nw4r2ef18DrawStripeStrategy18AheadContextStripei(a0, a1, a2);
                break;

            case 2:
                GXSetCullMode(GX_CULL_FRONT);
                DrawStripeTube_ConnectEmitter__Q34nw4r2ef18DrawStripeStrategyFPQ44nw4r2ef18DrawStripeStrategy18AheadContextStripei(a0, a1, a2);
                GXSetCullMode(GX_CULL_BACK);
                DrawStripeTube_ConnectEmitter__Q34nw4r2ef18DrawStripeStrategyFPQ44nw4r2ef18DrawStripeStrategy18AheadContextStripei(a0, a1, a2);
                break;
            }
        }
    }
}
}

