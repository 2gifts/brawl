#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
#define FT_MODULE_BUILDER
#define WN_MARIO_FIREBALL_UNIT // the sora_melee article unit mode shared with the Mario fireball (builders out of line)
#define WN_ZELDA_LIGHTING_BOW_UNIT // the member layout and constructor of the module builder of this unit mode (wn/zelda/wn_zelda_lighting_bow.h)
#define WN_KIRBY_PIKMIN_UNIT // the complete article class (wn/pikmin/wn_kirby_pikmin.h)
#define SO_OTHER_ANIM_CTOR // the motion builder owns one other-animation slot (constructed and destroyed as an array)
#define WN_TRANSITION_CTOR_INLINE // the motion builder constructor of this unit expands the transition module constructor
#define WN_ANIMCMD_RESET_MODE_BOX // BYTES: word-aligned reset-mode temporaries (frame size of the anim cmd sub builder)
#define WN_ANIMCMD_PRESENTER_LOCAL_STORE // BYTES: the interpreters' presenters keep the looked-up list in a local
#define WN_ANIMCMD_STACK_PTR // BYTES: the interpreter stacks stay in a saved register across getManageId (ft_builder_animcmd.h)
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_STATUS_NO_TAIL
#define WN_ARTICLE_ANIMCMD_INT // BYTES: the builder vtable takes Weapon's int-index notifyEventAnimCmd thunk (@72@), as in the original
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#include <wn/wn_module_accesser_builder.h>
#include <wn/zelda/wn_zelda_lighting_bow.h>
#include <so/so_value_accesser.h>
#include <wn/pikmin/wn_kirby_pikmin.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <gf/gf_task_scheduler.h>

// BYTES: the empty base destructors that sora_melee always expands (they have no out-of-line copy there).
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }

// Registers the Kirby's Pikmin article class (kind 150) in the class info table during static initialization.
wnClassInfoImpl<150, wnKirbyPikmin, wnKirbyPikminData> g_wnKirbyPikminClassInfo;

