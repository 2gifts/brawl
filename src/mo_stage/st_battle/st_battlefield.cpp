// TODO: clean up - raw offset accesses kept to get a match; define the struct with named fields and replace the *(T*)(p + 0x..) casts with readable member accesses
#include <cstdio>
#include <gf/gf_3d_scene.h>
#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <gm/gm_lib.h>
#include <gr/gr_madein.h>
#include <memory.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/math/math_triangular.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st_battle/gr_battlefield.h>
#include <types.h>

#include <st_battle/st_battlefield.h>

stClassInfoImpl<Stages::BattleField, stBattleField> stBattleField::bss_loc_14;

stBattleField* stBattleField::create() {
    return new (Heaps::StageInstance) stBattleField;
}

stBattleField::stBattleField() : stMelee("stBattleField", Stages::Battle) {
    for (int i = 0; i < 5; i++) {
        m_enemyStartPos[i].m_x = 0.0f;
        m_enemyStartPos[i].m_y = 0.0f;
        m_enemyStartPos[i].m_z = 0.0f;
    }
    for (int i = 0; i < 4; i++) {
        m_bossStartPos[i].m_x = 0.0f;
        m_bossStartPos[i].m_y = 0.0f;
        m_bossStartPos[i].m_z = 0.0f;
    }
    for (int i = 0; i < 2; i++) {
        m_fighterStartPos[i].m_x = 0.0f;
        m_fighterStartPos[i].m_y = 0.0f;
        m_fighterStartPos[i].m_z = 0.0f;
    }
}

stBattleField::~stBattleField() {
    releaseArchive();
}

bool stBattleField::loading() {
    return true;
}

// Builds the stage out of its pieces, then reads the 100-Man Brawl spawn points off a throwaway grMadein: the
// "KumiteNode" model only exists to carry the hyakunin_*_start nodes.
void stBattleField::createObj() {
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 20, 1);
    initCameraParam();
    addGround(grBattleField::create(2, "", "grBattleFieldSky"));
    addGround(grBattleField::create(3, "", "grBattleFieldSun"));
    addGround(grBattleField::create(4, "", "grBattleFieldMoon"));
    addGround(grBattleField::create(5, "", "grBattleFieldStar"));
    addGround(grBattleField::create(6, "", "grBattleFieldCloud"));
    addGround(grBattleField::create(1, "", "grBattleFieldMainBg"));
    addGround(grBattleField::create(7, "", "grBattleFieldCrystal"));
    addGround(grBattleField::create(9, "", "grBattleFieldClock"));
    addGround(grBattleField::create(10, "zStgBattleFieldStage", "grBattleFieldStage"));
    addGround(grBattleField::create(11, "zStgBattleFieldAshiba01", "grBattleFieldAshiba01"));
    addGround(grBattleField::create(12, "zStgBattleFieldAshiba02", "grBattleFieldAshiba02"));
    addGround(grBattleField::create(13, "zStgBattleFieldAshiba03", "grBattleFieldAshiba03"));
    addGround(grBattleField::create(8, "", "grBattleFieldFlare"));
    Ground* ground;
    for (u32 i = 0, groundNum = getGroundNum(); i != groundNum; i++) {
        ground = getGround(i);
        if (ground != NULL) {
            ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            ground->setStageData(m_stageData);
            ground->setDontMoveGround();
        }
    }
    createCollision(m_fileData, 2, NULL);
    grMadein* kumiteNode = grMadein::create(200, "", "KumiteNode", Heaps::StageInstance);
    if (kumiteNode != NULL) {
        kumiteNode->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        kumiteNode->initializeEntity();
        kumiteNode->startEntity();
        kumiteNode->updateG3dProcCalcWorld();
        for (int i = 0; i < 5; i++) {
            char nodeName[0x40];
            sprintf(nodeName, "hyakunin_enemy_start%d", i);
            kumiteNode->getNodePosition(&m_enemyStartPos[i], 0, nodeName);
        }
        for (int i = 0; i < 4; i++) {
            char nodeName[0x40];
            sprintf(nodeName, "hyakunin_boss_start%d", i);
            kumiteNode->getNodePosition(&m_bossStartPos[i], 0, nodeName);
        }
        for (int i = 0; i < 2; i++) {
            char nodeName[0x40];
            sprintf(nodeName, "hyakunin_fighter_start%d", i);
            kumiteNode->getNodePosition(&m_fighterStartPos[i], 0, nodeName);
        }
        kumiteNode->endEntity();
        delete kumiteNode;
    }
    void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
    if (posData) {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    } else {
        createStagePositions();
    }
    createWind2ndOnly();
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
    loadStageAttrParam(m_fileData, 30);
    initPosPokeTrainer(1, 0);
    createObjPokeTrainer(m_fileData, 101, "PokeTrainer00", m_pokeTrainerPos, NULL);
}

static inline float stBattleClamp(float lo, float hi, float value) {
    value = nw4r::math::FSelect(value - lo, value, lo);
    return nw4r::math::FSelect(value - hi, hi, value);
}

