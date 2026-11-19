// MATCH-ONLY: the constructor re-extends the s16 manage id at every use.
#define SO_EVENT_OBSERVER_MANAGE_S16
#include <cm/cm_camera_controller.h>
#include <ft/ft_manager.h>
#include <gm/gm_global.h>
#include <if/if_mngr.h>
#include <if/if_player.h>
#include <ml/ml_ft_event.h>
#include <nt/nt_etc_upload_eval.h>
#include <revolution/OS/OSError.h>
#include <so/damage/so_damage.h>
#include <st/operator/st_operator_info.h>
#include <st/operator/st_operator_rule.h>
#include <st/stage.h>
#include <st_madein/st_madein.h>
#include <types.h>
#include <st/st_result.h>

extern "C" u8 fn_27_111D70(ftOwner* owner);

mlFtEvent::~mlFtEvent() {
}

void mlFtEvent::notifyEventBeat(int entryId1, int entryId2) {
    if (!isInfoManager()) {
        return;
    }
    if (!unk34_20) {
        return;
    }
    if (m_operatorRule != NULL && m_operatorRule->isGameSet()) {
        return;
    }
    int playerNo1 = g_ftManager->getPlayerNo(entryId1);
    int playerNo2 = g_ftManager->getPlayerNo(entryId2);
    if (m_operatorRule != NULL) {
        m_operatorRule->notifyEventBeat(playerNo1, playerNo2);
    } else if (m_condition->m_meleeInitData.m_0x3_2 && g_IfMngr != NULL) {
        if (m_condition->m_meleeInitData.m_isTeams == true
            && static_cast<u8>(m_condition->m_playersInitData[playerNo1].m_teamNo) == static_cast<u8>(m_condition->m_playersInitData[playerNo2].m_teamNo)) {
            g_IfMngr->m_ifPlayers[m_condition->m_playersInitData[playerNo1].m_playerId]->setScoreVary(-1);
            g_IfMngr->m_ifPlayers[m_condition->m_playersInitData[playerNo2].m_playerId]->setScoreVary(-1);
        } else {
            g_IfMngr->m_ifPlayers[m_condition->m_playersInitData[playerNo1].m_playerId]->setScoreVary(1);
            g_IfMngr->m_ifPlayers[m_condition->m_playersInitData[playerNo2].m_playerId]->setScoreVary(-1);
        }
    }
    nteUpdateEvaluator* evaluator = nteUpdateEvaluator::getInstance();
    if (evaluator != NULL) {
        evaluator->notifyEventBeat(playerNo1, playerNo2);
    }
}

void mlFtEvent::notifyEventChangeEnd(int entryId, int playerNo, int activeInstanceIndex, ftKind kind) {
    if (!isInfoManager()) {
        return;
    }
    u32 faceKind;
    switch (g_ftManager->getFighterGmKind(entryId)) {
    case 29:
    case 31:
    case 33:
        switch (kind) {
        case 0x1D: faceKind = 0x1D; break;
        case 0x1F: faceKind = 0x21; break;
        case 0x1E: faceKind = 0x1F; break;
        }
        break;
    case 3:
    case 4:
        switch (kind) {
        case 3: faceKind = 3; break;
        case 0x18: faceKind = 4; break;
        }
        break;
    case 14:
    case 15:
        switch (kind) {
        case 0xD: faceKind = 0xE; break;
        case 0xE: faceKind = 0xF; break;
        }
        break;
    case 12:
    case 44:
        switch (kind) {
        case 0xB: faceKind = 0xC; break;
        case 0x30: faceKind = 0x2C; break;
        }
        break;
    case 23:
    case 45:
        switch (kind) {
        case 0x15: faceKind = 0x17; break;
        case 0x31: faceKind = 0x2D; break;
        }
        break;
    default:
        return;
    }
    g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[getInfoAreaNo(playerNo)])]->setStockFace(
        0, faceKind);
}

