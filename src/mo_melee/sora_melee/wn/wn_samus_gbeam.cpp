#define WN_ANIMCMD_PRESENTER_LOCAL_STORE // BYTES: the interpreters' presenters keep the looked-up list in a local (98.3% vs 97.2%), as in the Mario fireball
#define WN_ANIMCMD_RESET_MODE_BOX // BYTES: word-aligned reset-mode temporaries (frame size of the anim cmd sub builder), as in the Mario fireball
#define WN_ANIMCMD_STACK_PTR_INLINE_CAST // BYTES: stack capacity 2, converted through an inline function (ft_builder_animcmd.h)
#define WN_ANIMCMD_STACK_PTR // BYTES: the interpreter stacks stay in a saved register across getManageId (ft_builder_animcmd.h)
#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define WN_ACTIVATE_DESC_SIGNED_HIGH_PAIR // BYTES: activate sets the top flag pair through a signed two-bit field (see activate)
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: activate copies the Vec3f position as an aggregate (word moves), see mt_vector.h
#define FT_MODULE_BUILDER
#define WN_MARIO_FIREBALL_UNIT // the sora_melee article unit mode shared with the Mario fireball (builders out of line)
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_STATUS_NO_TAIL
#define SO_OTHER_ANIM_CTOR // BYTES: the sora_melee motion builder constructs and destroys its other-animation slot (soOtherAnim has real ctor and dtor)
#define WN_TRANSITION_CTOR_INLINE
#define WN_ARTICLE_ANIMCMD_INT // BYTES: the builder vtable takes Weapon's int-index notifyEventAnimCmd thunk, as in the original
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#include <wn/wn_module_accesser_builder.h>
#include <wn/samus/wn_samus_gbeam.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_lasso_status_view.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <gf/gf_task_scheduler.h>

// BYTES: the empty base destructors that sora_melee always expands (they have no out-of-line copy there).
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }

// HYPOTHESIS: the model node the beam attaches to (-1: none); the first data word of the unit.
static int s_attachNodeId = -1;

extern soStatusUniqProcess g_wnLassoStatusUniqProcessShoot;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessRewind;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessMotionPull;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessHang;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessHangRewind;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessReach;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessFailure;

wnSamusGBeam::wnSamusGBeam(s32 articleId, const wnArticleBuildInfo& info, const wnSamusGBeamData* data) :
    wnWeaponBuilder<wnSamusGBeamModuleAccesserBuildConfig>(
        "SamusGBeam", articleId, info,
        wnModuleAccesserBuildData(0x84, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1,
                                  &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    uniqProcs[1] = &g_wnLassoStatusUniqProcessShoot;
    uniqProcs[3] = &g_wnLassoStatusUniqProcessRewind;
    uniqProcs[4] = &g_wnLassoStatusUniqProcessMotionPull;
    uniqProcs[5] = &g_wnLassoStatusUniqProcessHang;
    uniqProcs[6] = &g_wnLassoStatusUniqProcessHangRewind;
    uniqProcs[7] = &g_wnLassoStatusUniqProcessReach;
    uniqProcs[8] = &g_wnLassoStatusUniqProcessFailure;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 9);
    Weapon::deactivate(false);
}

// BYTES: the builder destructor is defined here, before the parameter accesser destructor, and compiled with inlining off
// (the big module builder destructors stay calls): the original emits the status and motion builder destructors right after
// the weapon builder destructor, in the order this body tears them down. WN_ARTICLE_BUILDER_DTOR_IN_CLASS keeps
// wn_zelda_lighting_bow_builder_ctor.h from defining it again after the constructor.
#pragma dont_inline on
inline soModuleAccesserBuilder<wnSamusGBeamModuleAccesserBuildConfig>::~soModuleAccesserBuilder() { }
#pragma dont_inline reset

inline wnSamusGBeamParamAccesser::~wnSamusGBeamParamAccesser() { }

#define WN_ARTICLE_BUILDER_DTOR_IN_CLASS
#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnSamusGBeamModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>

// BYTES: defined here (see the specialization in wn_samus_gbeam.h); the body is the shared template's.
wnAnimCmdModuleSubBuilder<wnSamusGBeamModuleAccesserBuildConfig::AnimCmdModuleSubBuildConfig>::wnAnimCmdModuleSubBuilder(
    soModuleAccesser* acc, const soModuleAccesserBuildData& bd) :
    m_unit0(acc, bd.getAnimCmdData(0, 0), bd.getAnimCmdData(0, 1), bd.getAnimCmdData(0, 2)),
    m_unit1(acc, bd.getAnimCmdData(1, 0), bd.getAnimCmdData(1, 1), bd.getAnimCmdData(1, 2)),
    m_unit2(acc, bd.getAnimCmdData(2, 0), bd.getAnimCmdData(2, 1), bd.getAnimCmdData(2, 2)),
    m_unit3(acc, bd.getAnimCmdData(3, 0), bd.getAnimCmdData(3, 1), bd.getAnimCmdData(3, 2)),
    m_unit4(acc),
    m_unit5(acc, bd.getAnimCmdData(5, 0), bd.getAnimCmdData(5, 1), bd.getAnimCmdData(5, 2)) { }

