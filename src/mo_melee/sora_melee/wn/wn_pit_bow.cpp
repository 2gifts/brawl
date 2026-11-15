#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define FT_MODULE_BUILDER
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_TRANSITION_CTOR_INLINE
#define WN_TRANSITION_INFO_BARE
#define SO_GENERAL_WORK_SIMPLE_CTOR_INLINE
#define FT_GENERAL_WORK_ABSTRACT_DTOR_INLINE
#define WN_SIMPLE_INLINE_TEAM
#define WN_ACCESSER_CTOR_INLINE
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#define WN_PIT_BOW_FULL_BUILDER
#define WN_WEAPON_NO_COMMON_ENERGIES // the bow has no movement energies of its own (it follows Pit's hand)
#include <wn/wn_module_accesser_builder.h>
#include <wn/pit/wn_pit_bow.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <memory.h>

// BYTES: the empty base destructors that sora_melee always expands (they have no out-of-line copy there).
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }
inline soTransitionInfo::~soTransitionInfo() { }

// Registers the Pit bow article class (kind 90) in the class info table during static initialization.
wnClassInfoImpl<90, wnPitBow, wnPitBowData> g_wnPitBowClassInfo;

// BYTES: the constructor addresses each singleton record of the module builders by its own address; with data pooling
// they share one hoisted .bss anchor register, as they do in the copy the class info's createArray expands.
#pragma push
#pragma pool_data off
wnPitBow::wnPitBow(s32 articleId, const wnArticleBuildInfo& info, const wnPitBowData* data)
    : wnWeaponBuilder<wnPitBowModuleAccesserBuildConfig>(
          "PitBow", articleId, info,
          wnModuleAccesserBuildData(90, data,
              info.getHeapModule()->getInstanceHeap(),
              info.getHeapModule()->getNWModelInstanceHeap(),
              info.getHeapModule()->getNWMotionInstanceHeap(),
              static_cast<u8>(data->m_resGroupNo), -1, &g_soParamAccesserNull,
              0, 0, 0, 0, 0, 0, 0)) {
    Weapon::deactivate(false);
}
#pragma pop

void wnPitBow::activate(int founderTaskId, int resourceId, int texResId, bool unk1, bool unk2) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0, pos, 0.0f, -1, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagBit7 = 1;
    desc.flagBit6 = 1;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unkC = texResId;
    desc.unk10 = unk1;
    desc.unk14 = unk2;
    desc.unk4D = (desc.unk4D & ~0xC0) | 0x40;
    Weapon::activate(&desc);
}