void mlFtEvent::notifyEventOnDamage(int entryId, u32 hp, soDamage* damage) {
    int playerNo = g_ftManager->getPlayerNo(entryId);
    if (m_operatorInfo != NULL) {
        m_operatorInfo->m_playerDamage[playerNo] = hp;
    }
    if (!isInfoManager()) {
        return;
    }
    if (m_condition->m_meleeInitData.m_hideDamageGauge) {
        return;
    }
    if (!unk34_20) {
        return;
    }
    if (m_operatorRule != NULL && m_operatorRule->isGameSet()) {
        return;
    }
    if (hp != 0) {
        IfPlayer* damagePlayer = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[getInfoAreaNo(playerNo)])];
        damagePlayer->updateDamageHP(hp, 0);
        float rumbleRate = getDamageInfoRumbleRate(damage->m_damageAdd);
        IfPlayer* player = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[getInfoAreaNo(playerNo)])];
        if (player->unk1C == player->unk18) {
            player->unk264 = rumbleRate;
            player->setDamageState(1);
        }
    }
    nteUpdateEvaluator* evaluator = nteUpdateEvaluator::getInstance();
    if (evaluator != NULL) {
        evaluator->notifyEventOnDamage(playerNo, hp);
    }
}

void mlFtEvent::notifyEventRecover(int entryId, int hp) {
    int playerNo = g_ftManager->getPlayerNo(entryId);
    if (!isInfoManager()) {
        return;
    }
    if (m_condition->m_meleeInitData.m_hideDamageGauge) {
        return;
    }
    if (!unk34_20) {
        return;
    }
    if (m_operatorRule != NULL && m_operatorRule->isGameSet()) {
        return;
    }
    if (playerNo >= 0) {
        IfPlayer* player = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[getInfoAreaNo(playerNo)])];
        player->updateDamageHP(hp, 1);
    }
}

void mlFtEvent::notifyEventSetDamage(int entryId, float damage, u32 percent, bool p4, bool p5) {
    int playerNo = g_ftManager->getPlayerNo(entryId);
    if (m_operatorInfo != NULL) {
        m_operatorInfo->m_playerDamage[playerNo] = percent;
    }
    if (!isInfoManager()) {
        return;
    }
    if (m_condition->m_meleeInitData.m_hideDamageGauge) {
        return;
    }
    if (!unk34_20) {
        return;
    }
    if (m_operatorRule != NULL && m_operatorRule->isGameSet()) {
        return;
    }
    CameraController::getInstance()->clearDisablePlayerNo(playerNo);
    if (playerNo >= 0) {
        IfPlayer* player = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[getInfoAreaNo(playerNo)])];
        player->updateDamageHP(percent, p5);
        if (p4) {
            float rumbleRate = getDamageInfoRumbleRate(damage);
            IfPlayer* rumblePlayer = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[getInfoAreaNo(playerNo)])];
            if (rumblePlayer->unk1C == rumblePlayer->unk18) {
                rumblePlayer->unk264 = rumbleRate;
                rumblePlayer->setDamageState(1);
            }
        }
    } else {
        OSReport("mlFtEvent::notifyEventSetDamage() nEntryID(0x%08x) is not safe!!!\n", entryId);
    }
}

void mlFtEvent::notifyEventSuicide(int entryId) {
    if (!isInfoManager()) {
        return;
    }
    if (!unk34_20) {
        return;
    }
    if (m_operatorRule != NULL && m_operatorRule->isGameSet()) {
        return;
    }
    int playerNo = g_ftManager->getPlayerNo(entryId);
    if (g_IfMngr != NULL && m_condition->m_meleeInitData.m_0x3_2) {
        g_IfMngr->m_ifPlayers[m_condition->m_playersInitData[playerNo].m_playerId]->setScoreVary(
            m_condition->m_meleeInitData.m_suicideScoreMultiplier);
    }
    if (g_Stage == NULL) {
        return;
    }
    if (unk38 != entryId) {
        return;
    }
    g_Stage->endAppear();
}

bool mlFtEvent::isInfoManager() {
    return g_IfMngr != NULL;
}

mlFtEvent::mlFtEvent() : ftOutsideEventObserver(-1, 0) {
    if (m_sendID <= -1) {
        soEventManageModule* module = &g_ftManager->m_eventManageModule;
        addObserverSubS16(module->getManageId(), this, -1);
    }
    m_condition = g_GameGlobal->m_modeMelee;
    m_operatorInfo = NULL;
    m_operatorRule = NULL;
    unk38 = 0;
    unk34_80 = true;
    unk34_40 = false;
    unk34_20 = true;
    unk34_10 = false;
}

void mlFtEvent::destroy() {
    delete this;
}

int mlFtEvent::getInfoAreaNo(int playerNo) {
    return m_condition->m_playersInitData[playerNo].m_playerId;
}

