#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
#define FT_MODULE_BUILDER
#define WN_MARIO_FIREBALL_UNIT // the sora_melee article unit mode shared with the Mario fireball (builders out of line)
#define WN_ZELDA_LIGHTING_BOW_UNIT // the member layout and constructor of the module builder of this unit mode (wn/zelda/wn_zelda_lighting_bow.h)
#define WN_ARTICLE_BUILDER_DTOR_IN_CLASS // the module accesser builder destructor is a weak function of its own here
#define WN_WEAPON_NO_COMMON_ENERGIES // no kinetic module, so the weapon builder adds no energies
#define SO_OTHER_ANIM_CTOR // the motion builder owns one other-animation slot (constructed and destroyed as an array)
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_STATUS_NO_TAIL
#define WN_ARTICLE_ANIMCMD_INT // BYTES: the builder vtable takes Weapon's int-index notifyEventAnimCmd thunk (@72@), as in the original
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#include <wn/wn_module_accesser_builder.h>
#include <wn/zelda/wn_zelda_lighting_bow.h>
#include <wn/falco/wn_falco_blaster.h>
#include <wn/wn_class_info_impl.h>
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

// Registers the Falco Blaster article class (kind 35) in the class info table during static initialization.
wnClassInfoImpl<35, wnFalcoBlaster, wnFalcoBlasterData> g_wnFalcoBlasterClassInfo;

wnFalcoBlaster::wnFalcoBlaster(s32 articleId, const wnArticleBuildInfo& info, const wnFalcoBlasterData* data) :
    wnWeaponBuilder<wnFalcoBlasterModuleAccesserBuildConfig>(
        "FalcoBlaster", articleId, info,
        wnModuleAccesserBuildData(35, data, info.getHeapModule()->getInstanceHeap(),
                                  info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1,
                                  &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

// BYTES: the .text order of the original unit is the article constructor (with its weak copies: the weapon builder
// destructor and the builder destructors it calls), the parameter accesser destructor, getModuleAccesser, the module
// accesser builder constructor, then the article members; MWCC emits weak copies in definition order.
inline wnFalcoBlasterParamAccesser::~wnFalcoBlasterParamAccesser() { }

#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnFalcoBlasterModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>

void wnFalcoBlaster::activate(int founderTaskId, u32 mdlResId, u32 texResId, bool unk1, bool unk2, int team) {
    Vec3f origin(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = mdlResId;
    desc.unk8 = mdlResId;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = origin;
    desc.lr = 0.0f;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagBit7 = true;
    desc.flagBit6 = true;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = true;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    desc.unkC = texResId;
    desc.unk10 = unk1;
    desc.unk14 = unk2;
    Weapon::activate(&desc);
}

void wnFalcoBlaster::notifyEventLink(soLinkEventArgs* event, soModuleAccesser* moduleAccesser, StageObject* sender,
                                   int selector) {
    switch (event->m_eventKind) {
    case 0x838:
        return;
    default:
        Weapon::notifyEventLink(event, moduleAccesser, sender, selector);
        return;
    }
}

wnFalcoBlaster::~wnFalcoBlaster() { }
