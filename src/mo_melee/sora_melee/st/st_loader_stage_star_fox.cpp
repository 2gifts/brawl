#include <cstring>
#include <gf/gf_archive.h>
#include <gf/gf_heap_manager.h>
#include <gm/gm_global.h>
#include <mt/mt_prng.h>
#include <st/loader/st_loader_stage_star_fox.h>
#include <types.h>

extern "C" bool fn_27_23ABA0(stLoader* loader);

// MATCH-ONLY: the native code reads the 16-bit stage kind unsigned, not through the signed enum bitfield.
static inline u32 getStageKind(gmGlobalModeMelee* melee) {
    return *reinterpret_cast<u16*>(reinterpret_cast<u8*>(&melee->m_meleeInitData) + 0x12);
}

stLoaderStageStarFox::stLoaderStageStarFox() {
    m_partIndex = 0;
    m_loadedStageKind = 0;
    m_isLocked = false;
    m_table[0] = 0;
    m_table[1] = 1;
    m_table[2] = 2;
    m_table[3] = 3;
}

stLoaderStageStarFox::~stLoaderStageStarFox() {
    if (m_archive) {
        delete m_archive;
    }
    if (m_secondaryArchive) {
        delete m_secondaryArchive;
    }
    if (m_fileArchive) {
        delete m_fileArchive;
    }
    if (m_effectArchive) {
        delete m_effectArchive;
    }
    if (m_secondaryData) {
        gfHeapManager::free(m_secondaryData);
    }
    if (m_fileData) {
        gfHeapManager::free(m_fileData);
    }
    if (m_effectData) {
        gfHeapManager::free(m_effectData);
    }
    m_archive = nullptr;
    m_secondaryArchive = nullptr;
    m_fileArchive = nullptr;
    m_effectArchive = nullptr;
    m_secondaryData = nullptr;
    m_fileData = nullptr;
    m_effectData = nullptr;
}

void stLoaderStageStarFox::buildFileName() {
    static const char* s_starFoxParts[] = { "ASTEROID", "BATTLESHIP", "CORNERIA", "SPACE" };
    static const char* s_emblemParts[] = { "00", "01", "02" };

    memset(m_fileName, 0, sizeof(m_fileName));
    if (getStageKind(m_globalModeMelee) == 0x13) {
        strcpy(m_fileName, "/STAGE/MELEE/STGSTARFOX_");
        strcat(m_fileName, s_starFoxParts[m_partIndex]);
    } else {
        strcpy(m_fileName, "/STAGE/MELEE/STGEMBLEM_");
        strcat(m_fileName, s_emblemParts[m_partIndex]);
    }
    strcat(m_fileName, ".PAC");
    m_loadedStageKind = getStageKind(m_globalModeMelee);
}

void stLoaderStageStarFox::setStage(Stage* stage) {
    m_stage = stage;
}

// Shuffles the four parts (only a four part table is shuffled), never starting with the part that ended the last round.
void stLoaderStageStarFox::shuffuleTable(int num) {
    m_tableIndex = 0;
    int last = m_table[3];
    m_tableNum = num;
    if (num == 4) {
        for (int i = 0; i < 32; i++) {
            int a = randi(4);
            int b = randi(4);
            u8 tmp = m_table[a];
            m_table[a] = m_table[b];
            m_table[b] = tmp;
        }
        u8 first = m_table[0];
        if (last == first) {
            m_table[0] = m_table[1];
            m_table[1] = first;
        }
    }
}

bool stLoaderStageStarFox::isLoad() {
    return fn_27_23ABA0(this);
}

