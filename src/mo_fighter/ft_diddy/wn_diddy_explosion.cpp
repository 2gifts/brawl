#define WN_WEAPON_BUILDER
#define WN_EXPLOSION_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#include <wn/diddy/wn_diddy_explosion.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>

wnDiddyExplosion::wnDiddyExplosion(s32 articleId, const wnArticleBuildInfo& info, const wnDiddyExplosionData* data) :
    wnWeaponBuilder<wnDiddyExplosionModuleAccesserBuildConfig>(
        "DiddyExplosion", articleId, info,
        wnModuleAccesserBuildData(0x39, data, info.heapModule->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.heapModule->getNWMotionInstanceHeap(), data->m_resGroupNo, -1,
                                  &g_soParamAccesserNull, 0, 0, 0, 0, 0, 0, 0)),
    unk1F94(data->unk34) {
    soStatusUniqProcess* uniqProc = 0;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(&uniqProc, 1);
    Weapon::deactivate(false);
}

void wnDiddyExplosion::activate(int founderTaskId, int team, Vec3f* position, float lr) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = 0xFFFF;
    desc.unk8 = 0xFFFF;
    desc.unkC = 0xFFFF;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
#ifdef MATCHING
    __memcpy(&desc.pos, position, sizeof(Vec3f));
#else
    desc.pos = *position;
#endif
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getWorkManageModule().setInt(unk1F94[0], 0x10000005);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}

void wnDiddyExplosion::onDeactivate() { }
