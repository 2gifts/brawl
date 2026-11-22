#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL
#define YK_DATA_INLINE_CTOR
#define SO_COLLISION_GROUP_ALIGNED
#define SO_COLLISION_GROUP_MEMBERWISE

#include <cm/cm_quake.h>
#include <gm/gm_global.h>
#include <gr/gr_gimmick_motion_path.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <sora_adv_stage/gr_adventure_block_mng.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_adventure_move_attack_ground.h>
#include <yk/yk_no_hit_normal.h>
#include <yk/yk_normal.h>
#include <types.h>

// HYPOTHESIS: getInstance/[cmQuake] (main.dol map), not decompiled yet.
extern "C" cmQuake* fn_8009D2DC();

// The sound and effect records of the statue data (id, repeat frame, node and end frame).
struct grKingStatueSoundData {
    int m_id;
    u16 m_repeatFrame;
    u16 m_nodeIndex;
    u16 m_endFrame;
    u16 m_pad;
};

// grGimmick::AttackData as the statue copies it (the copy moves the members by their original types: words where our
// header has bytes or floats).
// TODO: clean up - fix the member types of grGimmick::AttackData instead (shared by many units).
struct grKingStatueAttackData {
    float m_damage;
    Vec3f m_offsetPos;
    float m_size;           // 0x10
    u32 m_unk14;
    u32 m_unk18;
    u32 m_unk1C;
    u32 m_unk20;
    u32 m_unk24;
    u32 m_unk28;
    u8 m_unk2C;
    u8 m_unk2D;
    u8 m_unk2E;
    u8 m_unk2F;
    Vec3f m_unk30;
    u32 m_unk3C;
    u32 m_unk40;
    u32 m_unk44;
    u8 m_unk48;
    u8 m_unk49;
    u16 m_unk4A;
    u8 m_unk4C;
    u8 m_unk4D;
    u8 m_unk4E;
    u8 m_unk4F;
    u32 m_nodeIndex;        // 0x50
    u32 m_unk54;
    u32 m_unk58;
};

// HYPOTHESIS: the type name is from the sora_adv_stage map (createGimmickFunc<23grGimmickKingStatueData, ...>); the
// layout is the move attack ground data with the statue's frames and sound/effect ids.
struct grGimmickKingStatueData {
    u8 m_unk0[0x8];
    float m_speeds[15];                 // 0x8 the animation speed per adventure difficulty level
    u8 m_unk44[0x1EC - 0x44];
    union {
        grGimmick::AttackData m_attackData; // 0x1EC
        u8 m_attackDataRaw[0x5C];
    };
    u8 m_motionPathIndex;               // 0x248
    u8 m_unk249[3];
    u16 m_landRepeatFrame;              // 0x24C the repeat frame of the landing sound/effect (index 2)
    u16 m_landEndFrame;                 // 0x24E
    u16 m_jumpRepeatFrame;              // 0x250 the jump sound/effect (index 3)
    u16 m_waitFrame;                    // 0x252 the frames the statue waits before the jump
    u16 m_attackRepeatFrame;            // 0x254 the attack sound/effects (indices 4, 5)
};

// The king statue of the adventure mode (the map spells the class grAdventurKingStatue): it waits, jumps along a second
// motion path and lands with a quake, attacking with its body.
class grAdventurKingStatue : public grAdventureMoveAttackGround {
public:
    u8 m_state;                             // 0x168
    float m_frame;                          // 0x16C
    float m_unk170;                         // 0x170
    int m_animFrames[3];                    // 0x174
    grGimmickKingStatueData* m_statueData;  // 0x180
    grGimmickMotionPath* m_jumpMotionPath;  // 0x184
    grAdventureBlockHitData m_hitData;      // 0x188 (the hit record with its flag word)
    soSet<grAdventureBlockHitData> m_hitSet; // 0x1A8
    // TODO: clean up - ykDataGroup/ykData have inline constructors in this REL, and the statue constructor clears them in
    // its body after its own members, so they are spelled out here.
    u32 m_hitGroupIndex;                            // 0x1B0 ykDataGroup
    soSet<soCollisionHitData::Simple>* m_hitSimpleSet; // 0x1B4
    soSet<soCollisionHitData>* m_hitDataSet;        // 0x1B8
    int m_dataGroupNum;                             // 0x1BC ykData
    ykDataGroup* m_dataGroups;                      // 0x1C0