float mlFtEvent::getDamageInfoRumbleRate(float damage) {
    float min = 3.0f;
    float max = 15.0f;
    float rate = 0.6f;
    if (damage >= min && damage <= max) {
        return rate * ((damage - min) / (max - min));
    }
    if (damage < min) {
        return 0.0f;
    }
    return 1.0f;
}

void mlFtEvent::notifyEventKnockout(int entryId) {
    if (m_operatorRule != NULL) {
        m_operatorRule->notifyEventKnockout(g_ftManager->getScoreEntryId(entryId));
    }
    int playerNo = g_ftManager->getPlayerNo(entryId);
    CameraController::getInstance()->setDisablePlayerNo(playerNo);
}

void mlFtEvent::setCondition(gmGlobalModeMelee* condition) {
    m_condition = condition;
}

void mlFtEvent::notifyEventAddDragoonParts(int entryId, int part) {
    if (isInfoManager()) {
        g_IfMngr->addPlayerDragoonParts(getInfoAreaNo(g_ftManager->getPlayerNo(entryId)), part);
    }
}

void mlFtEvent::notifyEventRemoveDragoonParts(int entryId, int part) {
    if (isInfoManager()) {
        int playerNo = g_ftManager->getPlayerNo(entryId);
        g_IfMngr->deletePlayerDragoonParts(getInfoAreaNo(playerNo), part);
        if (fn_27_111D70(g_ftManager->getOwner(entryId)) == 2) {
            g_IfMngr->setPlayerDragoonPartsComplete(getInfoAreaNo(playerNo), 0);
        }
    }
}

void mlFtEvent::notifyEventCompDragoonParts(int entryId) {
    if (isInfoManager()) {
        g_IfMngr->setPlayerDragoonPartsComplete(getInfoAreaNo(g_ftManager->getPlayerNo(entryId)), 1);
    }
}

void mlFtEvent::notifyEventResetDragoonParts(int entryId) {
    if (isInfoManager()) {
        g_IfMngr->resetPlayerDragoonParts(getInfoAreaNo(g_ftManager->getPlayerNo(entryId)));
    }
}

void mlFtEvent::notifyEventChangeStart(int entryId, int playerNo, int activeInstanceIndex, ftKind kind) { }

void mlFtEvent::notifyEventRemoveEntry(int entryId) {
    if (isInfoManager() && unk34_80 && m_operatorInfo != NULL) {
        int areaNo = getInfoAreaNo(g_ftManager->getPlayerNo(entryId));
        m_operatorInfo->setPlayerCursorClear(areaNo);
        m_operatorInfo->setPlayerLoupeStatus(areaNo, 0);
    }
}

void mlFtEvent::notifyEventGetItem(int entryId, itKind kind, int itVariation, int genParamId, int instanceId) { }

void mlFtEvent::notifyEventReEntryRequestFighter() { }

void mlFtEvent::notifyEventSucceedHit(int entryId, u32 consecutiveHits, float totalDamage) { }

void mlFtEvent::notifyEventFinalSlow(int entryId, float rate, int frames) {
    if (g_Stage != NULL) {
        g_Stage->setMotionRatio(rate, frames);
    }
}

void mlFtEvent::notifyEventFinalSlowCancel(int entryId) {
    if (g_Stage != NULL) {
        g_Stage->setMotionRatio(1.0f, 0.0f);
    }
}

void mlFtEvent::notifyEventFinalStop(int entryId) {
    if (g_Stage != NULL) {
        g_Stage->setMotionRatio(0.0f, 0.0f);
    }
}

void mlFtEvent::notifyEventFinalStopCancel(int entryId) {
    if (g_Stage != NULL) {
        g_Stage->setMotionRatio(1.0f, 0.0f);
    }
}

void mlFtEvent::notifyEventPikminMakeBloomAll(int entryId) {
    if (m_operatorRule != NULL) {
        m_operatorRule->notifyEventPikminBloom();
    }
}

void mlFtEvent::notifyEventHeartSwapStart(int entryId1, int entryId2) {
    if (isInfoManager()) {
        int areaNo1 = getInfoAreaNo(g_ftManager->getPlayerNo(entryId1));
        g_IfMngr->doHeartSwap(areaNo1, getInfoAreaNo(g_ftManager->getPlayerNo(entryId2)));
    }
}

