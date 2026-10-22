#include <gr/gr_adventure_motion_ground.h>
#include <gf/gf_model.h>
#include <sora_adv_stage/gr_adventure_common.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <st/st_trigger.h>

// HYPOTHESIS: stTrigger::setValidEdgeCallback and setUnValidEdgeCallback (sora_melee map names, still unnamed).
extern "C" void fn_27_233F58(stTrigger* trigger, gfCallBack* cb);
extern "C" void fn_27_233FCC(stTrigger* trigger, gfCallBack* cb);
// HYPOTHESIS: grVisibleProductionMaterialMotion::create (sora_melee map name, still unnamed) and its parameter types.
extern "C" grVisibleProduction* fn_27_2714A4(nw4r::g3d::ScnMdl* sceneModel, gfModelAnimation* modelAnim, u32* animIndex);
// HYPOTHESIS: the "effect_locator" node name that the grGimmick effect set-up shares (sora_melee .data).
extern "C" const char lbl_27_data_56D70[];

// HYPOTHESIS: the start indices into the two visible-production arrays for the sound and effect productions.
// MATCH-ONLY: the zero-initialized ones live in .data in the original, which pools all four.
#pragma section ".data" ".data"
__declspec(section ".data") static u32 sSEProductionIndex0 = 0;
__declspec(section ".data") static u32 sSEProductionIndex1 = 0;
static u32 sEffectProductionIndex0 = 4;
static u32 sEffectProductionIndex1 = 4;