    grAdventurKingStatue(const char* taskName) : grAdventureMoveAttackGround(taskName) {
        m_state = 0;
        m_frame = 0.0f;
        m_unk170 = 0.0f;
        m_statueData = NULL;
        m_hitGroupIndex = 0;
        m_hitSimpleSet = NULL;
        m_hitDataSet = NULL;
        m_dataGroupNum = 0;
        m_dataGroups = NULL;
        m_transparencySettings = 0x1F;
        unkF8 = 100.0f;
    }

    // Inline helpers of the statue's TU (startup calls them out of line).
    // HYPOTHESIS: the map names them grGimmick::setGimmickSoundID/setGimmickEffectID (and the ...1 variants that take
    // a record), Ground::getModel and gfTask::addChild; they are declared here because our headers lack them.
    // TODO: clean up - move them to grGimmick/Ground/gfTask.
    void setGimmickSoundID(u32 index, int sndId, u32 repeatFrame, Vec2f offsetPos) {
        if (sndId > 0) {
            m_soundEffects[index].m_id = sndId;
            m_soundEffects[index].m_repeatFrame = repeatFrame;
            m_soundEffects[index].m_nodeIndex = 0;
            m_soundEffects[index].m_endFrame = 0;
            m_soundEffects[index].m_offsetPos = offsetPos;
        }
    }
    void setGimmickEffectID(u32 index, int efId, u32 repeatFrame, u16 nodeIndex, Vec2f offsetPos, float scale) {
        if (efId > 0) {
            m_effects[index].m_id = efId;
            m_effects[index].m_repeatFrame = repeatFrame;
            if (nodeIndex == 0) {
                m_effects[index].m_nodeIndex = getNodeIndex(0, "effect_locator");
            } else {
                m_effects[index].m_nodeIndex = nodeIndex;
            }
            m_effects[index].m_endFrame = 0;
            m_effects[index].m_offsetPos = offsetPos;
            m_effects[index].m_scale = scale;
        }
    }
    void setGimmickSoundID1(u32 index, grKingStatueSoundData* data, short generatorIndex, Vec2f offsetPos) {
        if (data->m_id > 0) {
            m_soundEffects[index].m_id = data->m_id;
            m_soundEffects[index].m_repeatFrame = data->m_repeatFrame;
            // TODO: clean up - the store is unsigned (Effect_Info::m_nodeIndex is short in our header).
            *reinterpret_cast<u16*>(&m_soundEffects[index].m_nodeIndex) = data->m_nodeIndex;
            m_soundEffects[index].m_endFrame = data->m_endFrame;
            m_soundEffects[index].m_generatorIndex = generatorIndex;
            m_soundEffects[index].m_offsetPos = offsetPos;
        }
    }
    void setGimmickEffectID1(u32 index, grKingStatueSoundData* data, Vec2f offsetPos, float scale) {
        if (data->m_id > 0) {
            m_effects[index].m_id = data->m_id;
            m_effects[index].m_repeatFrame = data->m_repeatFrame;
            if (data->m_nodeIndex == 0) {
                m_effects[index].m_nodeIndex = getNodeIndex(0, "effect_locator");
            } else {
                m_effects[index].m_nodeIndex = data->m_nodeIndex;
            }
            m_effects[index].m_endFrame = data->m_endFrame;
            m_effects[index].m_offsetPos = offsetPos;
            m_effects[index].m_scale = scale;
        }
    }
    nw4r::g3d::ScnMdl* getModel(u32 index) { return m_sceneModels[index]; }
    Yakumono* getYakumono() { return m_yakumono; }
    void addChild(gfTask* child) {
        child->m_connectedTask = this;
        if (m_attachedTask == NULL) {
            m_attachedTask = child;
            return;
        }
        gfTask* task = m_attachedTask;
        while (task->m_nextTask != NULL) {
            task = task->m_nextTask;
        }
        task->m_nextTask = child;
    }
    static grAdventurKingStatue* create(int mdlIndex, const char* taskName);
    virtual ~grAdventurKingStatue();
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void update(float deltaFrame);
    virtual void setAttackMoveGround();
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
};
static_assert(sizeof(grAdventurKingStatue) == 0x1C4, "Class is wrong size!");

