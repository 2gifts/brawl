#define MT_VEC3F_ASSIGN_NOINLINE
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC2F_CTOR_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <types.h>
#include <ft/poke/ft_poke_lizardon_article_pools.h>
#include <wn/poke/wn_poke_lizardon_rock_stone.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <ac/ac_anim_cmd_impl.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

wnPokeLizardonRockStone::wnPokeLizardonRockStone(s32 articleId, const wnArticleBuildInfo& info, const wnPokeLizardonRockStoneData* data) :
    wnWeaponBuilder<wnPokeLizardonRockStoneModuleAccesserBuildConfig>(
        "PokeLizardonRockStone", articleId, info,
        wnModuleAccesserBuildData(0xF, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &g_soParamAccesserNull,
                                  0, 0, 0, 0, 0, 0, 0)) {
    Weapon::deactivate(false);
}

void wnPokeLizardonRockStone::updateNodeSRT() {
    Vec3f scale;
    Vec3f rotation;
    Vec3f nodeRotation;
    Vec3f position;
    scale.m_x = scale.m_y = scale.m_z = getModuleAccesser()->getPostureModule().getScale();
    position = getModuleAccesser()->getPostureModule().getPos();
    rotation = getModuleAccesser()->getPostureModule().getRot(0);
    rotation.m_y += getModuleAccesser()->getPostureModule().getRotYLr();
    getModuleAccesser()->getModelModule().setNodeSRT(0, &scale, &rotation, &position);
    nodeRotation.m_x = getModuleAccesser()->getWorkManageModule().getFloat(0x11000001);
    nodeRotation.m_y = 0.0f;
    nodeRotation.m_z = 0.0f;
    getModuleAccesser()->getModelModule().setNodeRotate(1, &nodeRotation);
}

bool wnPokeLizardonRockStone::notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* acc, int index) {
    if (Weapon::notifyEventAnimCmd(cmd, acc, index)) return true;
    s8 group = cmd->getGroup();
    if (!isObserv(group)) return false;
    if (cmd->getType() <= -1 || cmd->getType() >= 11) return false;
    switch (cmd->getType()) {
    case 6:
        getModuleAccesser()->getStatusModule().changeStatusForce(1, getModuleAccesser());
        return true;
    }
    return false;
}
