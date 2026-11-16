#define WN_KINETIC_MEDIATOR_INLINE_DTOR // BYTES: the null mediator's destructor is inlined into the kinetic module builder's
#define SO_BUILDER_HELPERS_WEAK
#define IP_INPUT_BUTTON_RAW
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
// BYTES: link order. MWCC emits the weak vtables (and with them the batches of their inline virtual functions) in reverse
// order of the class definitions, and the overriders of one class in reverse declaration order. These headers come
// first, in the order the original defines the classes; the three opt-in macros move what the shared headers would
// otherwise define earlier.
#define SO_PARAM_CUSTOMIZE_IMPL_LATE
#define SO_KINETIC_MEDIATOR_NULL_UPDATE_REVERSED
#define SO_ARTICLE_LOG_HELPER_LATE
#include <ip/ip_null.h>
#include <so/team/so_team.h>
#include <so/resource/so_resource_id_accesser.h>
#include <so/team/so_team_impl.h>
#include <so/kinetic/so_kinetic_mediator_null.h>
#include <so/article/so_article_mediator_null.h>
#include <so/param/so_param_customize_module_impl.h>
#include <wn/wn_module_accesser_builder.h>
#include <wn/peach/wn_peach_kassar.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>

wnPeachKassar::wnPeachKassar(s32 articleId, const wnArticleBuildInfo& info, const wnPeachKassarData* data) :
    wnWeaponBuilder<wnPeachKassarModuleAccesserBuildConfig>(
        "PeachKassar", articleId, info,
        wnModuleAccesserBuildData(0x5C, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

// The two top bits of the first flag byte are written one by one (like the 0x4D bits): the paired 2-bit write gives the
// same instructions but one IR temporary less, which reorders the callee-saved registers (unkC, this, the float
// constant's address, team). Any extra integer temporary has the same effect on the colouring.
void wnPeachKassar::activate(int founderTaskId, int resourceId, int unkC, int team) {
    Vec3f position(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = position;
    desc.lr = 0.0f;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagBit7 = 1;
    desc.flagBit6 = 1;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.flag4DBit7 = 0;
    desc.flag4DBit6 = 0;
    desc.unkC = unkC;
    Weapon::activate(&desc);
}