// The scene animation is a 6000-frame day/night cycle; the stage's shadow (light) direction follows it.
void stBattleField::update(float deltaFrame) {
    stParam* param = m_stageParam;
    if (param != NULL) {
        float frame = 0.0f;
        if (g_gfSceneRoot->m_anmScnRes != NULL) {
            frame = g_gfSceneRoot->m_anmScnRes->GetFrame();
        }
        if (frame >= 0.0f && frame <= 6000.0f) {
            float t = stBattleClamp(0.0f, 1.0f, frame / 6000.0f);
            nw4r::math::SinFIdx(NW4R_MATH_IDX_TO_FIDX(nw4r::math::U16ToF32((u16)(int)(t * 32768.0f))));
            float c = stBattleClamp(0.0f, 1.0f, frame / 6000.0f);
            param->m_shadowPitch = 40.0f;
            param->m_shadowYaw = 240.0f + -120.0f * c;
            *(float*)((u8*)param + 0x20) = 0.0f; // HYPOTHESIS: third float of the shadow direction
            // HYPOTHESIS: tells the scene to pick the changed shadow parameters up
            *(u8*)((u8*)this + 0xE8) = 1;
        } else {
            param->m_shadowPitch = 90.0f;
            param->m_shadowYaw = 0.0f;
            *(float*)((u8*)param + 0x20) = 0.0f;
        }
    }
}

// In 100-Man Brawl the fighters and enemies start from the nodes of the "KumiteNode" model.
void stBattleField::getFighterStartPos(Vec3f* startPos, int fighterIndex) {
    u32 mode = g_GameGlobal->m_modeMelee->m_meleeInitData.m_gameMode;
    if (mode == Game_Mode_Kumite || mode == Game_Mode_Net_Kumite) {
        if ((u32)fighterIndex >= 20) {
            *startPos = m_enemyStartPos[(u32)(fighterIndex - 20) % 5];
        } else if ((u32)fighterIndex >= 10) {
            *startPos = m_bossStartPos[(u32)(fighterIndex - 10) % 5]; // HYPOTHESIS: the original wraps by 5 here although there are 4 boss nodes
        } else {
            *startPos = m_fighterStartPos[fighterIndex & 1];
        }
    } else {
        stMelee::getFighterStartPos(startPos, fighterIndex);
    }
}

// TODO: clean up, give variables real names
namespace jj_AE0 {
extern "C" s32 adventureEventGetItem__6GroundFiP13stTriggerData(void) {
    return 0x0;
}
}

// TODO: clean up, give variables real names
namespace jj_AE8 {
extern "C" s32 getInitializeInfo__6GroundFi(void) {
    return 0x0;
}
}

// TODO: clean up, give variables real names
namespace jj_AF0 {
extern "C" void setInitializeInfo__6GroundFi(void) {}
}

// TODO: clean up, give variables real names
namespace jj_AF4 {
extern "C" void setInitializeFlag__6GroundFv(void) {}
}

// TODO: clean up, give variables real names
namespace jj_AF8 {
extern "C" void disableCalcCollision__6GroundFv(unsigned char* self)
{
    self[0x6C] &= ~0x08;
}
}

// TODO: clean up, give variables real names
namespace jj_B08 {
struct StObj {
    char pad[0x6c];
    unsigned char flags;
};

extern "C" {
void enableCalcCollision__6GroundFv(StObj *self)
{
    self->flags |= 0x8;
}
}
}

// TODO: clean up, give variables real names
namespace jj_B18 {
extern "C" unsigned char isEnableCalcCollision__6GroundFv(unsigned char *arg0)
{
    return (arg0[0x6c] >> 3) & 1;
}
}

// TODO: clean up, give variables real names
namespace jj_B24 {
extern "C" s16 getMdlIndex__6GroundFv(void* self) {
    return *reinterpret_cast<s16*>(reinterpret_cast<u8*>(self) + 0x5c);
}
}

// TODO: clean up, give variables real names
namespace jj_B2C {
extern "C" void setMdlIndex__6GroundFi(void* self, u16 value) {
    *reinterpret_cast<u16*>(reinterpret_cast<u8*>(self) + 0x5c) = value;
}
}

// TODO: clean up, give variables real names
namespace jj_B34 {
extern "C" void initStageData__6GroundFv(void) {}
}

// TODO: clean up, give variables real names
namespace jj_B38 {
extern "C" u32 getStageData__6GroundFv(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x60);
}
}

// TODO: clean up, give variables real names
namespace jj_B40 {
struct ResObject
{
    unsigned char padding[0x40];
    unsigned int file;
};

extern "C" unsigned int GetResMdlNumEntries__Q34nw4r3g3d7ResFileCFv(const void*);

extern "C" unsigned int getModelCount__6GroundFv(ResObject* self)
{
    if (self->file != 0)
        return GetResMdlNumEntries__Q34nw4r3g3d7ResFileCFv(&self->file);
    return 0;
}
}

