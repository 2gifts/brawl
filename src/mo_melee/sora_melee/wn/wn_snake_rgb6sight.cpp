#define WN_SNAKE_RGB6SIGHT_UNIT // the sora_melee unit mode of this article (the module builder members and methods of the Snake pools header)
#define WN_ANIMCMD_RESET_MODE_BOX // BYTES: word-aligned reset-mode temporaries (frame size of the anim cmd sub builder)
#define WN_ANIMCMD_STACK_PTR // BYTES: the interpreter stacks stay in a saved register across getManageId (ft_builder_animcmd.h)
#define WN_ANIMCMD_PRESENTER_LOCAL_STORE // BYTES: the interpreters' presenters keep the looked-up list in a local
#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: activate copies the Vec3f position as an aggregate (word moves), see mt_vector.h
#define WN_EVENT_UNIT_DTOR_INLINE // BYTES: the inside event builder destructor expands the event unit destructors
#define WN_STATUS_MODULE_KIND_COUNT_ZERO // BYTES: the status module is built with a status kind count of 0 (ft_builder_status.h)
#define WN_EVENT_UNIT_NULL_CTOR_OWNER // this unit owns the weak constructors of the disabled event units (so_event_system.h)
#define SO_COLLISION_GROUP_ALIGNED // the collision group records are word-aligned (so_collision_group.h)
#define WN_KINETIC_MEDIATOR_INLINE_DTOR // the kinetic builder destructor this unit owns expands the empty mediator destructor
#define WN_KINETIC_GRAVITY_INLINE_DTOR // the article destructor expands the empty gravity energy destructor
#define SO_KINETIC_ENERGY_3F_FULL // the weapon builder constructs the rotation energy inline
#define FT_MODULE_BUILDER
#define WN_MARIO_FIREBALL_UNIT // the sora_melee article unit mode shared with the Mario fireball (builders out of line)
#define WN_ZELDA_LIGHTING_BOW_UNIT // the member layout of the module builder of this unit mode (wn/zelda/wn_zelda_lighting_bow.h)
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define SO_OTHER_ANIM_CTOR // BYTES: the sora_melee motion builder constructs and destroys its other-animation slot
#define WN_TRANSITION_CTOR_INLINE
#define WN_ARTICLE_ANIMCMD_INT // BYTES: the builder vtable takes Weapon's int-index notifyEventAnimCmd thunk, as in the original
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#include <wn/wn_module_accesser_builder.h>
#include <wn/snake/wn_snake_status_builder.h> // before the bow header, whose config instantiates the generic spelling of it
#include <wn/zelda/wn_zelda_lighting_bow.h>
#include <wn/snake/wn_snake_rgb6_sight_parts.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <gf/gf_task_scheduler.h>
#include <memory.h>

// BYTES: the empty base destructors that sora_melee always expands (they have no out-of-line copy there).
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }
// BYTES: the controller module builder destructor expands the link reference's (empty) destructor and calls its base.
inline soControllerModuleLinkRef::~soControllerModuleLinkRef() { }

// The weapon builder ctor and the weak module builders (this unit owns them in the link order).
#pragma push
#pragma pool_data off
wnSnakeRgb6Sight::wnSnakeRgb6Sight(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeRgb6SightData* data) :
    wnWeaponBuilder<wnSnakeRgb6SightModuleAccesserBuildConfig>(
        "SnakeRgb6Sight", articleId, info,
        wnModuleAccesserBuildData(0x88, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1,
                                  &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}
#pragma pop

#define WN_ZELDA_BUILDER_CTOR_WEAK
#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnSnakeRgb6SightModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>

void wnSnakeRgb6Sight::renderDebug() {
    StageObject::renderDebug();
}
template class wnExtendParamAccesser<3999, 6, 23999, 0, 0, 0>;

// TODO: clean up, give variables real names
namespace jj_3B1720 {
extern "C" void __dl__FPv(void*);

extern "C" void* __dt__29wnSnakeRgb6SightParamAccesserFv(void* ptr, int count) {
    if (ptr != 0 && count > 0)
        __dl__FPv(ptr);
    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_3B2C90 {
#pragma push
#pragma optimization_level 2
#pragma scheduling off
#pragma opt_propagation off

struct Owner;
class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19(int, Owner*);
};
struct Middle {
    char pad[0x64];
    Object_vptr_0* target;
};
struct Owner {
    char pad[0xD8];
    Middle* middle;
};
struct Object {
    char pad[0x60];
    Owner* owner;
};
extern "C" void enableOperate__16wnSnakeRgb6SightFv(Object* self) {
    Owner* owner = self->owner;
    int value = 0x12000003;
    owner->middle->target->slot19(value, owner);
}
#pragma pop
}

// TODO: clean up, give variables real names
namespace jj_3B2CB4 {
#pragma push
#pragma optimization_level 2
#pragma scheduling off
#pragma opt_propagation off

struct Owner;
class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18(int, Owner*);
};
struct Middle { char pad[0x64]; Object_vptr_0* target; };
struct Owner { char pad[0xD8]; Middle* middle; };
struct Object { char pad[0x60]; Owner* owner; };
extern "C" void unableOperate__16wnSnakeRgb6SightFv(Object* self) {
    Owner* owner = self->owner;
    int value = 0x12000003;
    owner->middle->target->slot18(value, owner);
}
#pragma pop
}

// TODO: clean up, give variables real names
namespace jj_3B3248 {
extern "C" void fn_27_3B13C0(void*);

extern "C" void fn_27_3B3248(void* self) {
    fn_27_3B13C0(reinterpret_cast<u8*>(self) - 0x40);
}
}

// TODO: clean up, give variables real names
namespace jj_3B3250 {
extern "C" void fn_27_3B13C0(void*);

extern "C" void fn_27_3B3250(void* self) {
    fn_27_3B13C0(reinterpret_cast<u8*>(self) - 0x90);
}
}

// TODO: clean up, give variables real names
namespace jj_3B3258 {
extern "C" void fn_27_3B13C0(void*);

extern "C" void fn_27_3B3258(void* self) {
    fn_27_3B13C0(reinterpret_cast<u8*>(self) - 0x88);
}
}

// TODO: clean up, give variables real names
namespace jj_3B3270 {
extern "C" void fn_27_3B319C(void*);

extern "C" void fn_27_3B3270(void* self) {
    fn_27_3B319C(reinterpret_cast<u8*>(self) - 0x88);
}
}

