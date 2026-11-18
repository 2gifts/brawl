#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define YK_CLTARGET_4_FIRST // the clTarget constructor writes the second member first in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <st_greenhill/gr_greenhill.h>
#include <st_greenhill/gr_greenhill_anim.h>
#include <gr/gr_calc_world_callback.h>
#include <ai/ai_mgr.h>
#include <ec/ec_mgr.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>
#include <yk/yk_normal.h>

grGreenhillCheck* grGreenhillCheck::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grGreenhillCheck* ground = new (Heaps::StageInstance) grGreenhillCheck(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grGreenhillCheck::grGreenhillCheck(const char* taskName) : grGreenhill(taskName) {
    m_timer2 = 0.0f;
    unk15C = NULL;
    unk160 = NULL;
    unk164 = NULL;
    m_order[0] = 4;
    m_order[1] = 4;
    m_order[2] = 4;
    m_order[3] = 4;
    m_orderDone = 0;
    m_hitTeam = 0;
    m_animId = 5;
    unk17C = 0.0f;
    m_motionEndFrame = 0.0f;
    unk184 = 0;
    m_attackEnabled = 0;
    unk188[0] = NULL;
    unk188[1] = NULL;
    unk188[2] = NULL;
    unk188[3] = NULL;
    unk188[4] = NULL;
    m_dangerZoneId = -1;
    m_effectId = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grGreenhillCheck::~grGreenhillCheck() {
    if (unk188[0] != NULL) {
        delete static_cast<u8*>(unk188[0]);
    }
    unk188[0] = NULL;
    if (unk188[1] != NULL) {
        delete static_cast<u8*>(unk188[1]);
    }
    unk188[1] = NULL;
    if (unk188[2] != NULL) {
        delete static_cast<u8*>(unk188[2]);
    }
    unk188[2] = NULL;
    if (unk188[3] != NULL) {
        delete static_cast<u8*>(unk188[3]);
    }
    unk188[3] = NULL;
    if (unk188[4] != NULL) {
        delete static_cast<u8*>(unk188[4]);
    }
    unk188[4] = NULL;
}

// Runs the three Check updates once the gimmick is running.
void grGreenhillCheck::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The hit model/yakumono is built the first time the ball needs it.
void grGreenhillCheck::updateYakumono(float deltaFrame) {
    if (unk184 != 1) {
        setHit();
        if (m_yakumono != NULL) {
            unk184 = 1;
        }
    }
}

// The four values the stage's Check parameter block holds (read through getStageData()).
struct GreenhillCheckParams {
    float unk0;     // timer set when the ball starts rolling
    float unk4;     // minimum wait between two drops
    float unk8;     // maximum wait between two drops
    float firstWait; // wait before the first drop
};

// State machine of the checkered ball. A marker is picked from a shuffled order of four positions, the ball waits,
// drops there, stays until its timer runs out and then rolls off; a fighter hit (state work 3) sends it rolling.
// State numbers are HYPOTHESIS; the transitions are read from the original jump table.
void grGreenhillCheck::updateActive(float deltaFrame) {
    GreenhillCheckParams* params = static_cast<GreenhillCheckParams*>(getStageData());
    if (params == NULL) {
        return;
    }
    m_timer -= deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    m_timer2 -= deltaFrame;
    if (m_timer2 < 0.0f) {
        m_timer2 = 0.0f;
    }

    switch (m_state) {
    case 0: {
        // Reset: hide the ball and pick the next marker.
        setMotion(4, false, true, NULL);
        setVisibility(0);
        disableHit(0, 0);
        disableAttack(0);
        m_attackEnabled = 0;
        if (m_order[1] != 4) {
            m_order[0] = m_order[1];
            m_order[1] = m_order[2];
            m_order[2] = m_order[3];
            m_order[3] = 4;
        } else {
            // The order is used up (or never built): shuffle the four markers, avoiding the one used last.
            u8 previousFirst = m_order[0];
            m_order[0] = 0;
            m_order[1] = 1;
            m_order[2] = 2;
            m_order[3] = 3;
            do {
                u32 count = 4;
                for (u8 i = 0; i < 4; i++) {
                    int value = (int)(count * randf());
                    int nonZero = ((u8)value != 0) ? value : 0;
                    u8 j = ((u8)nonZero >= 3) ? 3 : nonZero;
                    u8 swap = m_order[i];
                    m_order[i] = m_order[j];
                    m_order[j] = swap;
                }
            } while (m_order[0] == previousFirst);
        }
        m_orderDone = 0;
        if (m_yakumono != NULL) {
            m_yakumono->setTeam(16);
        }
        unk15C[0] = 5;
        m_timer = params->firstWait;
        m_state = 1;
        break;
    }
    case 1:
        // Wait for the drop; skip a marker whose breakable piece is already gone.
        if (0.0f == m_timer) {
            switch (m_order[0]) {
            case 1:
                if (unk160[1] == 2) {
                    return;
                }
                break;
            case 2:
                if (unk160[2] == 2) {
                    return;
                }
                break;
            }
            setMotion(0, false, true, &m_motionEndFrame);
            unk15C[0] = 0;
            m_sndGenerator.playSE(static_cast<SndID>(0x1d17), 0, 0, -1);
            m_sndGenerator.setPos(&unk164[m_order[0]]);
            m_state = 5;
        }
        break;
    case 5:
        if (getMotionFrame(0) >= m_motionEndFrame) {
            setMotion(4, false, true, &m_motionEndFrame);
            enableHit(0, 0);
            float random = randf();
            float span = params->unk8 - params->unk4;
            m_timer = params->unk4 + span * random;
            unk15C[0] = 4;
            m_state = 9;
        } else if (!m_isVisible) {
            setVisibility(1);
        }
        break;
    case 6:
        if (getMotionFrame(0) >= m_motionEndFrame) {
            m_state = 0;
        }
        break;
    case 7:
        if (getMotionFrame(0) >= m_motionEndFrame) {
            m_state = 0;
        }
        break;
    case 8: {
        Vec3f ballPos;
        getNodePosition(&ballPos, 0, "ball");
        // Danger zone for the AI: a box around the ball.
        Vec2f corners[2] = {Vec2f(ballPos.m_x - 20.0f, 20.0f + ballPos.m_y),
                            Vec2f(20.0f + ballPos.m_x, ballPos.m_y - 20.0f)};
        m_dangerZoneId = g_aiMgr->setDangerZone(&corners[0], &corners[1], m_dangerZoneId, false, false);
        if (0.0f == m_timer2) {
            if (getMotionFrame(0) < unk17C) {
                // The roll animation wrapped around: the ball is done.
                setMotion(4, false, true, &m_motionEndFrame);
                enableHit(0, 0);
                disableAttack(0);
                m_attackEnabled = 0;
                if (m_yakumono != NULL) {
                    m_yakumono->setTeam(16);
                }
                if (m_dangerZoneId != -1) {
                    g_aiMgr->delDangerZone(m_dangerZoneId);
                    m_dangerZoneId = -1;
                }
                changeColor(0);
                unk15C[0] = 4;
                m_state = 9;
            } else {
                unk17C = getMotionFrame(0);
            }
        } else {
            unk17C = getMotionFrame(0);
        }
        break;
    }
    case 9:
        if (0.0f == m_timer) {
            setMotion(1, false, true, &m_motionEndFrame);
            disableHit(0, 0);
            m_sndGenerator.playSE(static_cast<SndID>(0x1d18), 0, 0, -1);
            m_sndGenerator.setPos(&unk164[m_order[0]]);
            m_state = 6;
        } else {
            switch (unk15C[0]) {
            case 3:
                // MATCH-ONLY: the team number of the attacker lives in the collision log of the damage (a signed byte at 0x30).
struct CheckDamageView {
    u8 unk00[0x30];
    s8 m_teamNo;
};

// A fighter hit the ball: roll.
                setMotion(3, true, true, &m_motionEndFrame);
                unk17C = 0.0f;
                disableHit(0, 0);
                setAttack();
                m_effectId = g_ecMgr->setEffect(ef_ptc_stg_greenhill_marker);
                g_ecMgr->setParent(m_effectId, m_sceneModels[0], "ballPosition", false);
                changeColor(1);
                m_timer2 = params->unk0;
                m_state = 8;
                break;
            }
        }
        break;
    }

    // Common tail: when the marker's breakable piece has been broken the ball is removed.
    if (m_orderDone == 0) {
        u8 first = m_order[0];
        if ((first == 1 && unk160[1] == 2) || (first == 2 && unk160[2] == 2)) {
            setMotion(2, false, true, &m_motionEndFrame);
            disableHit(0, 0);
            if (m_attackEnabled == 1) {
                disableAttack(0);
                m_attackEnabled = 0;
            }
            if (g_ecMgr->isExistEffect(m_effectId) == 1) {
                g_ecMgr->endEffect(m_effectId);
                m_effectId = 0;
            }
            unk15C[0] = 2;
            m_orderDone = 1;
            if (m_dangerZoneId != -1) {
                g_aiMgr->delDangerZone(m_dangerZoneId);
                m_dangerZoneId = -1;
            }
            m_state = 7;
        }
    }
}

// The ball model is moved by a world-calc callback: it follows the marker position the random order currently selects.
void grGreenhillCheck::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            if (unk164 != NULL) {
                calcWorldCallBack->m_nodeCallbackDatas[0].m_pos = unk164[m_order[0]];
            }
        }
    }
}


