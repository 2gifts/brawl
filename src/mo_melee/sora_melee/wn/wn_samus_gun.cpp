#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
#define FT_MODULE_BUILDER
#define WN_ARTICLE_ANIMCMD_INT
#define WN_PARAM_ACCESSER_INLINE_DTOR
#define WN_STATUS_PROC_VECTOR2_DEFAULT_CTOR
#define WN_STATUS_PLAIN_NULL_ARRAY
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define FT_REL_LINK_EXTERN
#include <wn/wn_module_accesser_builder.h>
#include <wn/samus/wn_samus_gun.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <memory.h>

typedef wnSamusGunModuleAccesserBuildConfig SgBC;

// The original keeps the builders' constructors and destructors out of line.
#pragma push
#pragma dont_inline on
template soInsideEventManageModuleBuilder<SgBC::InsideEventManageModuleBuildConfig, SgBC::InsideEventManageModuleTypes>::soInsideEventManageModuleBuilder();
template soMotionModuleBuilder<SgBC::MotionModuleBuildConfig>::soMotionModuleBuilder(soModuleAccesser*, void*);
#pragma pop

wnSamusGun::wnSamusGun(s32 articleId, const wnArticleBuildInfo& info, const wnSamusGunData* data)
    : wnWeaponBuilder<wnSamusGunModuleAccesserBuildConfig>(
          "SamusGun", articleId, info,
          wnModuleAccesserBuildData(0x58, data,
              info.getHeapModule()->getInstanceHeap(),
              info.getHeapModule()->getNWModelInstanceHeap(),
              info.getHeapModule()->getNWMotionInstanceHeap(),
              static_cast<u8>(data->m_resGroupNo), 0, &g_soParamAccesserNull,
              0, 0, 0, 0, 0, 0, 0)) {
    soStatusUniqProcess* uniqProcs[1] = {0};
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 1);
    Weapon::deactivate(false);
}

void wnSamusGun::activate(int founderTaskId, int unk1, int unk2, int unk3, int unk4, int team) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = unk1;
    desc.unk8 = unk1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = pos;
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
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unkC = unk2;
    desc.unk10 = unk3;
    desc.unk14 = unk4;
    desc.unk4DHigh = 0;
    desc.flag4DBit6 = 1;
    Weapon::activate(&desc);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}
