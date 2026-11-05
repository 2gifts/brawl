#define WN_WEAPON_NO_COMMON_ENERGIES
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define WN_KINETIC_GENERIC_BUILDER
#define WN_KINETIC_NORMAL_HOLDER_DEFAULT
#define WN_KINETIC_NORMAL_EXTERN
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/popo/wn_popo_ice_berg.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_value_accesser.h>
#include <st/stage.h>
#include <so/so_slow.h>

// MATCH-ONLY: the unit's rodata starts with this word (HYPOTHESIS: the remains of a constant of an unreconstructed function).
#pragma force_active on
extern const int sIceBergRodataLead = 3;
#pragma force_active reset

wnPopoIceBerg::wnPopoIceBerg(s32 articleId, const wnArticleBuildInfo& info, const wnPopoIceBergData* data) :
    wnWeaponBuilder<wnPopoIceBergModuleAccesserBuildConfig>(
        "PopoIceBerg", articleId, info,
        wnModuleAccesserBuildData(0x82, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, data->unk40, 0, 0, 0, 0)),
    soDamageEventObserver(ftGetManageId(m_moduleAccesser), -1), m_param(data->m_param), m_paramAccesser(data->m_param) {
    m_moduleAccesser->getCollisionHitModule().add(reinterpret_cast<soSet<soCollisionHitData>*>(const_cast<u8*>(data->unk38)), 0);
    Weapon::deactivate(false);
}

void wnPopoIceBerg::onDeactivate() {
    wnPopoIceBergLinkEvent event(0x45c);
    m_moduleAccesser->getLinkModule().sendEventParents(3, event);
}

void wnPopoIceBerg::activate(int founderTaskId, int resourceId, int team, const Vec3f* position, float lr, int heap) {
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0,
                        Vec3f(0.0f, 0.0f, 0.0f), lr, team, 0, 2, 0x80, 0, 0x35f, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.unk4DHigh = 0;
    desc.flagsLowNibble = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getModelModule().setScaleZ(1.0f);
    m_moduleAccesser->getEffectModule().ignoreScaleZ(true);
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000000);
    float offset = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfa2, 0);
    Stage* stage = g_Stage;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    if (stage && stage->m_stageParam) {
        scaleX = stage->m_stageParam->m_iceClimbersFinalScale.m_x;
        scaleY = stage->m_stageParam->m_iceClimbersFinalScale.m_y;
    }
    offset *= scaleY;
    Vec3f pos(*position);
    pos.m_y -= offset;
    m_moduleAccesser->getPostureModule().setPos(&pos);
    m_moduleAccesser->getWorkManageModule().setFloat(scaleX, 0x11000001);
    m_moduleAccesser->getWorkManageModule().setFloat(scaleY, 0x11000002);
    m_moduleAccesser->getPostureModule().setScale(scaleX);
    m_moduleAccesser->getWorkManageModule().setFloat(position->m_x, 0x11000003);
    m_moduleAccesser->getWorkManageModule().setFloat(position->m_y, 0x11000004);
    m_moduleAccesser->getGroundModule().constructGrCollision(resourceId, 0, static_cast<Heaps::HeapType>(heap), 0, 0, 0);
    m_moduleAccesser->getGroundModule().setGrCollisionFlagAll(1, 2);
    // HYPOTHESIS: default compact stage attack parameters.
    int attackData[6] = { 0, 0, 0, 0, 2, -1 };
    attackData[0] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc4, 0);
    attackData[1] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc5, 0);
    attackData[2] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc6, 0);
    attackData[3] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc7, 0);
    attackData[5] = team;
    g_Stage->setStageAttackData(attackData);
    m_moduleAccesser->getGroundModule().setGrCollisionMode(true);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    updateNodeSRT();
    Vec3f top = m_moduleAccesser->getModelModule().getNodeGlobalPosition(9, false);
    Vec3f bottom = m_moduleAccesser->getModelModule().getNodeGlobalPosition(7, false);
    m_moduleAccesser->getGenerateArticleManageModule().removeExist(4, 0);
    soArticle* article = m_moduleAccesser->getLinkModule().generateArticleLinkParents(3, 4, m_moduleAccesser);
    m_moduleAccesser->getGenerateArticleManageModule().entry(article);
}

