#define WN_ANIMCMD_RESET_MODE_BOX // BYTES: word-aligned reset-mode temporaries (frame size of the anim cmd sub builder)
#define WN_ANIMCMD_STACK_PTR // BYTES: the interpreter stacks stay in a saved register across getManageId (ft_builder_animcmd.h)
#define WN_ANIMCMD_PRESENTER_LOCAL_STORE // BYTES: the interpreters' presenters keep the looked-up list in a local, as in the Mario fireball
#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: activate copies the Vec3f position as an aggregate (word moves), see mt_vector.h
#define WN_EVENT_UNIT_DTOR_INLINE // BYTES: the inside event builder destructor expands the event unit destructors
#define WN_STATUS_MODULE_KIND_COUNT_ZERO // BYTES: the status module is built with a status kind count of 0 (ft_builder_status.h)
#define FT_MODULE_BUILDER
#define WN_MARIO_FIREBALL_UNIT // the sora_melee article unit mode shared with the Mario fireball (builders out of line)
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_STATUS_NO_TAIL
#define SO_OTHER_ANIM_CTOR // BYTES: the sora_melee motion builder constructs and destroys its other-animation slot
#define WN_TRANSITION_CTOR_INLINE
#define WN_ARTICLE_ANIMCMD_INT // BYTES: the builder vtable takes Weapon's int-index notifyEventAnimCmd thunk, as in the original
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#define WN_POKE_LIZARDON_BREATH_BUILD
#include <wn/wn_module_accesser_builder.h>
#include <so/reflect/so_reflect_module.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/poke/wn_poke_lizardon_breath.h>
#include <so/model/so_model_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <gf/gf_task_scheduler.h>
#include <memory.h>
// BYTES: the empty base destructors that sora_melee always expands (they have no out-of-line copy there).
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <mt/mt_prng.h>
#include <math.h>

wnPokeLizardonBreath::wnPokeLizardonBreath(s32 articleId, const wnArticleBuildInfo& info, const wnPokeLizardonBreathData* data) :
    wnWeaponBuilder<wnPokeLizardonBreathModuleAccesserBuildConfig>(
        "PokeLizardonBreath", articleId, info,
        wnModuleAccesserBuildData(13, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1,
                                  &g_soParamAccesserNull, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param) {
    Weapon::deactivate(false);
}

// The weak builders this unit owns in the link order (the other units with the same configurations reuse them).
typedef wnPokeLizardonBreathModuleAccesserBuildConfig PlBC;
template soInsideEventManageModuleBuilder<PlBC::InsideEventManageModuleBuildConfig, PlBC::InsideEventManageModuleTypes>::soInsideEventManageModuleBuilder();
#pragma push
#pragma pool_data off
template soMotionModuleBuilder<PlBC::MotionModuleBuildConfig>::soMotionModuleBuilder(soModuleAccesser*, void*);
#pragma pop
template soStatusModuleBuilder<PlBC::StatusModuleBuildConfig>::soStatusModuleBuilder(soModuleAccesser*, const soStatusData*, void*);

#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnPokeLizardonBreathModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>

void wnPokeLizardonBreath::updateNodeSRT() {
    Vec3f scaling;
    float scale = m_moduleAccesser->getPostureModule().getScale();
    scaling.m_z = scale;
    scaling.m_y = scale;
    scaling.m_x = scale;
    Vec3f position = m_moduleAccesser->getPostureModule().getPos();
    Vec2f speed = m_moduleAccesser->getKineticModule().getSumSpeed(1);
    Vec3f rotation;
    float lr = m_moduleAccesser->getPostureModule().getLr();
    rotation.m_x = 57.29578f * -atan2f(speed.m_y, speed.m_x * lr);
    rotation.m_y = lr == 1.0f ? 90.0f : -90.0f;
    rotation.m_z = 0.0f;
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scaling, &rotation, &position);
}

bool wnPokeLizardonBreath::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

bool wnPokeLizardonBreath::reflect() {
    int life = m_moduleAccesser->getWorkManageModule().getInt(0x10000005);
    if (life > 0) {
        float multiplier = m_moduleAccesser->getReflectModule().getLifeMul();
        life = (int)(life * multiplier);
        m_moduleAccesser->getWorkManageModule().setInt(life, 0x10000005);
    }
    return Weapon::reflect();
}