// Shoots the beam: the article starts at pos facing lr, attached to the founder's resource, and enters the lasso shoot status.
// The last parameter is a bool (the original passes it to setFlag(bool, u32) unchanged and compares it with cmplwi) and the
// position is a `const Vec3f*` (the Mario fireball's form).
// BYTES: the two top bits of the descriptor's first flag byte are written through a signed two-bit field set to -1
// (WN_ACTIVATE_DESC_SIGNED_HIGH_PAIR, wn/wn_activate_desc.h). MWCC shares equal integer constants inside a function whatever
// their type: with `flagsHighPair = 3` the 3 of `unk38 = 3` is created at the flag write (it takes the register the last flag
// operation needs), and the two-statement form (`flagBit7 = 1; flagBit6 = 1;`) or `flags |= 0xc0` renumbers the saved
// register that holds -1. A -1 is already there (unk10), so nothing new is created.
// HYPOTHESIS: the original field is a signed two-bit value whose -1 means "unset"; its name is not known.
void wnSamusGBeam::activate(int founderTaskId, int etcResourceId, int team, float lr, const Vec3f* pos, bool attack) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = etcResourceId;
    desc.unk8 = etcResourceId;
    desc.unkC = etcResourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = *pos;
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    // MATCH-ONLY: the two top bits written as a signed two-bit -1 only so that MWCC does not create the constant 3, which it
    // would share with unk38 below and give the last flag operation another register (wn_activate_desc.h).
    desc.flagsHighPairSigned = -1;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.flag4DBit7 = 0;
    desc.flag4DBit6 = 0;
    desc.unk38 = 3;
    desc.unk3C = 0xff;
    Weapon::activate(&desc);
    soResourceModule& resource = m_moduleAccesser->getResourceModule();
    wnLassoAltStatusView* alt = wnLassoGetAltStatus(m_moduleAccesser);
    int nodeId = s_attachNodeId;
    int noValue = 0;
    alt->unk18Attach(resource.getResourceIdAccesser()->getBinResId(), noValue, nodeId, nodeId, 1, 0, noValue, -1);
    m_moduleAccesser->getWorkManageModule().setFlag(attack, 0x22000000);
    m_moduleAccesser->getWorkManageModule().setInt(0x50, 0x10000005);
    m_moduleAccesser->getWorkManageModule().setInt(2, 0x10000006);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    soPostureModule& posture = m_moduleAccesser->getPostureModule();
    wnLassoGetAltStatus(m_moduleAccesser)->unkDCFloat(posture.getScale());
    wnLassoGetAltStatus(m_moduleAccesser)->unkA4Int(6);
    static_cast<wnSamusGBeamStatusView*>(static_cast<wnLassoViewPad8*>(wnLassoGetAltStatus(m_moduleAccesser)))->unkB8Int(1);
    if (attack == 1) {
        m_moduleAccesser->getWorkManageModule().setInt(5, 0x20000000);
        m_moduleAccesser->getWorkManageModule().setFloat(32.064f, 0x21000000);
        m_moduleAccesser->getWorkManageModule().setFloat(100.0f, 0x21000001);
        m_moduleAccesser->getWorkManageModule().setFloat(m_param->unk14, 0x21000002);
    }
}

// TODO(link): every function is exact (26/26), the .text order equals the original's and the unit has its .text, .rodata,
// .data and .bss ranges in splits.txt. What is left is naming: 429 symbols this object defines have no name in sora_melee's
// symbols.txt (209 functions, 196 data objects, 24 .bss objects; a few are the unit's own vtables and RTTI, the rest are weak
// copies the linker would keep, growing the REL), and it calls 20 soEventUnitNull constructors/destructors (the turn
// inside-event types) that no symbols.txt names. Pairing through the relocations of the already named functions, the RTTI
// name strings and the vtables (TIPS, "Linking sora_melee fighter articles") gives about 130 of them; the destructors and
// vtables behind weak copies whose bodies differ from the copy the original kept (the model, effect and link builders)
// still need masked-byte pairing, and several original vtables and RTTI records sit inside unsplit data labels that need
// new symbols. The unit's own .data, .rodata and .bss objects come out in the original order (the 20.0f of the weak
// soModuleAccesserBuildData::getSlopeAngleLimit copy is stripped with that copy).
// The original's soInstanceUnitFullProperty<soTransitionTerm> constructor (a weak copy in another unit) also initializes the
// term's fields, which our header's implicit constructor does not.
wnSamusGBeam::~wnSamusGBeam() { }
