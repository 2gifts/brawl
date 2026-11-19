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