void wnPokeLizardonBreath::activate(int founderTaskId, int team, const Vec3f& position, int variation,
                                    float lr, float speed, float scale, float speedMul, float angle) {
    wnActivateDesc desc(founderTaskId, 0xFFFF, 0xFFFF, 0xFFFF, -1, -1, 0, 0, position,
                        lr, team, m_param->m_work5, 5, 0x80, 0, 0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getWorkManageModule().setInt(variation, 0x10000006);
    m_moduleAccesser->getWorkManageModule().setInt(m_param->m_work6, 0x10000005);
    m_moduleAccesser->getWorkManageModule().setFloat(scale, 0x11000001);
    m_moduleAccesser->getGroundModule().setOffsetY(m_param->m_groundOffsetY, 0);
    m_moduleAccesser->getPostureModule().setScale(scale);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    float randomSpeed = m_param->m_speedRandMin + (m_param->m_speedRandMax - m_param->m_speedRandMin) * randf();
    speedMul *= speed * randomSpeed;
    energy.m_speed = Vec2f(lr * (speedMul * (float)cos(angle)), speedMul * (float)sin(angle));
    updateNodeSRT();
}

#include <nw4r/math/math_arithmetic.h>
#include <mt/mt_common.h>
// TODO: clean up, give variables real names
namespace jj_36D2CC {
#pragma fp_contract off
struct Vec2 { float x; float y; };
extern "C" {
int notifyEventAnimCmd__6WeaponFP9acAnimCmdP16soModuleAccesseri(void*);
void* __dynamic_cast(void*, long, const void*, const void*, short);
double atan2(double, double);
double cos(double);
double sin(double);
float angle__5Vec2fCFPC5Vec2f(const Vec2* lhs, const Vec2* rhs);
extern const float lbl_27_rodata_7B40[];
extern char __RTTI__21wnKineticEnergyNormal[];
extern char __RTTI__15soKineticEnergy[];
}
inline Vec2 unpack(unsigned long long bits) { return *(Vec2*)&bits; }
struct gfTask_base { char pad[0x3c]; };
class gfTask : public gfTask_base {
public:
    virtual void slot0(); virtual void slot1(); virtual void slot2(); virtual void slot3();
    virtual void slot4(); virtual void slot5(); virtual void slot6(); virtual void slot7();
    virtual void slot8(); virtual void slot9(); virtual void slot10(); virtual void slot11();
    virtual void slot12(); virtual void slot13(); virtual void slot14(); virtual void slot15();
    virtual void slot16(); virtual void slot17(); virtual void slot18(); virtual void slot19();
    virtual void slot20(); virtual void slot21(); virtual void slot22(); virtual void slot23();
    virtual void slot24(); virtual void slot25(); virtual void slot26(); virtual void slot27();
    virtual void slot28(); virtual void slot29(); virtual void slot30(); virtual void slot31();
    virtual void slot32(); virtual void slot33(); virtual void slot34(); virtual void slot35();
    virtual void slot36(); virtual void slot37(); virtual void slot38(); virtual void slot39();
    virtual void slot40(); virtual void slot41(); virtual void slot42(); virtual void slot43();
    virtual void slot44(); virtual void slot45(); virtual void slot46();
    virtual int fn_27_24CC58(signed char);
};
class NullableModule {
public:
    virtual void isNull();
    virtual signed char slot1();
    virtual signed char slot2();
};
class KineticEnergy {
public:
    virtual void isNull();
    virtual void updateEnergy();
    virtual unsigned long long getSpeed();
    unsigned char p04;
    unsigned char p05;
    char pad06[2];
    Vec2 speed;
};
inline void setSpeed(KineticEnergy* e, float x, float y) { Vec2 out; out.y = y; out.x = x; e->speed = out; }
class Kinetic {
public:
    virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3();
    virtual void s4(); virtual void s5();
    virtual KineticEnergy* getEnergy(int index);
};
class Posture {
public:
    virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3();
    virtual void s4(); virtual void s5(); virtual void s6(); virtual void s7();
    virtual void s8(); virtual void s9();
};
struct FlagVecModule_base { char pad[8]; };
class FlagVecModule : public FlagVecModule_base {
public:
    virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3();
    virtual void s4(); virtual void s5(); virtual void s6(); virtual void s7();
    virtual void s8(); virtual void s9(); virtual void s10(); virtual void s11();
    virtual void s12(); virtual void s13(); virtual void s14(); virtual void s15();
    virtual void s16(); virtual void s17(); virtual void s18(); virtual void s19();
    virtual void s20(); virtual void s21(); virtual void s22(); virtual void s23();
    virtual void s24(); virtual void s25(); virtual void s26(); virtual void s27();
    virtual void s28(); virtual void s29(); virtual void s30(); virtual void s31();
    virtual void s32(); virtual void s33(); virtual void s34(); virtual void s35();
    virtual void s36(); virtual void s37(); virtual void s38(); virtual void s39();
    virtual void s40(); virtual void s41(); virtual void s42(); virtual void s43();
    virtual void s44(); virtual void s45(); virtual void s46(); virtual void s47();
    virtual void s48(); virtual void s49(); virtual void s50(); virtual void s51();
    virtual void s52(); virtual void s53(); virtual void s54(); virtual void s55();
    virtual void s56();
    virtual unsigned long long getSumSpeed(int flags, int unk);
};
struct Modules {
    char pad0[0xc];
    Posture* posture;
    FlagVecModule* vecmod;
    char pad1[0x68];
    Kinetic* kinetic;
};
struct soModuleAccesser { char pad[0xd8]; Modules* modules; };
extern "C" int fn_27_36D2CC(gfTask* self, NullableModule* m, soModuleAccesser* a) {
    float cy;
    const float* c = lbl_27_rodata_7B40;
    if (notifyEventAnimCmd__6WeaponFP9acAnimCmdP16soModuleAccesseri(self)) return 1;
    if (self->fn_27_24CC58(m->slot1()) == 0) return 0;
    if (m->slot2() <= -1 || m->slot2() >= 11) return 0;
    switch (m->slot2()) {
    case 6: {
        KineticEnergy* e = static_cast<KineticEnergy*>(__dynamic_cast(
            a->modules->kinetic->getEnergy(0), 0, __RTTI__21wnKineticEnergyNormal, __RTTI__15soKineticEnergy, 1));
        Vec2 v = unpack(e->getSpeed());
        float sum = v.x * v.x + v.y * v.y;
        float mag;
        if ((float)(double)nw4r::math::FAbs(sum) <= c[7]) mag = c[1];
        else mag = sum * rsqrtf(sum);
        Vec2 w = unpack(a->modules->vecmod->getSumSpeed(0xff, 0));
        double wy = w.y;
        double wx = w.x;
        float angB = (float)atan2(wx, wy);
        double vy = v.y;
        double vx = v.x;
        float angA = (float)atan2(vx, vy);
        float diff = angA - angB;
        a->modules->posture->s9();
        while (diff > c[9]) diff -= c[8];
        while (diff < c[10]) diff += c[8];
        float corr = c[1];
        if (c[1] != diff) {
            float r = angle__5Vec2fCFPC5Vec2f(&w, &v);
            float ar = (float)(double)nw4r::math::FAbs(r);
            float t;
            if (ar < c[11]) t = c[12] * (ar / c[9]);
            else t = c[13] * (ar / c[9]);
            if (diff < c[1]) t = -t;
            corr = r * t;
        }
        float ang = angA - corr;
        while (ang < c[10]) ang += c[8];
        while (ang > c[9]) ang -= c[8];
        cy = mag * (float)cos(ang);
        float sx = mag * (float)sin(ang);
        setSpeed(e, sx, cy);
        return 1;
    }
    default:
        break;
    }
    return 0;
}
}

// The weak destructor of the effect builder, out of line (the weapon builder destructor calls it).
template <typename T>
soEffectModuleBuilder<wnEffectModuleBuildConfig<0, 1, 0, 0, -1, T> >::~soEffectModuleBuilder() { }
template soEffectModuleBuilder<PlBC::EffectModuleBuildConfig>::~soEffectModuleBuilder();

#include <wn/wn_class_info_impl.h>
wnClassInfoImpl<13, wnPokeLizardonBreath, wnPokeLizardonBreathData> g_wnPokeLizardonBreathClassInfo;