void mlFtEvent::notifyEventHeartSwapEnd(int entryId1, int entryId2) {
    if (isInfoManager()) {
        g_IfMngr->initHeartSwap();
    }
}

bool mlFtEventResult::isInfoManager() {
    return false;
}

void mlFtEvent::notifyEventOnInput(int entryId) {
    if (g_Stage != NULL) {
        if (m_condition->m_meleeInitData.m_stageKind == Stages::Madein && g_Stage != NULL) {
            int playerNo = g_ftManager->getPlayerNo(entryId);
            static_cast<stMadein*>(g_Stage)->m_unk954[playerNo & 3] = 0;
        }
    }
}

mlFtEventTraining::~mlFtEventTraining() {
}

void mlFtEventTraining::notifyEventOnDamage(int entryId, u32 hp, soDamage* damage) {
    mlFtEvent::notifyEventOnDamage(entryId, hp, damage);
    if (unk34_80 && m_operatorInfo != NULL && g_ftManager->getPlayerNo(entryId) != 0) {
        m_damage = static_cast<int>(damage->m_damageAdd);
        if (g_IfMngr != NULL) {
            g_IfMngr->setTrainingStatusDamage(m_damage);
        }
    }
}

void mlFtEventResult::notifyEventResultWin(int entryId, int winAnim) {
    if (g_Stage != NULL) {
        int fighterKind = g_ftManager->getResultFighterGmKind(entryId);
        stResult* stage = static_cast<stResult*>(g_Stage);
        stage->unk270 = 1;
        stage->m_winningCharKind = fighterKind;
        stage->m_winAnim = winAnim;
    }
}

mlFtEventResult::~mlFtEventResult() {
}

void mlFtEventTraining::notifyEventSucceedHit(int entryId, u32 consecutiveHits, float totalDamage) {
    if (unk34_80 && m_operatorInfo != NULL && g_ftManager->getPlayerNo(entryId) != 0) {
        unk40 = consecutiveHits;
        unk44 = static_cast<int>(totalDamage);
        if (g_IfMngr != NULL) {
            g_IfMngr->setTrainingStatusCombo(consecutiveHits);
            g_IfMngr->setTrainingStatusTotalDamage(unk44);
        }
        if (unk4C < unk40) {
            unk4C = unk40 <= 65535 ? unk40 : 65535;
            unk48->unkC8 = unk4C;
        }
    }
}

void mlFtEvent::notifyEventAppeal(int entryId, int appeal) {
    if (!isInfoManager()) {
        return;
    }
    if (unk34_40 && appeal < 4 && g_IfMngr != NULL) {
        int areaNo = getInfoAreaNo(g_ftManager->getPlayerNo(entryId));
        g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[areaNo])]->dispApealCommentBalloon(appeal);
        Vec3f position = g_ftManager->getFighterCursorPos(entryId, -1);
        g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[areaNo])]->setApealCommentBalloonPos(
            (const nw4r::math::VEC3*)&position);
    }
    if (appeal == 4) {
        switch (g_ftManager->getFighterGmKind(entryId)) {
        case 7:
            if (m_condition->m_meleeInitData.m_stageKind == 0x13 && g_Stage != NULL) {
                g_Stage->setAppearKind(0);
                unk34_10 = g_Stage->startAppear();
                unk38 = entryId;
                unk34_10 = true;
            }
            if (m_condition->m_meleeInitData.m_stageKind == 0x30 && g_Stage != NULL) {
                g_Stage->setAppearKind(0);
                unk34_10 = g_Stage->startAppear();
                unk38 = entryId;
            }
            break;
        case 0x15:
            if (m_condition->m_meleeInitData.m_stageKind == 0x13 && g_Stage != NULL) {
                g_Stage->setAppearKind(1);
                unk34_10 = g_Stage->startAppear();
                unk38 = entryId;
            }
            if (m_condition->m_meleeInitData.m_stageKind == 0x30 && g_Stage != NULL) {
                g_Stage->setAppearKind(1);
                unk34_10 = g_Stage->startAppear();
                unk38 = entryId;
            }
            break;
        case 0x29:
            if (m_condition->m_meleeInitData.m_stageKind == 0x13 && g_Stage != NULL) {
                g_Stage->setAppearKind(2);
                unk34_10 = g_Stage->startAppear();
                unk38 = entryId;
            }
            break;
        case 0x2A:
            if (m_condition->m_meleeInitData.m_stageKind == 0x22 && g_Stage != NULL) {
                g_Stage->setAppearKind(0);
                unk34_10 = g_Stage->startAppear();
                unk38 = entryId;
            }
            break;
        }
    } else if (m_condition->m_meleeInitData.m_stageKind == Stages::Madein && g_Stage != NULL) {
        int playerNo = g_ftManager->getPlayerNo(entryId);
        static_cast<stMadein*>(g_Stage)->m_unk9E3[static_cast<u8>(playerNo)] = 1;
    }
}

