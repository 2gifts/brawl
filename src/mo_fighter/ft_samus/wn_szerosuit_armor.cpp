#define IP_INPUT_BUTTON_RAW
#define SO_BUILDER_HELPERS_WEAK
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_TRANSITION_CTOR_INLINE
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define WN_KINETIC_GENERIC_BUILDER
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#include <types.h>
#include <wn/wn_module_accesser_builder.h>
#include <wn/samus/wn_szerosuit_armor.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_kinetic_energy_rot_normal.h>
#include <ft/ft_common_data_accesser.h>
#include <it/it_manager.h>
#include <it/item.h>
#include <math.h>

// The item generation sheets of the Power Suit piece item. HYPOTHESIS: names (the same pair as the enemies' units).
static itGenSheetKind s_genSheetKindLevel0(Stages::srStageKind(0xff), 0);
static itGenSheetKind s_genSheetKindLevel1(Stages::srStageKind(0xff), 1);

wnSZerosuitArmor::wnSZerosuitArmor(s32 articleId, const wnArticleBuildInfo& info, const wnSZerosuitArmorData* data) :
    wnWeaponBuilder<wnSZerosuitArmorModuleAccesserBuildConfig>(
        "SZerosuitArmor", articleId, info,
        wnModuleAccesserBuildData(0x42, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

// The piece is attached to a node of the fighter model until it is shot.
void wnSZerosuitArmor::activate(int founderTaskId, u32 etcResId, int index, int mdlResIndex, u32 node, u32 targetNode, int team) {
    wnActivateDesc desc(founderTaskId, etcResId, etcResId, 0xFFFF, -1, -1, index, 0, Vec3f(0.0f, 0.0f, 0.0f), 0.0f, team, 0, 2,
                        0x80, 0, 0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    desc.unkC = mdlResIndex;
    Weapon::activate(&desc);
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    work.setInt(etcResId, 0x20000000);
    work.setInt(index, 0x20000001);
    work.setInt(targetNode, 0x20000002);
    m_moduleAccesser->getLinkModule().setModelConstraintPosOrt(3, targetNode, node, 3, true);
    m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_None, 0);
}

// The piece leaves the fighter and becomes a Power Suit piece item with the given speed and angle (in degrees).
void wnSZerosuitArmor::shoot(float speed, float angle, float lr, int node) {
    soWorkManageModule& work = getModuleAccesser()->getWorkManageModule();
    soPostureModule& posture = getModuleAccesser()->getPostureModule();
    soKineticModule& kinetic = getModuleAccesser()->getKineticModule();
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*kinetic.getEnergy(0));
    dynamic_cast<soKineticEnergyRotNormal&>(*kinetic.getEnergy(1));
    int parentTaskId = getModuleAccesser()->getLinkModule().getParentTaskId(3);
    getModuleAccesser()->getLinkModule().adjustModelConstraintPosture(&Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    getModuleAccesser()->getLinkModule().removeModelConstraint(true);
    updateNodeSRT();
    Vec3f pos = posture.getPos();
    Vec3fSub(&pos, &pos, &getModuleAccesser()->getModelModule().getNodeGlobalOffsetFromTop(node));
    posture.setPos(&pos);
    posture.setLr(lr);
    const wnSZerosuitArmorData* data = static_cast<const wnSZerosuitArmorData*>(g_ftCommonDataAccesser.getSZerosuitArmorData());
    Vec2f velocity;
    velocity.m_x = lr * (speed * cosf(0.017453292f * angle));
    velocity.m_y = speed * sinf(0.017453292f * angle);
    Vec2f limit = energy.m_speedLimit;
    limit.m_x = 10.0f * velocity.length();
    limit.m_y = 10.0f * velocity.length();
    energy.m_speed = velocity;
    energy.m_accel = Vec2f(0.0f, -0.05f);
    energy.m_speedTarget = limit;
    energy.m_speedLimit = limit;
    updateNodeSRT();
    int etcResId = work.getInt(0x20000000);
    int index = work.getInt(0x20000001);
    if (index == 2 || index == 6 || index == 7) {
        u32 variation = 0;
        if (index == 2) {
            variation = 0;
        } else if (index == 6) {
            variation = 1;
        } else if (index == 7) {
            variation = 2;
        }
        BaseItem* item = itManager::getInstance()->createItem(
            &pos, lr, static_cast<itKind>(0x5a), variation, parentTaskId, &getModuleAccesser()->getResourceModule(), data->m_resGroupNo,
            etcResId, getModuleAccesser()->getResourceModule().getResourceIdAccesser()->getTexResIndex(),
            getModuleAccesser()->getResourceModule().getResourceIdAccesser()->getTexResId());
        if (item != NULL) {
            item->setOwnerScale(getModuleAccesser()->getPostureModule().getScale());
            getModuleAccesser()->getVisibilityModule().setWhole(0);
        }
    }
    getModuleAccesser()->getStatusModule().changeStatusRequest(0, getModuleAccesser());
}