// TODO: clean up, give variables real names
namespace jj_240190 {
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern "C" {
void* __nw__FUlQ25Heaps8HeapType(u32, int);
void* __ct__9gfArchiveFv(void*);
void __dt__9gfArchiveFv(void*, int);
void setFileImage__9gfArchiveFPviQ25Heaps8HeapType(void*, void*, int, int);
void* getData__9gfArchiveF11ARCNodeTypeiPiUs(void*, int, int, int*, u16);
u32 fn_800151FC(void*, void*, int, int, int, int);
void shuffuleTable__20stLoaderStageStarFoxFi(void*);
}

struct gfTask_base { char pad[0x3c]; };

class gfTask : public gfTask_base {
public:
    virtual void processDefault();
    virtual void processBegin();
    virtual void processAnim();
    virtual void processUpdate();
    virtual void processPreMapCorrection();
    virtual void processMapCorrection();
    virtual void processFixPosition();
    virtual void processPreCollision();
    virtual void processCollision();
    virtual void processCatch();
    virtual void processHit();
    virtual void processCamera();
    virtual void processFixCamera();
    virtual void processEffect();
    virtual void processGameProc();
    virtual void processEnd();
    virtual void renderPre();
    virtual void renderOpa();
    virtual void renderXlu();
    virtual void processDebug();
    virtual void renderDebug();
    virtual void init();
    virtual void slot22();
    virtual void slot23();
    virtual void createObjPokeTrainer();
    virtual void getPokeTrainerPointNum();
    virtual void getPokeTrainerPointData();
    virtual void getPokeTrainerPositionZ();
    virtual void getPokeTrainerDrawLayer(void* p);
    virtual void isAdventureStage();
    virtual void getItemPac();
    virtual void getItemGenPac();
    virtual void getItemPacEnemyFigure();
    virtual void getEnemyPac();
    virtual void getAdvRadarData();
    virtual void initializeStage();
    virtual void closeStage();
    virtual void renderDebugPositions();
    virtual void slot38();
    virtual void process();
    virtual void updateStagePositions();
    virtual void debugCollision();
    virtual void getAIRange();
    virtual void getDefaultLightSetIndex();
    virtual void getZoneLightSetIndex();
    virtual void getScrollDir();
    virtual void clearCameraParam();
    virtual void initCameraParam();
    virtual void startLoadLocalData();
    virtual void isLoadLocalData();
    virtual void entryLocalData();
    virtual void removeLocalData();
    virtual void appearanceFighterLocal();
    virtual void slot53();
    virtual void slot54();
    virtual void slot55();
    virtual void slot56();
    virtual void slot57();
    virtual void slot58();
    virtual void slot59();
    virtual void getKirifudaPos();
    virtual void getKirifudaAngle();
    virtual void getKirifudaScale();
    virtual void getKirifudaModelType();
    virtual void getPokeTrainerPosCount();
    virtual void getPokeTrainerPos();
    virtual void getFighterDeadEffectSizeRate();
    virtual void getEnemyDeadEffectSizeRate();
    virtual void getEnableZ();
    virtual void slot69();
    virtual void getBgmID();
    virtual void getNowStepBgmID();
    virtual void getBgmOptionID();
    virtual void isBgmChange();
    virtual void getBgmChangeID();
    virtual void setBgmChange();
    virtual void getBgmVolume();
    virtual void getBgmPlayOffsetFrame();
    virtual void isNextStepBgmEqualNowStepBgmFromFlag();
    virtual void setFrameRuleTime();
    virtual void getFrameRuleTime();
    virtual void notifyTimmingGameStart();
    virtual void isEnd();
    virtual void isEventEnd();
    virtual void enableDevil();
    virtual void disableDevil();
    virtual void isDevil();
    virtual void setDevilScrool();
    virtual void getLucarioFinalTechniquePosition();
    virtual void startAppear();
    virtual void setAppearKind();
    virtual void endAppear();
    virtual void getAppearTask();
    virtual void forceStopAppear();
    virtual void slot94();
    virtual void setMotionRatio();
    virtual void saveMotionRatio();
    virtual void restoreMotionRatio();
    virtual void setMotionSubRatio();
    virtual void saveMotionSubRatio();
    virtual void restoreMotionSubRatio();
    virtual void isCameraLocked();
    virtual void isBossBattleMode();
    virtual void isSimpleBossBattleMode();
    virtual void isAppear();
    virtual void isStartAppearTimming();
    virtual void getMadeinAiData();
    virtual void slot107();
    virtual void getBamperVector();
    virtual void slot109();
    virtual void slot110();
    virtual void getDestroyBossParamCommon();
    virtual void stAdventureEventGetItem();
    virtual void setStageOutEffectInit();
    virtual void setStageInEffectInit();
    virtual void helperStarWarp();
    virtual void initializeFighterAttackRatio();
    virtual void startFighterEvent();
    virtual void getZoneState();
    virtual void getZonePos();
    virtual void getMagmaHeight();
    virtual void getAcidHeight();
    virtual void getIteamDropStatus();
    virtual void createWind2ndOnly();
    virtual void getWind2ndOnlyData__7stMeleeFv();
    virtual void updateWind2ndOnly();
    virtual void setVision();
    virtual void slot127();
    virtual void setCameraLimitRange();
    virtual void resetCameraLimitRange();
    virtual u32 checkChangeScene();
    virtual void resetChangeScene();
    virtual void setChangeSceneNumber(u8 n);