wnKirbyPikmin::wnKirbyPikmin(s32 articleId, const wnArticleBuildInfo& info, const wnKirbyPikminData* data) :
    wnWeaponBuilder<wnKirbyPikminModuleAccesserBuildConfig>(
        "KirbyPikmin", articleId, info,
        wnModuleAccesserBuildData(150, data, info.getHeapModule()->getInstanceHeap(),
                                  info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1,
                                  &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param), m_data(data), m_animCmdTable0(),
    m_animCmdPack0(&m_animCmdTable0, (soArrayFixed<const acAnimCmdConv*>*)g_soAnimCmdAddrArrayNull,
                   (soArrayFixed<const acAnimCmdConv*>*)g_soAnimCmdAddrArrayNull),
    m_animCmdTable1(),
    m_animCmdPack1(&m_animCmdTable1, (soArrayFixed<const acAnimCmdConv*>*)g_soAnimCmdAddrArrayNull,
                   (soArrayFixed<const acAnimCmdConv*>*)g_soAnimCmdAddrArrayNull),
    m_animCmdTable2(),
    m_animCmdPack2(&m_animCmdTable2, (soArrayFixed<const acAnimCmdConv*>*)g_soAnimCmdAddrArrayNull,
                   (soArrayFixed<const acAnimCmdConv*>*)g_soAnimCmdAddrArrayNull),
    m_hitStatusTable() {
    m_moduleAccesser->getAnimCmdModule().unk44(1, &m_animCmdPack0);
    m_moduleAccesser->getAnimCmdModule().unk44(2, &m_animCmdPack1);
    m_moduleAccesser->getAnimCmdModule().unk44(3, &m_animCmdPack2);
    m_moduleAccesser->getVisibilityModule().setStatusDefault(0, 0, true);
    m_moduleAccesser->getVisibilityModule().setDefaultAll();
    m_moduleAccesser->getWorkManageModule().setInt(0, 0x10000006);
    Weapon::deactivate(false);
}

// BYTES: the original addresses each singleton's destructor-chain record by its own address; with data pooling the
// records share one hoisted .bss anchor register. The pragma applies where the template is instantiated.
#pragma push
#pragma pool_data off
template soMotionModuleBuilder<
    wnKirbyPikminModuleAccesserBuildConfig::MotionModuleBuildConfig>::soMotionModuleBuilder(soModuleAccesser*, void*);
#pragma pop

inline wnKirbyPikminParamAccesser::~wnKirbyPikminParamAccesser() { }

#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnKirbyPikminModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>

bool wnKirbyPikmin::notifyEventCollisionAttackCheck(u32 flags) {
    if ((flags & 7) != 0) {
        return false;
    }
    int reflected = 0;
    if ((flags & 8) != 0) {
        if (m_moduleAccesser->getReflectModule().isReflect() == 1) {
            reflected = 1;
        }
    }
    if (reflected != 0) {
        Vec2f accel(1.0f, 1.0f);
        float speedY = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA0, 0);
        Vec2f speed(-m_moduleAccesser->getReflectModule().getSpeedMul(), speedY);
        return reflectRequest(&speed, &accel);
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

void wnKirbyPikmin::shoot(Vec3f* position, float lr) {
    if (m_moduleAccesser->getLinkModule().isModelConstraint() == true) {
        m_moduleAccesser->getLinkModule().removeModelConstraint(true);
    }
    m_moduleAccesser->getStatusModule().changeStatusForce(1, m_moduleAccesser);
    m_moduleAccesser->getPostureModule().setPos(position);
    m_moduleAccesser->getPostureModule().setLr(lr);
    updateNodeSRT();
    wnKineticEnergyNormal* movement = dynamic_cast<wnKineticEnergyNormal*>(
        m_moduleAccesser->getKineticModule().getEnergy(0));
    if (movement) {
        const float* parameters = m_data->m_speedTable[m_moduleAccesser->getWorkManageModule().getInt(0x10000005)];
        movement->m_speed = Vec2f(parameters[2] * (m_moduleAccesser->getPostureModule().getLr() < 0.0f ? -1 : 1), parameters[4]);
        float speedLimitX = parameters[3];
        movement->m_accel = Vec2f(0.0f, -parameters[0]);
        movement->m_speedLimit = Vec2f(speedLimitX, parameters[1]);
        movement->enable();
    }
}

// TODO: clean up, give variables real names
namespace jj_30EBD8 {
extern "C" {
extern char __RTTI__21wnKineticEnergyNormal[];
extern char __RTTI__15soKineticEnergy[];
extern const float lbl_27_rodata_65B0;
extern const float lbl_27_rodata_65B4;
void* __dynamic_cast(void*, long, const void*, const void*, short);
void notifyEventChangeStatus__6WeaponFiiP12soStatusDataP16soModuleAccesser(void*, int);
}

struct Vec2f {
    float x, y;
    Vec2f() {}
    Vec2f(float a, float b) : x(a), y(b) {}
};

static inline const float& asFloat(const unsigned long long& v) {
    return *(const float*)&v;
}

struct Kinetic {
    virtual void d0();
    virtual void d1();
    virtual void d2();
    virtual void d3();
    virtual void d4();
    virtual void d5();
    virtual void* get(int);
};

struct Work {
    virtual void d0();
    virtual void d1();
    virtual void d2();
    virtual void d3();
    virtual int getInt(unsigned long);
};

struct Modules {
    char pad0[0x64];
    Work* work;
    char pad1[0x7c - 0x68];
    Kinetic* kinetic;
};

struct Accesser {
    char pad[0xd8];
    Modules* modules;
};

struct Entry {
    char pad[0x14];
    float x;
    float y;
};

struct Table {
    char pad[0x70];
    Entry* entries;
};

struct Energy {
    virtual void d0();
    virtual void d1();
    virtual unsigned long long getRaw();
    virtual void d3();
    virtual void d4();
    virtual void d5();
    virtual void d6();
    virtual void d7();
    virtual void d8();
    virtual void d9();
    virtual void d10();
    virtual void d11();
    virtual void update();
    char pad4[4];
    Vec2f m8;
    char pad10[8];
    Vec2f m18;
    Vec2f m20;
    Vec2f m28;
};

struct Self {
    char pad[0x60];
    Accesser* accesser;
    char pad2[0x2044 - 0x64];
    Table* table;
};

extern "C" void fn_27_30EBD8(Self* self, int arg) {
    notifyEventChangeStatus__6WeaponFiiP12soStatusDataP16soModuleAccesser(self, arg);
    if (arg == 2) {
        Energy* e = (Energy*)__dynamic_cast(
            self->accesser->modules->kinetic->get(0), 0,
            __RTTI__21wnKineticEnergyNormal, __RTTI__15soKineticEnergy, 0);
        if (e != 0) {
            Entry* ent = &self->table->entries[self->accesser->modules->work->getInt(0x10000005)];
            e->m8 = Vec2f(asFloat(e->getRaw()), lbl_27_rodata_65B4);
            e->m18 = Vec2f(lbl_27_rodata_65B4, lbl_27_rodata_65B4);
            e->m20 = Vec2f(ent->x, lbl_27_rodata_65B0);
            e->m28 = Vec2f(ent->y, lbl_27_rodata_65B4);
            e->update();
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_30F788 {
extern "C" void fn_27_30C060(void*);

extern "C" void fn_27_30F788(void* self) {
    fn_27_30C060(reinterpret_cast<u8*>(self) - 0x40);
}
}

// TODO: clean up, give variables real names
namespace jj_30F790 {
extern "C" void fn_27_30C060(void*);

extern "C" void fn_27_30F790(void* self) {
    fn_27_30C060(reinterpret_cast<u8*>(self) - 0x90);
}
}

// TODO: clean up, give variables real names
namespace jj_30F798 {
extern "C" void fn_27_30C060(void*);

extern "C" void fn_27_30F798(void* self) {
    fn_27_30C060(reinterpret_cast<u8*>(self) - 0x88);
}
}

// TODO: clean up, give variables real names
namespace jj_30F7A0 {
extern "C" void __dt__13wnKirbyPikminFv(void*);

extern "C" void fn_27_30F7A0(void* self) {
    __dt__13wnKirbyPikminFv(reinterpret_cast<u8*>(self) - 0x40);
}
}

// TODO: clean up, give variables real names
namespace jj_30F7A8 {
extern "C" void fn_27_30EBD8(void*);

extern "C" void fn_27_30F7A8(void* self) {
    fn_27_30EBD8(reinterpret_cast<u8*>(self) - 0x64);
}
}

// TODO: clean up, give variables real names
namespace jj_30F7B0 {
extern "C" void notifyEventCollisionAttackCheck__13wnKirbyPikminFUl(void*);

extern "C" void fn_27_30F7B0(void* self) {
    notifyEventCollisionAttackCheck__13wnKirbyPikminFUl(reinterpret_cast<u8*>(self) - 0x70);
}
}

// TODO: clean up, give variables real names
namespace jj_30F7B8 {
extern "C" void __dt__13wnKirbyPikminFv(void*);

extern "C" void fn_27_30F7B8(void* self) {
    __dt__13wnKirbyPikminFv(reinterpret_cast<u8*>(self) - 0x90);
}
}

// TODO: clean up, give variables real names
namespace jj_30F7C0 {
extern "C" void fn_27_30F510(void*);

extern "C" void fn_27_30F7C0(void* self) {
    fn_27_30F510(reinterpret_cast<u8*>(self) - 0x88);
}
}

// TODO: clean up, give variables real names
namespace jj_30F7C8 {
extern "C" void __dt__13wnKirbyPikminFv(void*);

extern "C" void fn_27_30F7C8(void* self) {
    __dt__13wnKirbyPikminFv(reinterpret_cast<u8*>(self) - 0x88);
}
}

wnKirbyPikmin::~wnKirbyPikmin() { }

bool wnKirbyPikmin::reflectRequest(Vec2f* speedMul, Vec2f* accelMul) {
    m_moduleAccesser->getPostureModule().reverseLr();
    m_moduleAccesser->getPostureModule().updateRotYLr();
    int team = m_moduleAccesser->getReflectModule().getTeam();
    m_moduleAccesser->getTeamModule().setTeam(team, true);
    m_moduleAccesser->getTeamModule().setHitTeam(team);
    m_moduleAccesser->getTeamModule().setTeamOwnerId(m_moduleAccesser->getReflectModule().getTaskId());
    if (speedMul->m_x != 1.0f || speedMul->m_y != 1.0f) {
        Vec3f mul(speedMul->m_x, speedMul->m_y, 1.0f);
        soKineticEnergy::AttributeFlag attr(1);
        m_moduleAccesser->getKineticModule().mulSpeed(&mul, attr);
    }
    if (accelMul->m_x != 1.0f || accelMul->m_y != 1.0f) {
        Vec3f mul(accelMul->m_x, accelMul->m_y, 1.0f);
        soKineticEnergy::AttributeFlag attr(1);
        m_moduleAccesser->getKineticModule().mulAccel(&mul, attr);
    }
    return false;
}