void mlFtEvent::notifyEventResultWin(int entryId, int winAnim) {
    OSReport("win no %d \n", winAnim);
    if (g_Stage != NULL) {
        int fighterKind = g_ftManager->getResultFighterGmKind(entryId);
        stResult* stage = static_cast<stResult*>(g_Stage);
        stage->unk270 = 1;
        stage->m_winningCharKind = fighterKind;
        stage->m_winAnim = winAnim;
    }
}

// TODO: clean up, give variables real names
namespace jj_27D810 {

extern "C" {
extern char* g_IfMngr;
void setDamageState__8IfPlayerFi(void*, int);
void updateStockCoin__8IfPlayerFi(void*, int);
void setDisablePlayerNo__16CameraControllerFi(void*, unsigned int);
}
struct Stage_base { char pad[0x3c]; };
class Stage : public Stage_base { public:
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
virtual void destructor();
virtual void createObj();
virtual void createObjPokeTrainer();
virtual void getPokeTrainerPointNum();
virtual int getPokeTrainerPointData();
virtual void getPokeTrainerPositionZ();
virtual void getPokeTrainerDrawLayer();
virtual void isAdventureStage();
virtual void getItemPac();
virtual void getItemGenPac();
virtual void getItemPacEnemyFigure();
virtual int getEnemyPac(int, int, int);
virtual void getAdvRadarData();
virtual void initializeStage();
virtual void closeStage();
virtual void renderDebugPositions();
virtual void loading();
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
virtual void getFighterStartPos();
virtual void getFighterReStartPos();
virtual void isReStartSamePoint();
virtual void getPokeTrainerStartPos();
virtual void getItemPosCount();
virtual void getItemPos();
virtual void getRandItemPos();
virtual void getKirifudaPos();
virtual void getKirifudaAngle();
virtual void getKirifudaScale();
virtual void getKirifudaModelType();
virtual void getPokeTrainerPosCount();
virtual void getPokeTrainerPos();
virtual void getFighterDeadEffectSizeRate();
virtual void getEnemyDeadEffectSizeRate();
virtual void getEnableZ();
virtual void getBgmID();
virtual void getBgmID_2();
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
char pad40[8];
float value;
};
extern Stage* g_Stage;
struct Player { char pad[0x5c]; };
struct Data { char pad[9]; unsigned char mode; char padA; unsigned char flagB; };
class Self {
public:
virtual void slot0();
virtual void slot1();
virtual void slot2();
virtual void slot3();
virtual void slot4();
virtual void slot5();
virtual void slot6();
virtual void slot7();
virtual void slot8();
virtual void slot9();
virtual void slot10();
virtual void slot11();
virtual void slot12();
virtual void slot13();
virtual void slot14();
virtual void slot15();
virtual void slot16();
virtual void slot17();
virtual void slot18();
virtual void slot19();
virtual void slot20();
virtual void slot21();
virtual void slot22();
virtual void slot23();
virtual void slot24();
virtual void slot25();
virtual void slot26();
virtual void slot27();
virtual void slot28();
virtual void slot29();
virtual void slot30();
virtual void slot31();
virtual void slot32();
virtual void slot33();
virtual void slot34();
virtual void slot35();
virtual void slot36();
virtual void slot37();
virtual void slot38();
virtual void slot39();
virtual void slot40();
virtual void slot41();
virtual void slot42();
virtual int slot43();
char pad04[8];
Data* data;
Stage* entries[8];
Stage* module;
unsigned char flags;
char pad35[3];
unsigned int id;
};

inline int decrement(Self* self, int playerNo) {
    int off = playerNo * 0x5c;
    signed char c = *(signed char*)((char*)self->data + off + 0x9c);
    if (c != 0) *(signed char*)((char*)self->data + off + 0x9c) = c - 1;
    int v;
    int c2 = *(signed char*)((char*)self->data + off + 0x9c);
    v = c2;
    if (v < 0) v = 0;
    if (self->slot43() != 0 && ((self->flags >> 5) & 1) != 0) {
        updateStockCoin__8IfPlayerFi(*(void**)(g_IfMngr + *(unsigned char*)((char*)self->data + off + 0x9a) * 4 + 0x4c), v);
    }
    return c2 != 0;
}
extern "C" void fn_27_27D810(Self* self, int entryId, int u1, int u2, int arg7) {
    int result = 0;
    int playerNo = g_ftManager->getPlayerNo(entryId);
    if (self->slot43() != 0 && ((self->data->flagB >> 1) & 1) == 0 &&
        ((self->flags >> 5) & 1) != 0 && (!self->module || !self->module->getPokeTrainerPointData())) {
        int off = playerNo * 0x5c;
        unsigned char idx = *(unsigned char*)((char*)self->data + off + 0x9a);
        unsigned char b = *(unsigned char*)(g_IfMngr + idx + 0x68);
        setDamageState__8IfPlayerFi(*(void**)(g_IfMngr + (b << 2) + 0x4c), 2);
    }
    if (self->module) {
        result = self->module->getEnemyPac(entryId, playerNo, arg7);
    } else if ((unsigned int)((self->data->mode >> 5) & 7) == 1) {
        if (decrement(self, playerNo)) result = 1;
    } else {
        result = 1;
    }
    if (result && arg7 >= 0) {
        self->entries[playerNo]->value = (float)arg7;
        self->entries[playerNo]->createObj();
    } else {
        setDisablePlayerNo__16CameraControllerFi(CameraController::getInstance(), playerNo);
    }
    if (g_Stage && self->id == (unsigned int)entryId) {
        g_Stage->endAppear();
    }
}
}