void wnPopoIceBerg::processFixPosition() {
    Weapon::processFixPosition();
    if (soSlow::getInstance()->isAdjust() == true && !m_moduleAccesser->getStopModule().isStop()) {
        if (m_moduleAccesser->getWorkManageModule().isFlag(0x12000003) ||
            m_moduleAccesser->getWorkManageModule().isFlag(0x12000004)) {
            // HYPOTHESIS: default compact stage attack parameters.
            int attackData[6] = { 0, 0, 0, 0, 2, -1 };
            attackData[5] = m_moduleAccesser->getTeamModule().getTeam()->getNo();
            if (m_moduleAccesser->getWorkManageModule().isFlag(0x12000004)) {
                m_moduleAccesser->getWorkManageModule().offFlag(0x12000004);
                attackData[0] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc8, 0);
                attackData[1] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc9, 0);
                attackData[2] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dca, 0);
                attackData[3] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dcb, 0);
            } else {
                m_moduleAccesser->getWorkManageModule().offFlag(0x12000003);
                attackData[0] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc4, 0);
                attackData[1] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc5, 0);
                attackData[2] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc6, 0);
                attackData[3] = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc7, 0);
            }
            g_Stage->setStageAttackData(attackData);
        } else if (m_moduleAccesser->getWorkManageModule().isFlag(0x12000006) == true) {
            m_moduleAccesser->getWorkManageModule().offFlag(0x12000006);
            float scale = m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
            m_moduleAccesser->getWorkManageModule().getFloat(0x11000002);
            Vec3f origin = m_moduleAccesser->getModelModule().getNodeGlobalPosition(9, false);
            Vec3f upper = m_moduleAccesser->getModelModule().getNodeGlobalPosition(8, false) - origin;
            Vec3f lower = m_moduleAccesser->getModelModule().getNodeGlobalPosition(5, false) - origin;
            if (scale > 0.0f && scale != 1.0f) {
                upper.m_x /= scale;
                upper.m_y /= scale;
                lower.m_x /= scale;
                lower.m_y /= scale;
            }
            m_moduleAccesser->getCollisionAttackModule().setOffset(1, &upper);
            m_moduleAccesser->getCollisionAttackModule().setOffset(3, &upper);
            m_moduleAccesser->getCollisionAttackModule().setOffset(0, &lower);
            m_moduleAccesser->getCollisionAttackModule().setOffset(2, &lower);
        }
        if (m_moduleAccesser->getWorkManageModule().isFlag(0x12000005) == true) {
            m_moduleAccesser->getWorkManageModule().offFlag(0x12000005);
            syncCapsuleSRT(4);
            syncCapsuleSRT(5);
            syncCapsuleSRT(6);
        }
    }
}

bool wnPopoIceBerg::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

void wnPopoIceBerg::syncCapsuleSRT(u32 index) {
    if (m_moduleAccesser->getCollisionAttackModule().isAttack(index, false) &&
        m_moduleAccesser->getCollisionAttackModule().getData(index, false) != 0) {
        m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
        m_moduleAccesser->getWorkManageModule().getFloat(0x11000002);
    }
}

void wnPopoIceBergKineticTransactor::changeKinetic(int kineticType, wnPopoIceBergKineticPools* pools, soModuleAccesser* acc) {
    if (kineticType <= 0x1f) {
        wnKineticTransactor::changeKinetic(kineticType, pools, acc);
        return;
    }
    switch (kineticType) {
    case 32:
        changeKineticSub(soIntToType<32>(), pools, acc);
        break;
    case 33:
        changeKineticSub(soIntToType<33>(), pools, acc);
        break;
    }
}

void wnPopoIceBergKineticTransactor::changeKineticSub(const soIntToType<32>&, wnPopoIceBergKineticPools* pools, soModuleAccesser* acc) {
    acc->getKineticModule().unableEnergyAll();
    float targetY = acc->getWorkManageModule().getFloat(0x11000004);
    float distance = targetY - acc->getPostureModule().getPos().m_y;
    float frames = soValueAccesser::getConstantInt(acc, 0x5dc0, 0);
    soKineticEnergyNormal* energy = pools->getEnergy(soIntToType<0>());
    *energy = soKineticEnergyNormal(Vec2f(0.0f, 0.0f), Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f),
                                  Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f), false, false);
    energy->setSpeed(&Vec2f(0.0f, distance / frames));
    energy->enable();
}

void wnPopoIceBergKineticTransactor::changeKineticSub(const soIntToType<33>&, wnPopoIceBergKineticPools* pools, soModuleAccesser* acc) {
    acc->getKineticModule().unableEnergyAll();
    float targetY = acc->getWorkManageModule().getFloat(0x11000004);
    float offset = soValueAccesser::getConstantFloat(acc, 0xfa2, 0);
    offset *= acc->getWorkManageModule().getFloat(0x11000002);
    float distance = targetY - offset - acc->getPostureModule().getPos().m_y;
    float frames = soValueAccesser::getConstantInt(acc, 0x5dc1, 0);
    soKineticEnergyNormal* energy = pools->getEnergy(soIntToType<0>());
    *energy = soKineticEnergyNormal(Vec2f(0.0f, 0.0f), Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f),
                                  Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f), false, false);
    energy->setSpeed(&Vec2f(0.0f, distance / frames));
    energy->enable();
}