grAdventureMotionGround* grAdventureMotionGround::create(int mdlIndex, const char* taskName) {
    grAdventureMotionGround* ground = new (Heaps::StageInstance) grAdventureMotionGround(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureMotionGround::grAdventureMotionGround(const char* taskName) : grYakumono(taskName) {
    m_data = NULL;
    m_transparencySettings = 0xE;
}

grAdventureMotionGround::~grAdventureMotionGround() { }

inline void setSoundEffectInfo(grGimmick* gimmick, u32 index, grAdventureMotionGroundSEData* data, Vec2f offset) {
    if (data->m_id > 0) {
        gimmick->m_soundEffects[index].m_id = data->m_id;
        gimmick->m_soundEffects[index].m_repeatFrame = data->m_repeatFrame;
        gimmick->m_soundEffects[index].m_nodeIndex = (u32)data->m_nodeIndex;
        gimmick->m_soundEffects[index].m_endFrame = data->m_endFrame;
        gimmick->m_soundEffects[index].m_generatorIndex = 0;
        gimmick->m_soundEffects[index].m_offsetPos = offset;
    }
}

inline void setEffectInfo(grGimmick* gimmick, u32 index, grAdventureMotionGroundSEData* data, Vec2f offset, float scale) {
    if (data->m_id > 0) {
        gimmick->m_effects[index].m_id = data->m_id;
        gimmick->m_effects[index].m_repeatFrame = data->m_repeatFrame;
        if (data->m_nodeIndex == 0) {
            gimmick->m_effects[index].m_nodeIndex = gimmick->getNodeIndex(0, lbl_27_data_56D70);
        } else {
            gimmick->m_effects[index].m_nodeIndex = data->m_nodeIndex;
        }
        gimmick->m_effects[index].m_endFrame = data->m_endFrame;
        gimmick->m_effects[index].m_offsetPos = offset;
        gimmick->m_effects[index].m_scale = scale;
    }
}

inline void setEndEffectInfo(grGimmick* gimmick, u32 index, int id, Vec2f offset, float scale) {
    if (id > 0) {
        gimmick->m_effects[index].m_id = id;
        gimmick->m_effects[index].m_repeatFrame = 0;
        gimmick->m_effects[index].m_nodeIndex = gimmick->getNodeIndex(0, "effect_locator");
        gimmick->m_effects[index].m_endFrame = 0;
        gimmick->m_effects[index].m_offsetPos = offset;
        gimmick->m_effects[index].m_scale = scale;
    }
}

void grAdventureMotionGround::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grGimmick::startup(data, unk1, layerType);
    m_data = static_cast<grGimmickMotionGroundData*>(m_gimmickData);
    m_motionRatio = m_data->m_motionRatios[advGetDifficultyLevel()];
    if (m_modelAnims != NULL) {
        m_modelAnims[0]->unbindNodeAnim(m_sceneModels[0]);
    }
    createSoundWork(11, 1);
    createEffectWork(11);
    for (u32 i = 0; i < 3; i++) {
        if (m_data->m_defaultSE[i].m_id > 0) {
            setSoundEffectInfo(this, i, &m_data->m_defaultSE[i], Vec2f(0.0f, 0.0f));
        }
        if (m_data->m_endSE[i].m_id > 0) {
            setSoundEffectInfo(this, i + 8, &m_data->m_endSE[i], Vec2f(0.0f, 0.0f));
        }
        if (m_data->m_defaultEffect[i].m_id > 0) {
            setEffectInfo(this, i, &m_data->m_defaultEffect[i], Vec2f(0.0f, 0.0f), 1.0f);
        }
        if (m_data->m_endEffect[i].m_id > 0) {
            setEndEffectInfo(this, i + 8, m_data->m_endEffect[i].m_id, Vec2f(0.0f, 0.0f), 1.0f);
        }
    }
    for (u32 i = 0; i < 5; i++) {
        if (m_data->m_moveSE[i].m_id > 0) {
            setSoundEffectInfo(this, i + 3, &m_data->m_moveSE[i], Vec2f(0.0f, 0.0f));
        }
        if (m_data->m_moveEffect[i].m_id > 0) {
            setEffectInfo(this, i + 3, &m_data->m_moveEffect[i], Vec2f(0.0f, 0.0f), 1.0f);
        }
    }
    defaultSE();
    defaultEffect();
    if (m_data->m_motionTrigger.m_triggerId != 0) {
        stTrigger* trigger = g_stTriggerMng->createTrigger((Gimmick::AreaKind)0x33, &m_data->m_motionTrigger);
        stActTriggerMotionBind* bind = new (Heaps::StageInstance) stActTriggerMotionBind(this, m_motionRatio);
        stActTriggerMotionStop* stop = new (Heaps::StageInstance) stActTriggerMotionStop(this, m_motionRatio);
        if (m_data->m_motionTrigger.m_isValid) {
            fn_27_233F58(trigger, bind);
            fn_27_233FCC(trigger, stop);
        } else {
            fn_27_233F58(trigger, stop);
            fn_27_233FCC(trigger, bind);
            changeNodeAnim(0, 0);
            changeTexSrtAnim(0, 0);
        }
    } else {
        changeNodeAnim(0, 0);
        changeTexSrtAnim(0, 0);
    }
    makeCalcuCallback(1, Heaps::StageInstance);
    setCalcuCallbackRoot(0);
    createIsValidTrigger(&m_data->m_isValidTrigger);
    // HYPOTHESIS: BrawlHeaders declares 4 entries of the two 10-entry production arrays at 0xFC and 0x124.
    createSoundEffectVisibleProductionForExcel((u32)m_data->m_seProduction0, (u32)&sSEProductionIndex0, (u32)&m_visibleProductions[0]);
    createSoundEffectVisibleProductionForExcel((u32)m_data->m_seProduction1, (u32)&sSEProductionIndex1, (u32)&m_visibleProductions[10]);
    createEffectVisibleProductionForExcel(m_data->m_effectProduction0, &sEffectProductionIndex0, &m_visibleProductions[0]);
    createEffectVisibleProductionForExcel(m_data->m_effectProduction1, &sEffectProductionIndex1, &m_visibleProductions[10]);
    if (m_data->m_productionFlags & 1) {
        u32 animIndex = m_data->m_matAnimIndex;
        m_visibleProductions[9] = fn_27_2714A4(m_sceneModels[0], m_modelAnims[0], &animIndex);
        u32 animIndex2 = m_data->m_matAnimIndex;
        m_visibleProductions[19] = fn_27_2714A4(m_sceneModels[0], m_modelAnims[0], &animIndex2);
        if (m_data->m_matAnimIndex == 0) {
            m_modelAnims[0]->unbindMatColAnim(m_sceneModels[0]);
        }
    }
    if (m_data->m_productionFlags & 2) {
        createFadeVisibleProduction(m_data->m_matAnimIndex);
    }
}

void grAdventureMotionGround::setInitializeFlag() {
    if (!g_stTriggerMng->getTriggerFlag(&m_data->m_isValidTrigger, true)) {
        if (m_data->m_isValidTrigger.m_isValid) {
            setValid(0);
        }
    } else {
        if (!m_data->m_isValidTrigger.m_isValid) {
            setValid(0);
        }
    }
    if (g_stTriggerMng->getTriggerFlag(&m_data->m_motionTrigger, false) == true) {
        grGimmick::changeNodeAnim(0, 0);
        m_modelAnims[0]->m_anmObjChrRes->SetFrame(m_modelAnims[0]->m_anmObjChrRes->m_anmChrFile.ptr()->m_animLength);
    }
}

void grAdventureMotionGround::update(float deltaFrame) {
    Vec3f pos;
    getNodePosition(&pos, 0, (u32)0);
    setPos(&pos);
    updateSEandEffect();
    grGimmick::update(deltaFrame);
    grGimmick::updateCallback(0);
}

// HYPOTHESIS: the animation policy word of the CHR0 data (1: loop); the BrawlHeaders layout stops before it.
static inline s32 getAnmPolicy(nw4r::g3d::ResAnmChr anim) {
    return reinterpret_cast<s32*>(anim.ptr())[8];
}

void grAdventureMotionGround::updateSEandEffect() {
    nw4r::g3d::AnmObjChrRes* anmChr = m_modelAnims[0]->m_anmObjChrRes;
    if (anmChr != NULL && anmChr->m_anmChrFile.ptr()->m_animLength > 1) {
        float frame = anmChr->GetFrame();
        u16 animLength = m_modelAnims[0]->m_anmObjChrRes->m_anmChrFile.ptr()->m_animLength;
        if (m_modelAnims[0]->getUpdateRate() > 0.0f && frame < m_motionRatio) {
            startMoveSE();
            startMoveEffect();
            if (getAnmPolicy(m_modelAnims[0]->m_resFile.GetResAnmChr((u32)0)) == 1) {
                setMotionAttack();
            }
        }
        if (frame + m_motionRatio >= animLength && frame < animLength) {
            if (getAnmPolicy(m_modelAnims[0]->m_resFile.GetResAnmChr((u32)0)) == 1) {
                m_modelAnims[0]->m_anmObjChrRes->SetFrame(frame - animLength);
                disableAttack(0);
            }
        }
    }
}

void grAdventureMotionGround::setMotionAttack() { }

void grAdventureMotionGround::changeNodeAnim(u32 chrIndex, u32 modelAnimIndex) {
    grGimmick::changeNodeAnim(chrIndex, modelAnimIndex);
    startMoveSE();
    startMoveEffect();
}

void grAdventureMotionGround::defaultSE() {
    startGimmickSE(0);
    startGimmickSE(1);
    startGimmickSE(2);
}

void grAdventureMotionGround::defaultEffect() {
    startGimmickEffect(0);
    startGimmickEffect(1);
    startGimmickEffect(2);
}

void grAdventureMotionGround::startMoveSE() {
    for (u32 i = 0; i < 5; i++) {
        stopGimmickSE(i + 3);
    }
    for (int i = 0; i < 5; i++) {
        startGimmickSE(i + 3);
    }
}

void grAdventureMotionGround::startMoveEffect() {
    for (u32 i = 0; i < 5; i++) {
        stopGimmickEffect(i + 3);
    }
    for (int i = 0; i < 5; i++) {
        startGimmickEffect(i + 3);
    }
}

void grAdventureMotionGround::endMoveSE() {
    for (u32 i = 0; i < 3; i++) {
        stopGimmickSE(i + 8);
    }
    for (int i = 0; i < 3; i++) {
        startGimmickSE(i + 8);
    }
}

void grAdventureMotionGround::endMoveEffect() {
    for (u32 i = 0; i < 3; i++) {
        stopGimmickEffect(i + 8);
    }
    for (int i = 0; i < 3; i++) {
        startGimmickEffect(i + 8);
    }
}

void grAdventureMotionGround::setVisibility(u32 unk1) {
    Ground::setVisibility(unk1);
}

void grAdventureMotionGround::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    static u32 sOwnerFlags = 1;
    if (isCollisionStatusOwnerTask__6GroundFP12grCollStatusPi(this, collStatus, reinterpret_cast<int*>(&sOwnerFlags))) {
        if ((s8)getPlayerNumber(collStatus) == 0 && isFirstContact) {
            g_stTriggerMng->setTriggerFlag(&m_data->m_landingTrigger);
        }
    }
}

void grAdventureMotionGround::dbDispInvalidatedByCameraClippingSphere() {
    grGimmick::dbDispInvalidatedByCameraClippingSphere();
}