// MATCH-ONLY: the hit sphere is copied to its simple form as plain words (the two offsets, the size, the shape byte).
struct CheckHitWords {
    u32 m_x, m_y, m_z;
};
struct CheckHitCopy {
    CheckHitWords m_start;
    CheckHitWords m_end;
    float m_size;
    u8 m_shape;
};

#define CHECK_HIT_DATA (static_cast<soCollisionHitData*>(unk188[0]))
#define CHECK_HIT_SIMPLE (static_cast<soCollisionHitData::Simple*>(unk188[1]))
#define CHECK_HIT_SET (static_cast<soSet<soCollisionHitData::Simple>*>(unk188[2]))
#define CHECK_DATA_GROUP (static_cast<ykDataGroup*>(unk188[3]))
#define CHECK_DATA (static_cast<ykData*>(unk188[4]))

// The ball's hit object: one hit sphere around the "ball" node (z mirrored) and the damage module (it reports its damage to
// onDamage).
void grGreenhillCheck::setHit() {
    unk188[0] = new (Heaps::StageInstance) soCollisionHitData;
    unk188[1] = new (Heaps::StageInstance) soCollisionHitData::Simple;
    unk188[2] = new (Heaps::StageInstance) soSet<soCollisionHitData::Simple>;
    unk188[3] = new (Heaps::StageInstance) ykDataGroup;
    unk188[4] = new (Heaps::StageInstance) ykData;

    u32 nodeIndex = getNodeIndex(0, "ball");
    CHECK_HIT_DATA->m_startOffsetPos.m_x = 0.0f;
    CHECK_HIT_DATA->m_startOffsetPos.m_y = 5.0f;
    CHECK_HIT_DATA->m_startOffsetPos.m_z = -unk164->m_z;
    CHECK_HIT_DATA->m_endOffsetPos.m_x = 0.0f;
    CHECK_HIT_DATA->m_endOffsetPos.m_y = 5.0f;
    CHECK_HIT_DATA->m_endOffsetPos.m_z = -unk164->m_z;
    CHECK_HIT_DATA->m_size = 8.0f;
    CHECK_HIT_DATA->m_shapeType = soCollision::Shape_Capsule;
    *reinterpret_cast<CheckHitCopy*>(CHECK_HIT_SIMPLE) = *reinterpret_cast<CheckHitCopy*>(CHECK_HIT_DATA);
    CHECK_HIT_SIMPLE->m_height = soCollisionHitData::Height_Low;
    CHECK_HIT_SIMPLE->m_nodeIndex = nodeIndex;

    CHECK_HIT_SET->m_elements = CHECK_HIT_SIMPLE;
    CHECK_HIT_SET->m_size = 1;
    CHECK_DATA_GROUP->m_hitDataSimpleSet = CHECK_HIT_SET;
    CHECK_DATA_GROUP->m_hitGroupIndex = 0;
    CHECK_DATA->m_dataGroupNum = 1;
    CHECK_DATA->m_dataGroups = CHECK_DATA_GROUP;

    ykInitInfo info = { 0, 0, 0x10, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = CHECK_DATA;
    typedef ykNormal<soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 1, 0, soCollisionAttackModuleImpl, 1, false, true>,
                     soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 1, 1, soCollisionHitModuleImpl, 0x3FF, true> >
        Config;
    Config* yakumono = new (Heaps::StageInstance) Config(&info);
    setYakumono(yakumono);
}

// The ball hurts fighters it touches: one hitbox centred on the "ball" node (z mirrored), enabled once.
void grGreenhillCheck::setAttack() {
    if (m_attackEnabled == 1) {
        return;
    }

    soCollisionAttackData attack(1.0f);
    u32 nodeIndex = getNodeIndex(0, "ball");
    Vec3f offset;
    getNodePosition(&offset, 0, nodeIndex);
    float five = 5.0f;
    offset.m_x = 0.0f;
    offset.m_y = five;
    offset.m_z = -offset.m_z;
    float one = 1.0f;

    setAttackGimmickDetails(&attack, five, one, one, one,
        10, &offset, 361, 70, 0, 70, nodeIndex,
        0x3FF, 7, false, 15,
        soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Medium,
        soCollisionAttackData::Sound_Attribute_Punch,
        false, false, false, true, false, false, 0, 60,
        false, false, false, soCollisionAttackData::Lr_Check_Pos,
        false, false, false, false, false, soCollisionAttackData::Region_None, true);
    m_yakumono->setAttack(0, 0, &attack);
    m_attackEnabled = 1;
}


// Tints the ball's marker material: state 0 blue, state 1 red.
void grGreenhillCheck::changeColor(int state) {
    nw4r::g3d::ResMdl resMdl;
    nw4r::g3d::ResMat resMat;
    nw4r::g3d::ResMatTevColor tevColor;
    nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
    if (scnMdl != NULL) {
        resMdl = scnMdl->m_resMdl;
        if (resMdl.IsValid()) {
            resMat = resMdl.GetResMat("markerBall");
            if (resMat.IsValid()) {
                nw4r::g3d::ScnMdl::CopiedMatAccess access(scnMdl, resMat->m_id);
                tevColor = access.GetResMatTevColor(false);
                if (tevColor.IsValid()) {
                    GXColor color;
                    switch (state) {
                    case 0: {
                        GXColor colorA = { 0x00, 0x50, 0x80, 0xFF };
                        color = colorA;
                        break;
                    }
                    case 1: {
                        GXColor colorB = { 0x80, 0x00, 0x00, 0xFF };
                        color = colorB;
                        break;
                    }
                    }
                    tevColor.GXSetTevColor(GX_TEVREG0, color);
                    tevColor.DCStore(false);
                    resMat.DCStore(false);
                }
            }
        }
    }
}

// Switches the ball model to one of its five animations (0 drop, 1 leave, 2 break, 3 roll, 4 idle - HYPOTHESIS names);
// the same sequence of animation binds as the Tengan floor.
void grGreenhillCheck::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
    if (m_animId == animId && force == 0) {
        return;
    }

    nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
    if (sceneMdl == NULL) {
        return;
    }

    gfModelAnimation* modelAnim = *m_modelAnims;
    if (modelAnim == NULL) {
        return;
    }

    nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;
    if (!model.IsValid()) {
        return;
    }

    modelAnim->unbindNodeAnim(sceneMdl);
    modelAnim->unbindVisibleAnim(sceneMdl);
    modelAnim->unbindTexAnim(sceneMdl);
    modelAnim->unbindTexSrtAnim(sceneMdl);
    modelAnim->unbindMatColAnim(sceneMdl);
    m_animId = animId;

    if (animId >= 5) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grGreenhillBindChr(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grGreenhillBindTexPat(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grGreenhillBindTexSrt(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grGreenhillBindMatClr(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grGreenhillBindVis(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(shouldLoop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}

// MATCH-ONLY: the team number of the attacker lives in the collision log of the damage (a signed byte at 0x30).
struct CheckDamageView {
    u8 unk00[0x30];
    s8 m_teamNo;
};

// A fighter hit the ball: remember the attacker's team and let the ball react (state work 3).
void grGreenhillCheck::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    fn_27_26399C(m_yakumono);
    unk15C[0] = 3;
    m_sndGenerator.playSE(static_cast<SndID>(0x1d16), 0, 0, -1);
    m_sndGenerator.setPos(&unk164[m_order[0]]);
    int team = reinterpret_cast<CheckDamageView*>(damage)->m_teamNo;
    m_hitTeam = team;
    if (m_yakumono != NULL) {
        m_yakumono->setTeam(team);
    }
}
