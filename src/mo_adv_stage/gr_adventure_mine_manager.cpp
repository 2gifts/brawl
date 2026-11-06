#define MT_VEC3F_IMPLICIT_ASSIGN
#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define YK_ATTACK_GROUPS_PER_PART // ... with one collision group for each of its parts
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <gf/gf_model.h>
#include <gr/gr_adventure_motion_ground.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <st/st_trigger.h>
#include <yk/yk_normal.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_adventure_mine.h>
#include <sora_adv_stage/gr_adventure_mine_manager.h>

// Matrix::copy (the map name); mt_matrix.cpp defines it by this stand-in name.
extern "C" void fn_8003E388(const Matrix* src, Matrix* dst);

grAdventureMineManager* grAdventureMineManager::create(int mdlIndex, const char* taskName) {
    grAdventureMineManager* ground = new (Heaps::StageInstance) grAdventureMineManager(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

void grAdventureMineManager::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grYakumono::startup(data, unk1, layerType);
    unk158 = static_cast<grGimmickMineData*>(m_gimmickData);
    unk1AC = static_cast<grGimmickMineData*>(m_gimmickData);
    unk1AC->m_teamId = 0x12;
    nw4r::g3d::ResMdl resMdl = m_sceneModels[0]->m_resMdl;
    unk1A8 = resMdl.GetResNodeNumEntries() - 1;
    for (u32 i = 0; (u8)i < unk1A8; i++) {
        grAdventureMine* mine = grAdventureMine::create(static_cast<s16>(unk1AC->m_mineMdlIndex), "");
        mine->m_connectedTask = this;
        if (m_attachedTask == NULL) {
            m_attachedTask = mine;
        } else {
            gfTask* last = m_attachedTask;
            while (last->m_nextTask != NULL) {
                last = last->m_nextTask;
            }
            last->m_nextTask = mine;
        }
        mine->setGimmickData(unk1AC);
        mine->unk14C = i + 1;
        mine->startup(data, 0, static_cast<gfSceneRoot::LayerType>(0));
        u8 idx = i;
        unk174[idx] = mine;
        grMineHitData* hit = &m_hits[(u8)i];
        hit->m_startOffsetPos.m_x = unk1AC->m_hitStartOffset.m_x;
        hit->m_startOffsetPos.m_y = unk1AC->m_hitStartOffset.m_y;
        hit->m_startOffsetPos.m_z = unk1AC->m_hitStartOffset.m_z;
        hit->m_endOffsetPos.m_x = unk1AC->m_hitEndOffset.m_x;
        hit->m_endOffsetPos.m_y = unk1AC->m_hitEndOffset.m_y;
        hit->m_endOffsetPos.m_z = unk1AC->m_hitEndOffset.m_z;
        hit->m_size = unk1AC->m_hitSize;
        hit->m_nodeIndex = idx + 1;
        hit->m_part = 0;
        hit->m_height = 1;
        hit->m_0x1c_3 = 0;
        hit->m_shapeType = unk1AC->m_hitShape != 0;
        m_hitSets[(u8)i].m_elements = hit;
        m_hitSets[(u8)i].m_size = 1;
        m_dataGroups[idx].m_hitDataSimpleSet = NULL;
        m_dataGroups[idx].m_hitDataSet = reinterpret_cast<soSet<soCollisionHitData>*>(&m_hitSets[idx]);
        m_dataGroups[idx].m_hitGroupIndex = idx;
        Matrix copied(true);
        Matrix matrix(true);
        m_sceneModels[0]->GetScnMtxPos(&matrix, nw4r::g3d::ScnObj::MTX_WORLD, idx + 1);
        fn_8003E388(&matrix, &copied);
        unk174[idx]->setMatrix(&copied);
        unk174[idx]->updateG3dProcCalcWorld();
        unk174[idx]->unk164 = unk1AC->m_mineParam;
    }
    m_ykData.m_dataGroupNum = unk1A8;
    m_ykData.m_dataGroups = m_dataGroups;

    ykInitInfo info = { 0, 0, 0, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    info.m_unk8 = unk1AC->m_teamId;
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = &m_ykData;
    typedef ykNormal<soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 13, 0, soCollisionAttackModuleImpl, 1, false, true>,
                     soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 13, 13, soCollisionHitModuleImpl, 0x3FF, true> >
        Config;
    Config* yakumono = new (Heaps::StageInstance) Config(&info);
    yakumono->postInitialize();
    yakumono->activate(&pos, -1.0f, 0.0f);
    setYakumono(yakumono);
    grGimmick::AttackDetails details;
    details.m_targetCategory = static_cast<soCollision::CategoryMask>(0x41);
    details.m_unk1 = 0;
    details.m_targetSituationODD = false;
    details.m_lrCheck = static_cast<soCollisionAttackData::LrCheck>(5);
    details.m_hitStopFrame = 0.0f;
    for (int i = 0; i < unk1A8; i++) {
        grMineAttackData attack = unk1AC->m_attackData;
        attack.m_size = unk1AC->m_hitSize;
        setAttackGimmick(i, i, i + 1, &attack, &details);
    }
    if (m_modelAnims != NULL) {
        m_modelAnims[0]->unbindNodeAnim(m_sceneModels[0]);
    }
    if (unk1AC->m_trigger.m_triggerId != 0) {
        stTrigger* trigger = g_stTriggerMng->createTrigger((Gimmick::AreaKind)0x32, &unk1AC->m_trigger);
        stActTriggerMotionBind* bind = new (Heaps::StageInstance) stActTriggerMotionBind(this, m_motionRatio);
        stActTriggerMotionStop* stop = new (Heaps::StageInstance) stActTriggerMotionStop(this, m_motionRatio);
        if (unk1AC->m_trigger.m_isValid) {
            trigger->setValidEdgeCallback(bind);
            trigger->setUnValidEdgeCallback(stop);
        } else {
            trigger->setValidEdgeCallback(stop);
            trigger->setUnValidEdgeCallback(bind);
            grGimmick::changeNodeAnim(0, 0);
        }
    } else {
        changeNodeAnim(0, 0);
    }
}

void grAdventureMineManager::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    void (grAdventureMineManager::*flows[4])(const int&, float) = {
        &grAdventureMineManager::frowStandby,
        &grAdventureMineManager::frowExplosion,
        &grAdventureMineManager::frowExploding,
        &grAdventureMineManager::frowEnding
    };
    for (u8 i = 0; i < unk1A8; i++) {
        unk174[i]->m_isUpdate = true;
        if (unk174[i]->unk15C == 0) {
            Matrix copied(true);
            Matrix matrix(true);
            m_sceneModels[0]->GetScnMtxPos(&matrix, nw4r::g3d::ScnObj::MTX_WORLD, i + 1);
            fn_8003E388(&matrix, &copied);
            unk174[static_cast<int>(i)]->setMatrix(&copied);
        }
        (this->*flows[unk174[i]->unk15C])(i, deltaFrame);
    }
    chSympatheticDetonation();
    chWarning();
    if (unk170) {
        unk170 = false;
        clearAttackerInfo();
    }
}

// HYPOTHESIS: a mine that is exploding sets off every other mine closer than the two reaches together (the distance math is
// not matched yet).
void grAdventureMineManager::chSympatheticDetonation() {
    for (int i = 0; i < unk1A8; i++) {
        if (unk174[i]->unk15C == 2) {
            Matrix matrixA(true);
            unk174[i]->getMatrix(&matrixA);
            Vec3f posA = matrixA.getTranslation();
            float reachA = unk1AC->m_attackData.m_size;
            float reachB = unk1AC->m_hitSize;
            for (int j = 0; j < unk1A8; j++) {
                if (i != j) {
                    Matrix matrixB(true);
                    unk174[j]->getMatrix(&matrixB);
                    Vec3f posB = matrixB.getTranslation();
                    Vec3f diff = posB - posA;
                    if (diff.length() < reachA + reachB) {
                        if (unk174[j]->unk15C == 0) {
                            unk174[j]->unk15C = 1;
                        }
                    }
                }
            }
        }
    }
}

void grAdventureMineManager::onInflict(soCollisionLog* collisionLog, u32 unk2, float power) {
    grAdventureMine* mine = unk174[collisionLog->m_damageIndex];
    if (mine->unk15C == 0) {
        mine->unk15C = 1;
    }
}

// HYPOTHESIS: Yakumono::setAttackSize and setAttackTargetCategory of grYakumono (sora_melee gr_yakumono.o, unnamed); the
// shared header declares setAttackSize with one more parameter than the call passes.
extern "C" void fn_27_267914(grYakumono* self, int index, float size);
extern "C" void fn_27_26791C(grYakumono* self, int index, int categoryMask);

void grAdventureMineManager::frowStandby(const int& index, float deltaFrame) { }

void grAdventureMineManager::frowEnding(const int& index, float deltaFrame) { }

void grAdventureMineManager::dbDispInvalidatedByCameraClippingSphere() {
    grGimmick::dbDispInvalidatedByCameraClippingSphere();
}

void grAdventureMineManager::frowExplosion(const int& index, float deltaFrame) {
    unk174[index]->setVisibility(0);
    fn_27_267914(this, index, unk1AC->m_attackData.m_size);
    fn_27_26791C(this, index, 0x3FF);
    grYakumono::setTeamYakumono(0x11, -1);
    unk174[index]->unk15C = 2;
    unk174[index]->unk168 = 20.0f;
    unk174[index]->callBreak();
}

void grAdventureMineManager::frowExploding(const int& index, float deltaFrame) {
    float& time = unk174[index]->unk168;
    time -= deltaFrame;
    if (time < 0.0f) {
        unk174[index]->unk15C = 3;
    }
    if (time < 16.0f) {
        grYakumono::disableAttack(index);
        grYakumono::disableHit(index, 0);
        grYakumono::setTeamYakumono(0x12, -1);
    }
}