// The adventure difficulty level the statue speed is read with (the save data level lowered by the continues, kept in
// 0..14).
static inline u8 GmGetAdventureGameLevel() {
    int level = g_GameGlobal->m_advData->m_difficultyLevel - g_GameGlobal->m_advData->unk4916;
    if (level < g_GameGlobal->m_advData->m_difficultyLevel - 5) {
        level = g_GameGlobal->m_advData->m_difficultyLevel - 5;
    }
    if (level < 0) {
        level = 0;
    }
    if (level > 14) {
        level = 14;
    }
    return level;
}

grAdventurKingStatue* grAdventurKingStatue::create(int mdlIndex, const char* taskName) {
    grAdventurKingStatue* statue = new (Heaps::StageInstance) grAdventurKingStatue(taskName);
    if (statue != NULL) {
        statue->setMdlIndex(mdlIndex);
    }
    return statue;
}

grAdventurKingStatue::~grAdventurKingStatue() { }

static inline int kingGetNumFrame(const nw4r::g3d::ResAnmChr& anmChr) {
    return anmChr.ptr()->m_animLength;
}

// MATCH-ONLY: nothing is inlined into startup (the helpers above are called out of line).
#pragma dont_inline on
void grAdventurKingStatue::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grAdventureMoveGround::startup(data, unk1, layerType);
    m_moveAttackData = grGimmick::getGimmickData();
    m_statueData = static_cast<grGimmickKingStatueData*>(grGimmick::getGimmickData());
    m_hitData.m_startOffsetPos = Vec3f(0.0f, 0.0f, 0.0f);
    m_hitData.m_endOffsetPos = m_statueData->m_attackData.m_offsetPos;
    m_hitData.m_size = 20.0f;
    m_hitData.m_nodeIndex = getNodeIndex(0, "EnmPorkystatue_Effect02N");
    m_hitData.m_part = 0;
    m_hitData.m_height = 1;
    m_hitData.m_0x1c_3 = 0;
    m_hitData.m_shapeType = 1;
    m_hitSet.m_elements = &m_hitData;
    m_hitSet.m_size = 1;
    m_hitSimpleSet = NULL;
    m_hitDataSet = reinterpret_cast<soSet<soCollisionHitData>*>(&m_hitSet);
    m_hitGroupIndex = 0;
    m_dataGroupNum = 1;
    m_dataGroups = reinterpret_cast<ykDataGroup*>(&m_hitGroupIndex);

    ykInitInfo info = { 0, 0, 0x11, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(getModel(0));
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = &m_dataGroupNum;
    typedef ykNormal<soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 1, 0, soCollisionAttackModuleImpl, 1, false, true>,
                     soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 1, 1, soCollisionHitModuleImpl, 0x3FF, true> >
        Config;
    setYakumono(new (Heaps::StageInstance) Config(&info));
    setAttackMoveGround();

    m_animFrames[0] = kingGetNumFrame(m_resFile.GetResAnmChr(0UL));
    m_animFrames[1] = kingGetNumFrame(m_resFile.GetResAnmChr(1UL));
    m_animFrames[2] = kingGetNumFrame(m_resFile.GetResAnmChr(2UL));
    m_jumpMotionPath = grGimmickMotionPath::create((short)m_statueData->m_motionPathIndex, "path_locator", "grGimmickMotionPath");
    if (m_jumpMotionPath != NULL) {
        m_jumpMotionPath->setGimmickData(m_statueData);
        addChild(m_jumpMotionPath);
        m_jumpMotionPath->startup(data, 0, static_cast<gfSceneRoot::LayerType>(0));
    }
    m_motionRatio = m_statueData->m_speeds[GmGetAdventureGameLevel()];
    grGimmick::changeNodeAnim(0, 0);
    createSoundWork(5, 1);
    createEffectWork(6);
    setGimmickSoundID(0, 0x22D6, 0, Vec2f(0.0f, 0.0f));
    setGimmickSoundID(1, 0x22D7, 0, Vec2f(0.0f, 0.0f));
    setGimmickEffectID(0, 0xB, 0, Ground::getNodeIndex(0, "EnmPorkystatue_Effect04N"), Vec2f(0.0f, 0.0f), 2.0f);
    setGimmickEffectID(1, 0xB, 0, Ground::getNodeIndex(0, "EnmPorkystatue_Effect05N"), Vec2f(0.0f, 0.0f), 2.0f);

    grKingStatueSoundData landSound = { 0x22D8, 0, 0, 0 };
    landSound.m_repeatFrame = m_statueData->m_landRepeatFrame;
    landSound.m_endFrame = m_statueData->m_landEndFrame;
    setGimmickSoundID1(2, &landSound, 0, Vec2f(0.0f, 0.0f));
    grKingStatueSoundData landEffect = { 0x6B0001, 0, 0, 0 };
    landEffect.m_repeatFrame = m_statueData->m_landRepeatFrame;
    landEffect.m_nodeIndex = Ground::getNodeIndex(0, "EnmPorkystatue_Effect02N");
    landEffect.m_endFrame = m_statueData->m_landEndFrame;
    setGimmickEffectID1(2, &landEffect, Vec2f(0.0f, 0.0f), 1.0f);
    grKingStatueSoundData jumpSound = { 0x22D9, 0, 0, 0 };
    jumpSound.m_repeatFrame = m_statueData->m_jumpRepeatFrame;
    setGimmickSoundID1(3, &jumpSound, 0, Vec2f(0.0f, 0.0f));
    grKingStatueSoundData jumpEffect = { 0x6B0004, 0, 0, 0 };
    jumpEffect.m_repeatFrame = m_statueData->m_jumpRepeatFrame;
    setGimmickEffectID1(3, &jumpEffect, Vec2f(50.0f, -10.0f), 1.0f);
    grKingStatueSoundData attackSound = { 0x22DA, 0, 0, 0 };
    attackSound.m_repeatFrame = m_statueData->m_attackRepeatFrame;
    setGimmickSoundID1(4, &attackSound, 0, Vec2f(0.0f, 0.0f));
    grKingStatueSoundData attackEffect = { 7, 0, 0, 0 };
    attackEffect.m_repeatFrame = m_statueData->m_attackRepeatFrame;
    attackEffect.m_nodeIndex = Ground::getNodeIndex(0, "EnmPorkystatue_Effect04N");
    setGimmickEffectID1(4, &attackEffect, Vec2f(0.0f, 0.0f), 1.5f);
    grKingStatueSoundData attackEffect2 = { 7, 0, 0, 0 };
    attackEffect2.m_repeatFrame = m_statueData->m_attackRepeatFrame;
    attackEffect2.m_nodeIndex = Ground::getNodeIndex(0, "EnmPorkystatue_Effect05N");
    setGimmickEffectID1(5, &attackEffect2, Vec2f(0.0f, 0.0f), 1.5f);

    grYakumono::setTeamYakumono(0x11, -1);
    getYakumono()->setLr(1.0f);
    startGimmickSE(2);
    startGimmickEffect(2);
}
#pragma dont_inline reset

