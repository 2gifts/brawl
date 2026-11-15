#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define FT_MODULE_BUILDER
#define WN_MARIO_FIREBALL_UNIT // the sora_melee article unit mode shared with the Mario fireball
#define WN_ZELDA_LIGHTING_BOW_UNIT // the member layout and constructor of the module builder of this unit mode (wn/zelda/wn_zelda_lighting_bow.h)
#define WN_ARTICLE_BUILDER_CTOR_INLINE // the bow expands that constructor in its weapon builder constructor
#define WN_WEAPON_BUILDER
#define WN_STATUS_NO_TAIL
// BYTES: the weapon builder constructor expands the transition module, team and animation command builder constructors.
#define WN_TRANSITION_CTOR_INLINE
#define WN_SIMPLE_INLINE_TEAM
#define WN_ACCESSER_CTOR_INLINE
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#define WN_TOON_LINK_BOW_FULL_BUILDER
#include <wn/wn_module_accesser_builder.h>
#include <wn/zelda/wn_zelda_lighting_bow.h>
#include <wn/toon_link/wn_toon_link_bow.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <memory.h>

// BYTES: the empty base destructors that sora_melee always expands (they have no out-of-line copy there).
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }
// The same for the small getters the weapon builder constructor expands.
inline soEventObserverRegistrationDesc* soEventObserverRegistrationDescNull::getNullInstance() { return FT_NULL_OBSERVER_DESC; }
inline int gfTask::getId() { return m_taskId; }
inline gfTask::Category gfTask::getCategory() { return m_taskCategory; }

#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnToonLinkBowModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>

// Registers the Toon Link bow article class (kind 139) in the class info table during static initialization.
wnClassInfoImpl<139, wnToonLinkBow, wnToonLinkBowData> g_wnToonLinkBowClassInfo;

// BYTES: the article constructor calls the weapon builder constructor and the heap module getter out of line.
#pragma dont_inline on
wnToonLinkBow::wnToonLinkBow(s32 articleId, const wnArticleBuildInfo& info, const wnToonLinkBowData* data)
    : wnWeaponBuilder<wnToonLinkBowModuleAccesserBuildConfig>(
          "ToonLinkBow", articleId, info,
          wnModuleAccesserBuildData(139, data,
              info.getHeapModule()->getInstanceHeap(),
              info.getHeapModule()->getNWModelInstanceHeap(),
              info.getHeapModule()->getNWMotionInstanceHeap(),
              static_cast<u8>(data->m_resGroupNo), -1, soParamAccesserNull::getNullInstance(),
              0, 0, 0, 0, 0, 0, 0)) {
    Weapon::deactivate(false);
}
#pragma dont_inline reset

// BYTES: the original addresses each singleton record of the module builders by its own address; with data pooling
// they share one hoisted .bss anchor register (as the motion builder of wn_mario_fireball.cpp).
#pragma push
#pragma pool_data off
wnWeaponBuilder<wnToonLinkBowModuleAccesserBuildConfig>::wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info,
                                                                     const wnModuleAccesserBuildData& bd) :
    Weapon(name, bd.m_kind, bd.m_data, articleId, &info, m_modules.getModuleAccesser(), true), m_modules(this, bd) {
    Weapon::initialize();
}
#pragma pop

void wnToonLinkBow::activate(int founderTaskId, int resourceId) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = pos;
    desc.lr = 0.0f;
    desc.team = -1;
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
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
}