    void* p40;
    u8 state;
    u8 pad45;
    u8 enabled;
    u8 flag47;
    char pad48[0x54];
    void* arc9c;
    void* arca0;
    char padA4[0x20];
    gfTask* other;
    char padC8[4];
    u8 unkCC;
    u8 unkCD;
    u8 flagCE;
    u8 idxCF;
    u8 countD0;
};

extern "C" void processBegin__20stLoaderStageStarFoxFv(gfTask* self) {
    if (self->enabled != 0) {
        switch (self->state) {
        case 0:
            if (self->flag47 != 1 && ((self->flagCE >> 7) & 1) != 1U) {
                u16 id = *(u16*)((char*)self->p40 + 0x1A);
                if (id == 0x13 || id == 0x19) {
                    self->state = 1;
                }
            }
            break;
        case 1:
            if (self->arc9c == 0) {
                void* p = __nw__FUlQ25Heaps8HeapType(0x80, 0x11);
                if (p != 0) {
                    p = __ct__9gfArchiveFv(p);
                }
                self->arc9c = p;
            }
            {
                u8* base = (u8*)self + self->idxCF;
                self->unkCC = base[0xD1];
                self->getPokeTrainerDrawLayer(base);
            }
            if (fn_800151FC(self->arc9c, self->pad48, 0x11, 0, 0, 0xFF) == 1) {
                void* q = __nw__FUlQ25Heaps8HeapType(0x80, 0x11);
                if (q != 0) {
                    q = __ct__9gfArchiveFv(q);
                }
                self->arca0 = q;
                self->state = 2;
                return;
            }
            break;
        case 2:
            if ((((u8*)self->arc9c)[9] >> 7) & 1) {
                int size;
                void* data = getData__9gfArchiveF11ARCNodeTypeiPiUs(self->arc9c, 1, 1, &size, 0xFFFE);
                setFileImage__9gfArchiveFPviQ25Heaps8HeapType(self->arca0, data, size, 0x11);
                self->state = 3;
                return;
            }
            break;
        case 3:
            if (self->other != 0) {
                *(void**)((char*)self->other + 0x1A8) = self->arca0;
                self->other->setChangeSceneNumber(self->unkCC);
                if (self->other->checkChangeScene() == 1 || self->flag47 == 1 ||
                    (int)self->unkCD != (int)*(u16*)((char*)self->p40 + 0x1A) ||
                    self->unkCC != ((u8*)self + self->idxCF)[0xD1]) {
                    self->state = 5;
                    *(void**)((char*)self->other + 0x1A8) = 0;
                    self->other->resetChangeScene();
                    u8 count = self->countD0;
                    if (++self->idxCF >= count) {
                        shuffuleTable__20stLoaderStageStarFoxFi(self);
                        return;
                    }
                }
            } else if (self->flag47 == 1 ||
                       (int)self->unkCD != (int)*(u16*)((char*)self->p40 + 0x1A) ||
                       self->unkCC != ((u8*)self + self->idxCF)[0xD1]) {
                self->state = 5;
            }
            break;
        case 5:
            if (self->arc9c != 0) {
                __dt__9gfArchiveFv(self->arc9c, 1);
            }
            if (self->arca0 != 0) {
                __dt__9gfArchiveFv(self->arca0, 1);
            }
            self->arc9c = 0;
            self->arca0 = 0;
            if (self->flag47 == 1) {
                self->state = 6;
                return;
            }
            self->state = 0;
            break;
        }
    }
}
}