void grAdventurKingStatue::update(float deltaFrame) {
    grAdventureMoveAttackGround::update(deltaFrame);
    switch (m_state) {
    case 0: {
        float frame = m_frame - (u32)m_frame / m_animFrames[0] * m_animFrames[0];
        if (frame < m_motionRatio) {
            startGimmickSE(0);
            startGimmickEffect(0);
        }
        if (frame >= m_animFrames[0] / 2 && frame < m_animFrames[0] / 2 + m_motionRatio) {
            startGimmickSE(1);
            startGimmickEffect(1);
        }
        if (m_gimmickMotionPath->isMoveEnd()) {
            grGimmick::changeNodeAnim(1, 0);
            m_state = 1;
            m_frame = 0.0f;
            startGimmickSE(3);
            startGimmickEffect(3);
            m_motionRatio = 1.0f;
            grYakumono::disableAttack(0);
            grKingStatueAttackData attackData = *reinterpret_cast<grKingStatueAttackData*>(&m_statueData->m_attackData);
            attackData.m_nodeIndex = getNodeIndex(0, "King");
            attackData.m_size = 20.0f;
            grGimmick::AttackDetails details;
            details.m_targetCategory = 0x3FF;
            details.m_unk1 = 0;
            details.m_lrCheck = static_cast<soCollisionAttackData::LrCheck>(0);
            details.m_targetSituationODD = 1;
            details.m_hitStopFrame = 0.0f;
            grYakumono::setAttackGimmick(0, 0, attackData.m_nodeIndex,
                                         reinterpret_cast<grGimmick::AttackData*>(&attackData), &details);
        }
        break;
    }
    case 1:
        if (m_frame >= m_statueData->m_waitFrame) {
            grGimmick::changeNodeAnim(2, 0);
            m_state = 2;
            m_frame = 0.0f;
            m_gimmickMotionPath = m_jumpMotionPath;
            startGimmickSE(4);
            startGimmickEffect(4);
            startGimmickEffect(5);
            setAttackMoveGround();
        }
        break;
    case 2:
        if (m_frame >= m_animFrames[2]) {
            Vec3f pos = getPos();
            fn_8009D2DC()->reqQuake(cmQuake::Amplitude_XL, &pos);
            grGimmick::changeNodeAnim(0, 0);
            m_state = 3;
            m_frame = 0.0f;
            m_gimmickMotionPath->startMove();
            m_motionRatio = m_statueData->m_speeds[GmGetAdventureGameLevel()];
        }
        break;
    case 3: {
        float frame = m_frame - (u32)m_frame / m_animFrames[0] * m_animFrames[0];
        if (frame < m_motionRatio) {
            startGimmickSE(0);
            startGimmickEffect(0);
        }
        if (frame >= m_animFrames[0] / 2 && frame < m_animFrames[0] / 2 + m_motionRatio) {
            startGimmickSE(1);
            startGimmickEffect(1);
        }
        break;
    }
    }
    m_frame += m_statueData->m_speeds[GmGetAdventureGameLevel()];
}

void grAdventurKingStatue::setAttackMoveGround() {
    grGimmick::AttackDetails details;
    details.m_targetCategory = 0x3FF;
    details.m_unk1 = 0;
    details.m_targetSituationODD = 0;
    details.m_lrCheck = static_cast<soCollisionAttackData::LrCheck>(0);
    details.m_hitStopFrame = 1.0f;
    grYakumono::disableAttack(0);
    m_statueData->m_attackData.m_nodeIndex = getNodeIndex(0, "King");
    details.m_lrCheck = static_cast<soCollisionAttackData::LrCheck>(3);
    details.m_targetSituationODD = 1;
    grYakumono::setAttackGimmick(0, 0, m_statueData->m_attackData.m_nodeIndex, &m_statueData->m_attackData, &details);
}

void grAdventurKingStatue::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    clearAttackerInfo();
    grYakumono::onDamage(index, damage, attackerInfo);
}
