#define WN_WEAPON_BUILDER
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#include <so/anim/so_anim_obj.h>
#include <wn/wn_module_accesser_builder.h>
#include <wn/gamewatch/wn_game_watch_oil.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/resource/so_resource_module_impl.h>

wnGameWatchOil::wnGameWatchOil(s32 articleId, const wnArticleBuildInfo& info, const wnGameWatchOilData* data) :
    wnWeaponBuilder<wnGameWatchOilModuleAccesserBuildConfig>(
        "GameWatchOil", articleId, info,
        wnModuleAccesserBuildData(121, data, info.heapModule->getInstanceHeap(), info.heapModule->getNWModelInstanceHeap(),
                                  info.heapModule->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param), m_animObj() {
    Weapon::deactivate(false);
}

void wnGameWatchOil::activate(int founderTaskId, int resourceId, int team, float color) {
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
#ifdef MATCHING
    __memcpy(&desc.pos, &pos, sizeof(Vec3f));
#else
    desc.pos = pos;
#endif
    desc.lr = 0.0f;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4D = (desc.unk4D & 0x3F) | 0x40;
    Weapon::activate(&desc);

    if (m_moduleAccesser->getModelModule().isRefFB())
        return;
    if (color == 13.0f)
        return;
    if (color == 12.0f)
        return;

    soResourceModule& resourceModule = m_moduleAccesser->getResourceModule();
    u32 mdlResId = resourceModule.getResourceIdAccesser()->getMdlResId();
    nw4r::g3d::ResFile resource(resourceModule.getFile(mdlResId, (ARCNodeType)4, 0));
    soHeapModule& heapModule = m_moduleAccesser->getHeapModule();
    nw4r::g3d::ResMdl modelResource = m_moduleAccesser->getModelModule().getScnMdl()->m_resMdl;
    nw4r::g3d::ResFile resourceCopy = resource;
    nw4r::g3d::ResMdl modelResourceCopy = modelResource;
    m_animObj.create(resourceCopy, modelResourceCopy, heapModule.getNWMotionInstanceHeap(), 0);
    m_animObj.set(m_moduleAccesser->getModelModule().getScnMdl());
    m_animObj.setFrame(color);
}

void wnGameWatchOil::deactivate(bool force) {
    m_animObj.remove();
    m_animObj.destroy();
    Weapon::deactivate(force);
}
