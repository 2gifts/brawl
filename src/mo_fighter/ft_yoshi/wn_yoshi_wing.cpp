#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/yoshi/wn_yoshi_wing_build.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>

wnYoshiWing::wnYoshiWing(s32 articleId, const wnArticleBuildInfo& info, const wnYoshiWingData* data) :
    wnWeaponBuilder<wnYoshiWingModuleAccesserBuildConfig>(
        "YoshiWing", articleId, info,
        wnModuleAccesserBuildData(0x3d, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

// Starts the wing at the origin of its founder, facing straight ahead (lr 0).
void wnYoshiWing::activate(int founderTaskId, int resourceId, int team) {
    Vec3f position(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0, position, 0.0f, team, 0, 2,
                        0x80, 0, 0x35F, 0);
    desc.flagBit7 = 1;
    desc.flagBit6 = 1;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.flag4DBit7 = 0;
    desc.flag4DBit6 = 0;
    Weapon::activate(&desc);
}
