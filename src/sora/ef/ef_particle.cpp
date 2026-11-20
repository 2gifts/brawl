#include <ef/ef_manager.h>
#include <ef/ef_particle.h>
#include <gm/gm_global.h>
#include <math.h>
#include <sr/sr_common.h>
#include <string.h>
#include <types.h>

using namespace nw4r::ef;

extern "C" Emitter* fn_801643C8(const Effect* effect);

void* lbl_805A0118[2];
void* lbl_805A0120[2];
efParticle::EmitterBackup* lbl_805A0128;
efParticle::EmitterBackup* lbl_805A012C;
// HYPOTHESIS: untyped placeholders for the rest of this unit's .sbss, sizes and order from the original layout.
unsigned char lbl_805A0130[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0134[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0138[0x8] __attribute__((aligned(4))) = { 0 };

void efParticle::setSlowParam(void* p1, void* p2, void* p3, void* p4) {
    lbl_805A0118[0] = p1;
    lbl_805A0118[1] = p2;
    lbl_805A0120[0] = p3;
    lbl_805A0120[1] = p4;
}

efParticle* efParticle::create(const char* name, u8 group) {
    return new (Heaps::Effect) efParticle(name, group);
}

efParticle::efParticle(const char* name, u8 group) : efEffect(name, group) {
    unk54_b7 = false;
    unk54_b6 = false;
    unk54_b5 = false;
    unk54_b4 = false;
    unk54_b3 = false;
    unk55 = 0;
    unk58 = 0;
    unk5C = 0.0f;
    unk54_b6 = true;
    if (g_GameGlobal->isJustGameFrame()) {
        float frame = g_GameGlobal->getGameFrame();
        m_frame = frame - (float)floor(frame);
    }
    if (strcmp(name, "PtcFinIkeCutFire") == 0) {
        unk54_b3 = true;
        efManager::getInstance()->unk138++;
    }
}

efParticle::~efParticle() {
    if (m_handle.IsValid()) {
        Effect* effect;
        EffectSystem* system = EffectSystem::GetInstance();
        if (unk54_b3 == true) {
            efManager* manager = efManager::getInstance();
            if (manager->unk138 > 0) {
                manager->unk138--;
            }
        }
        effect = static_cast<Effect*>(m_handle.GetPtr());
        effect->RetireParticleAll();
        effect->RetireEmitterAll();
        system->RetireEffect(effect);
        HandleBase empty;
        m_handle = empty;
    }
}

void efParticle::endCallback() {
    if (m_handle.IsValid()) {
        Effect* effect = static_cast<Effect*>(m_handle.GetPtr());
        if (effect->GetNumEmitter()) {
            effect->RetireEmitterAll();
        }
    }
}

bool efParticle::isAlive() {
    bool alive = false;
    if (m_handle.IsValid()) {
        Effect* effect = static_cast<Effect*>(m_handle.GetPtr());
        u16 num = effect->GetNumEmitter();
        for (u16 i = 0; i < num; i++) {
            if (effect->GetEmitter(i)->mLifeStatus == 1) {
                alive = true;
                break;
            }
        }
        if (!alive) {
            efParticleCounter counter;
            if (counter.count(effect)) {
                alive = true;
            }
        }
    }
    return alive;
}

void efParticle::stopEmitter() {
    if (m_handle.IsValid()) {
        if (getState() == 4) {
            m_state = 2;
        }
        static_cast<Effect*>(m_handle.GetPtr())->RetireEmitterAll();
    }
}

void efParticle::updateRootMatrix() {
    Effect* effect = static_cast<Effect*>(m_handle.GetPtr());
    if (fn_801643C8(effect) == NULL) {
        m_state = 3;
        if (unk54_b7) {
            end();
        }
    } else {
        Matrix mtx(true);
        getWorldMatrix(&mtx);
        effect->SetRootMtx(mtx);
    }
}

void efParticle::foreachEmitterBackup(void* emitter, u32 param) {
    Emitter* em = static_cast<Emitter*>(emitter);
    EmitterBackup* backup = &lbl_805A012C[static_cast<u16>(em->mObjectID)];
    backup->unk24 = em->unk20;
    backup->m_trans = em->mTranslate;
    backup->m_rot = em->mRotate;
    backup->m_scale = em->mScale;
}

void efParticle::foreachEmitterRestore(void* emitter, u32 param) {
    Emitter* em = static_cast<Emitter*>(emitter);
    EmitterBackup* backup = &lbl_805A012C[static_cast<u16>(em->mObjectID)];
    em->unk20 = backup->unk24;
    em->mTranslate = backup->m_trans;
    em->mRotate = backup->m_rot;
    em->mScale = backup->m_scale;
    em->SetMtxDirty();
}

void efParticleCounter::foreachPM(void* particleManager, u32 counter) {
    reinterpret_cast<efParticleCounter*>(counter)->m_count +=
        static_cast<ParticleManager*>(particleManager)->mNumParticle;
}

u32 efParticleCounter::count(Effect* effect) {
    m_count = 0;
    effect->ForeachParticleManager(foreachPM, reinterpret_cast<u32>(this), true);
    return m_count;
}

// TODO: clean up, give variables real names
namespace jj_8005CE70 {
struct Object_vptr_1c_base { char pad[0x1c]; };
class Object_vptr_1c : public Object_vptr_1c_base {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5(void* arg);
};

struct Obj {
    char pad0[0x0c];
    int w0c;
    char pad1[0x3c];
    char sub[8];
    u8 f54;
    u8 f55;
    char pad2[6];
    float f5c;
};

extern "C" {
void* GetPtr__Q34nw4r2ef10HandleBaseCFv(void* p);
int IsValid__Q34nw4r2ef10HandleBaseCFv(void* p);
void create__9efManagerFv();
void ForeachEmitter__Q34nw4r2ef6EffectFPFPvUl_vUlb(void* self, void (*f)(), int a, int b);
void ForeachParticleManager__Q34nw4r2ef6EffectFPFPvUl_vUlb(void* self, void (*f)(), int a, int b);
void fn_8005D118();
void foreachEmitterRestore__10efParticleFPvUl();
void fn_8005D34C();

extern void* lbl_805A0110;
extern int lbl_805A0118[2];
extern int lbl_805A0120[2];
extern int lbl_805A0128;
extern int lbl_805A012C;
extern int lbl_805A0130;
extern int lbl_805A0134;
extern float lbl_805A0138;

// TODO: clean up, give variables real names
void fn_8005CE70(Obj* self) {
    int state = self->w0c >> 24;
    if ((state == 2 || state == 4) && IsValid__Q34nw4r2ef10HandleBaseCFv(self->sub) != 0) {
        char* r31;
        if (lbl_805A0110 == 0) {
            create__9efManagerFv();
        }
        r31 = (char*)lbl_805A0110 + 0x84;
        if (!(self->f55 & 1)) {
            lbl_805A0128 = lbl_805A0118[0];
            lbl_805A012C = lbl_805A0118[1];
            lbl_805A0130 = lbl_805A0120[0];
            lbl_805A0134 = lbl_805A0120[1];
        } else {
            lbl_805A0128 = lbl_805A0118[1];
            lbl_805A012C = lbl_805A0118[0];
            lbl_805A0130 = lbl_805A0120[1];
            lbl_805A0134 = lbl_805A0120[0];
        }
        int v = self->f54;
        v &= 0xFFFFFFEF;
        self->f54 = (u8)v;
        if ((v >> 5) & 1) {
            float f31 = self->f5c;
            void* r30 = GetPtr__Q34nw4r2ef10HandleBaseCFv(self->sub);
            lbl_805A0138 = f31;
            if (((u8)self->f54 >> 4) & 1) {
                if (!(self->f55 & 1)) {
                    lbl_805A0128 = lbl_805A0118[0];
                    lbl_805A012C = lbl_805A0118[1];
                    lbl_805A0130 = lbl_805A0120[0];
                    lbl_805A0134 = lbl_805A0120[1];
                } else {
                    lbl_805A0128 = lbl_805A0118[1];
                    lbl_805A012C = lbl_805A0118[0];
                    lbl_805A0130 = lbl_805A0120[1];
                    lbl_805A0134 = lbl_805A0120[0];
                }
                self->f54 = self->f54 & 0xFFFFFFEF;
            }
            ForeachEmitter__Q34nw4r2ef6EffectFPFPvUl_vUlb(r30, fn_8005D118, 0, 0);
            ForeachParticleManager__Q34nw4r2ef6EffectFPFPvUl_vUlb(r30, fn_8005D34C, 0, 1);
            ((Object_vptr_1c*)GetPtr__Q34nw4r2ef10HandleBaseCFv(self->sub))->slot5(r31);
        } else {
            ((Object_vptr_1c*)GetPtr__Q34nw4r2ef10HandleBaseCFv(self->sub))->slot5(r31);
        }
        void* r30b = GetPtr__Q34nw4r2ef10HandleBaseCFv(self->sub);
        if (((u8)self->f54 >> 4) & 1) {
            if (!(self->f55 & 1)) {
                lbl_805A0128 = lbl_805A0118[0];
                lbl_805A012C = lbl_805A0118[1];
                lbl_805A0130 = lbl_805A0120[0];
                lbl_805A0134 = lbl_805A0120[1];
            } else {
                lbl_805A0128 = lbl_805A0118[1];
                lbl_805A012C = lbl_805A0118[0];
                lbl_805A0130 = lbl_805A0120[1];
                lbl_805A0134 = lbl_805A0120[0];
            }
            self->f54 = self->f54 & 0xFFFFFFEF;
        }
        ForeachEmitter__Q34nw4r2ef6EffectFPFPvUl_vUlb(r30b, foreachEmitterRestore__10efParticleFPvUl, 1, 1);
        ForeachParticleManager__Q34nw4r2ef6EffectFPFPvUl_vUlb(r30b, fn_8005D34C, 1, 1);
    }
}
}
}