// TODO: clean up, give variables real names
namespace jj_27E498 {
class ftManager {
public:
    int getPlayerNo(int entryId);
};

extern ftManager* g_ftManager;

struct SubPad {
    u8 pad[0x3c];
};

class SubObj : public SubPad {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void vf5();
    virtual void vf6();
    virtual void vf7();
    virtual void vf8();
    virtual void vf9();
    virtual void vf10();
    virtual void vf11();
    virtual void vf12();
    virtual void vf13();
    virtual void vf14();
    virtual void vf15();
    virtual void vf16();
    virtual void vf17();
    virtual void vf18();
    virtual void vf19();
    virtual void vf20();
    virtual void vf21();
    virtual void vf22();
    virtual void vf23();
    virtual void vf24();
    virtual void vf25();
    virtual void vf26();
    virtual void vf27();
    virtual void vf28();
    virtual void vf29();
    virtual void vf32(u32 a, u32 b);
};

class Self {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void vf5();
    virtual void vf6();
    virtual void vf7();
    virtual void vf8();
    virtual void vf9();
    virtual void vf10();
    virtual void vf11();
    virtual void vf12();
    virtual void vf13();
    virtual void vf14();
    virtual void vf15();
    virtual void vf16();
    virtual void vf17();
    virtual void vf18();
    virtual void vf19();
    virtual void vf20();
    virtual void vf21();
    virtual void vf22();
    virtual void vf23();
    virtual void vf24();
    virtual void vf25();
    virtual void vf26();
    virtual void vf27();
    virtual void vf28();
    virtual void vf29();
    virtual void vf30();
    virtual void vf31();
    virtual void vf32();
    virtual void vf33();
    virtual void vf34();
    virtual void vf35();
    virtual void vf36();
    virtual void vf37();
    virtual void vf38();
    virtual void vf39();
    virtual void vf40();
    virtual void vf41();
    virtual void vf42();
    virtual bool vf45();
    u8 m_pad4[8];
    u8* m_entries;
    u8 m_pad10[0x1c];
    SubObj* m_sub;
    u8 m_pad30[4];
    u8 m_flags;
};

extern "C" void fn_27_27E498(Self* self, u32 entryId, u32 arg) {
    if (self->vf45()) {
        if ((self->m_flags >> 7) & 1) {
            if (self->m_sub != 0) {
                int playerNo = g_ftManager->getPlayerNo(entryId);
                u8* entry = self->m_entries + playerNo * 0x5c;
                if (!((entry[0xb3] >> 2) & 1)) {
                    self->m_sub->vf32(entry[0x9a], arg);
                }
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_27E54C {
class ftManager {
public:
    int getPlayerNo(int entryId);
};

extern ftManager* g_ftManager;

struct SubPad {
    u8 pad[0x3c];
};

class SubObj : public SubPad {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void vf5();
    virtual void vf6();
    virtual void vf7();
    virtual void vf8();
    virtual void vf9();
    virtual void vf10();
    virtual void vf11();
    virtual void vf12();
    virtual void vf13();
    virtual void vf14();
    virtual void vf15();
    virtual void vf16();
    virtual void vf17();
    virtual void vf18();
    virtual void vf19();
    virtual void vf20();
    virtual void vf21();
    virtual void vf22();
    virtual void vf23();
    virtual void vf24();
    virtual void vf25();
    virtual void vf26();
    virtual void vf27();
    virtual void vf28();
    virtual void vf29();
    virtual void vf32();
    virtual void vf33(u8, s32);
};

class SelfT {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void vf5();
    virtual void vf6();
    virtual void vf7();
    virtual void vf8();
    virtual void vf9();
    virtual void vf10();
    virtual void vf11();
    virtual void vf12();
    virtual void vf13();
    virtual void vf14();
    virtual void vf15();
    virtual void vf16();
    virtual void vf17();
    virtual void vf18();
    virtual void vf19();
    virtual void vf20();
    virtual void vf21();
    virtual void vf22();
    virtual void vf23();
    virtual void vf24();
    virtual void vf25();
    virtual void vf26();
    virtual void vf27();
    virtual void vf28();
    virtual void vf29();
    virtual void vf30();
    virtual void vf31();
    virtual void vf32();
    virtual void vf33();
    virtual void vf34();
    virtual void vf35();
    virtual void vf36();
    virtual void vf37();
    virtual void vf38();
    virtual void vf39();
    virtual void vf40();
    virtual void vf41();
    virtual void vf42();
    virtual bool vf45();
    u8 m_pad4[8];
    u8* m_entries;
    u8 m_pad10[0x1c];
    SubObj* m_sub;
    u8 m_pad30[4];
    u8 m_flags;
};

extern "C" void fn_27_27E54C(SelfT* self, s32 entryId, s32 arg2) {
    if (self->vf45()) {
        if ((self->m_flags >> 7) & 1) {
            if (self->m_sub != 0) {
                int playerNo = g_ftManager->getPlayerNo(entryId);
                u8* entry = self->m_entries + playerNo * 0x5c;
                if (!((entry[0xb3] >> 2) & 1)) {
                    self->m_sub->vf33(entry[0x9a], arg2);
                }
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_27E600 {
class ftManager {
public:
    int getPlayerNo(int entryId);
};

extern ftManager* g_ftManager;

class SubObj {
public:
    u8 pad[0x3c];
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void vf5();
    virtual void vf6();
    virtual void vf7();
    virtual void vf8();
    virtual void vf9();
    virtual void vf10();
    virtual void vf11();
    virtual void vf12();
    virtual void vf13();
    virtual void vf14();
    virtual void vf15();
    virtual void vf16();
    virtual void vf17();
    virtual void vf18();
    virtual void vf19();
    virtual void vf20();
    virtual void vf21();
    virtual void vf22();
    virtual void vf23();
    virtual void vf24();
    virtual void vf25();
    virtual void vf26();
    virtual void vf27();
    virtual void vf28();
    virtual void vf29();
    virtual void vf30();
    virtual void vf31();
    virtual void vf32(u8, s32);
};

class SelfT {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void vf5();
    virtual void vf6();
    virtual void vf7();
    virtual void vf8();
    virtual void vf9();
    virtual void vf10();
    virtual void vf11();
    virtual void vf12();
    virtual void vf13();
    virtual void vf14();
    virtual void vf15();
    virtual void vf16();
    virtual void vf17();
    virtual void vf18();
    virtual void vf19();
    virtual void vf20();
    virtual void vf21();
    virtual void vf22();
    virtual void vf23();
    virtual void vf24();
    virtual void vf25();
    virtual void vf26();
    virtual void vf27();
    virtual void vf28();
    virtual void vf29();
    virtual void vf30();
    virtual void vf31();
    virtual void vf32();
    virtual void vf33();
    virtual void vf34();
    virtual void vf35();
    virtual void vf36();
    virtual void vf37();
    virtual void vf38();
    virtual void vf39();
    virtual void vf40();
    virtual void vf41();
    virtual void vf42();
    virtual bool vf43();
    u8 m_pad4[8];
    u8* m_entries;
    u8 m_pad10[0x1c];
    SubObj* m_sub;
    u8 m_pad30[4];
    u8 m_flags;
};

extern "C" void fn_27_27E600(SelfT* self, s32 entryId, s32 arg) {
    if (self->vf43()) {
        if ((self->m_flags >> 7) & 1) {
            if (self->m_sub != 0) {
                int playerNo = g_ftManager->getPlayerNo(entryId);
                u8* entry = self->m_entries + playerNo * 0x5c;
                self->m_sub->vf32(entry[0x9a], arg);
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_27E950 {
struct ftManager;

extern "C" {
extern ftManager* g_ftManager;
int getPlayerNo__9ftManagerFi(ftManager*, int);
}

struct TargetBase {
    unsigned char padding[0x3C];
};

struct Target : TargetBase {
    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual void unused3();
    virtual void unused4();
    virtual void unused5();
    virtual void unused6();
    virtual void unused7();
    virtual void unused8();
    virtual void unused9();
    virtual void unused10();
    virtual void unused11();
    virtual void unused12();
    virtual void unused13();
    virtual void unused14();
    virtual void unused15();
    virtual void unused16();
    virtual void unused17();
    virtual void unused18();
    virtual void unused19();
    virtual void unused20();
    virtual void unused21();
    virtual void unused22();
    virtual void unused23();
    virtual void unused24();
    virtual void unused25();
    virtual void unused26();
    virtual void unused27();
    virtual void unused28();
    virtual void unused29();
    virtual void unused30();
    virtual void unused31();
    virtual void unused32();
    virtual void unused33();
    virtual void unused34();
    virtual void unused35();
    virtual void unused36();
    virtual void setEnabled(int);
};

struct Player {
    unsigned char padding[0x5A];
    unsigned char enabled;
    unsigned char end;
};

struct PlayerData {
    unsigned char padding[0x40];
    Player players[4];
};

struct State {
    unsigned char padding0[0x0C];
    PlayerData* players;
    unsigned char padding1[0x20];
    Target* target;
};

extern "C" void fn_27_27E950(State* self, int entryId) {
    if (self->target) {
        int playerNo = getPlayerNo__9ftManagerFi(g_ftManager, entryId);
        if (self->players->players[playerNo].enabled) {
            self->target->setEnabled(1);
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_27E9C0 {
struct ftManager;

extern "C" {
extern ftManager* g_ftManager;
int getPlayerNo__9ftManagerFi(ftManager*, int);
}

class Target {
    char pad[0x3C];

public:
    virtual void dummy0();
    virtual void dummy1();
    virtual void dummy2();
    virtual void dummy3();
    virtual void dummy4();
    virtual void dummy5();
    virtual void dummy6();
    virtual void dummy7();
    virtual void dummy8();
    virtual void dummy9();
    virtual void dummy10();
    virtual void dummy11();
    virtual void dummy12();
    virtual void dummy13();
    virtual void dummy14();
    virtual void dummy15();
    virtual void dummy16();
    virtual void dummy17();
    virtual void dummy18();
    virtual void dummy19();
    virtual void dummy20();
    virtual void dummy21();
    virtual void dummy22();
    virtual void dummy23();
    virtual void dummy24();
    virtual void dummy25();
    virtual void dummy26();
    virtual void dummy27();
    virtual void dummy28();
    virtual void dummy29();
    virtual void dummy30();
    virtual void dummy31();
    virtual void dummy32();
    virtual void dummy33();
    virtual void dummy34();
    virtual void dummy35();
    virtual void dummy36();
    virtual void invoke(int);
};

struct Entry {
    char pad[0x5A];
    unsigned char flag;
    char pad2;
};

struct Data {
    char pad[0x40];
    Entry entries[4];
};

struct State {
    char pad0[0xC];
    Data* data;
    char pad1[0x20];
    Target* target;
};

extern "C" void fn_27_27E9C0(State* self, int entryId) {
    if (self->target) {
        int playerNo = getPlayerNo__9ftManagerFi(g_ftManager, entryId);
        if (self->data->entries[playerNo].flag) {
            Target* target = self->target;
            target->invoke(-1);
        }
    }
}
}

