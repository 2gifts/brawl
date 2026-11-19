#include <gf/gf_task_scheduler.h>
#include <gm/gm_global.h>
#include <st/operator/st_operator_appearance_helper.h>
#include <st/operator/st_operator_bgm.h>
#include <st/operator/st_operator_rule_adventure.h>
#include <st/stage.h>
#include <types.h>

stOperatorRuleAdventure::stOperatorRuleAdventure()
    : unkCC(0.0f), unkD4(false), unkD5(false), unkD6(false), unkD7(false),
      unkD8(false), unkDC(150), unkE0(0), unkE4(120), unkE8(0), unkEC(0),
      unkF0(240), unkF4(-1), unkF8(NULL) {
    m_state = 0;
    unk2C_b1 = false;
}

bool stOperatorRuleAdventure::isEnd() {
    bool end = false;
    if (m_state == 12) {
        end = true;
    }
    gmAdvData* data = g_GameGlobal->m_advData;
    if (!end && data->m_saveRequest == 2) {
        if (g_Stage && !g_Stage->isNextStepBgmEqualNowStepBgmFromFlag()) {
            static_cast<stOperatorBgm*>(m_operatorBgm)->requestStop();
        }
        end = true;
    }
    if (end) {
        gfTaskScheduler::getInstance()->setPauseCategory(gfTask::Category_None, false);
        gfTaskScheduler::getInstance()->setPauseCategory(gfTask::Category_Fighter, false);
        gfTaskScheduler::getInstance()->setPauseCategory(gfTask::Category_Enemy, false);
        gfTaskScheduler::getInstance()->setPauseCategory(gfTask::Category_Weapon, false);
    }
    return end;
}

void stOperatorRuleAdventure::initEndProduction(int kind) {
    unkE0 = 0;
    unkE8 = 0;
    switch (kind) {
    case 0:
        unkD5 = false;
        unkD6 = false;
        break;
    case 1:
        unkD5 = true;
        unkE4 = 0;
        unkDC = 60;
        unkD6 = false;
        break;
    case 2:
        unkD5 = true;
        unkE4 = 120;
        unkDC = 150;
        unkD6 = true;
        unkEC = 0;
        unkF0 = 240;
        break;
    case 3:
        unkD5 = true;
        unkE4 = 60;
        unkDC = 120;
        unkD6 = false;
        break;
    }
    if (unkF8) {
        static_cast<stOperatorAppearanceHelper*>(unkF8)->endOperator();
    }
}

stOperatorRuleAdventure::~stOperatorRuleAdventure() { }

void stOperatorRuleAdventure::setOpAppearanceHelper(void* helper) {
    unkF8 = helper;
}

#include <st/st_trigger.h>
#include <snd/snd_system.h>
// TODO: clean up, give variables real names
namespace jj_25203C {
typedef unsigned char u8;
typedef unsigned long u32;

struct State {
    char pad0[0x40];
    u8 mode;
    char pad41[0x8B];
    float current;
    float limit;
    bool enabled;
};
struct Flags { u8 a : 1; u8 b : 1; u8 rest : 6; };
struct Game {
    char pad0[0x604];
    u8 mode;
    u8 reset;
    char pad606[0x16];
    u32 count;
    char pad620[0x4304];
    Flags flags;
};
struct Global { char pad0[0x30]; Game* game; };
struct Adv {
    char pad0[0x48];
    u8 enabled;
    char pad49[3];
    u32 count;
};
struct Interface { char pad0[0x44]; void* timer; };
struct Stage_base { char pad[0x3c]; };
class Stage : public Stage_base {
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
    virtual ~Stage();
    virtual void createObj();
    virtual void createObjPokeTrainer();
    virtual void getPokeTrainerPointNum();
    virtual void getPokeTrainerPointData();
    virtual void getPokeTrainerPositionZ();
    virtual void getPokeTrainerDrawLayer();
    virtual void isAdventureStage();
    virtual void getItemPac();
    virtual void getItemGenPac();
    virtual void getItemPacEnemyFigure();
    virtual void getEnemyPac();
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
};
extern "C" {
extern Global* g_GameGlobal;
extern Adv* g_IfAdvMngr;
extern Interface* g_IfMngr;
extern Stage* g_Stage;
extern float lbl_27_rodata_40C8;
void fn_800DDCF8(void*, u32);
void* getInstance__13adKeepManagerFv();
int getGlobalFlag__13adKeepManagerFUl(void*, int);
void setGlobalFlag__13adKeepManagerFUlb(void*, int, int);
}
union AdvFlag {
    u8 byte;
    struct { u8 enabled : 1; u8 rest : 7; } bits;
};
extern "C" void fn_27_25203C(State* self) {
    Game* game = g_GameGlobal->game;
    game->reset = 0;
    if (game->flags.a) {
        self->current = lbl_27_rodata_40C8;
        self->limit = (float)(unsigned int)game->count;
        self->enabled = 1;
    } else if (game->flags.b) {
        self->current = (float)game->count;
        self->limit = (float)game->count;
        self->enabled = 1;
    } else if (game->count != 0) {
        self->current = (float)game->count;
        self->enabled = 1;
    }
    if (g_IfAdvMngr) {
        g_IfAdvMngr->count = (u32)self->current;
        Adv* adv = g_IfAdvMngr;
        AdvFlag flag;
        flag.byte = adv->enabled;
        flag.bits.enabled = self->enabled;
        adv->enabled = flag.byte;
    }
    if (g_IfMngr) {
        fn_800DDCF8(g_IfMngr->timer, (u32)self->limit);
    }
    if (g_Stage) {
        g_Stage->notifyTimmingGameStart();
    }
    if (game->flags.b) {
        self->mode = 3;
    } else if (game->flags.a) {
        if (game->mode == 2) self->mode = 1;
        else self->mode = 3;
    } else {
        if (getGlobalFlag__13adKeepManagerFUl(getInstance__13adKeepManagerFv(), 1002) == 0) self->mode = 1;
        else self->mode = 3;
    }
    setGlobalFlag__13adKeepManagerFUlb(getInstance__13adKeepManagerFv(), 1002, 0);
}
}

// TODO: clean up, give variables real names
namespace jj_25223C {

extern "C" {
extern void* g_GameGlobal;
extern f32 lbl_27_rodata_40C8;
void initEndProduction__23stOperatorRuleAdventureFi(u8*, s32);
void* fn_27_232304(const void*, int, u32);
}

inline bool checkTrigger(u8* obj) {
    if (g_stTriggerMng != NULL) {
        u8* node = (u8*)fn_27_232304(g_stTriggerMng, 0x3e8, 0);
        if (node != NULL && node[0x2c] != 0) {
            obj[0x605] = 1;
            return true;
        }
    }
    return false;
}

inline bool setTrigger(u8* obj) {
    u8 flags = obj[0x607];
    if (((flags >> 5) & 1) && ((flags >> 4) & 1)) {
        stTriggerData data;
        *(u16*)&data = 0x3e8;
        *((u8*)&data + 2) = 1;
        *((u8*)&data + 3) = 0;
        g_stTriggerMng->setTriggerFlag(&data);
        obj[0x605] = 1;
        return true;
    }
    return false;
}

extern "C" s32 fn_27_25223C(u8* arg0) {
    u8* obj = *(u8**)((u8*)g_GameGlobal + 0x30);
    switch (obj[0x604]) {
    case 0:
    case 4:
        if (obj[0x603] == 8) {
            initEndProduction__23stOperatorRuleAdventureFi(arg0, 0);
            *(f32*)(arg0 + 0xcc) = lbl_27_rodata_40C8;
            return 1;
        }
        break;
    case 1:
        if (checkTrigger(obj)) {
            g_sndSystem->playSE((SndID)0x1a, -1, 0, 0, -1);
            initEndProduction__23stOperatorRuleAdventureFi(arg0, 2);
            *(f32*)(arg0 + 0xcc) = lbl_27_rodata_40C8;
            return 1;
        }
        break;
    case 2:
        if (checkTrigger(obj)) {
            initEndProduction__23stOperatorRuleAdventureFi(arg0, 0);
            *(f32*)(arg0 + 0xcc) = lbl_27_rodata_40C8;
            return 1;
        }
        break;
    case 3:
        if (setTrigger(obj)) {
            g_sndSystem->playSE((SndID)0x1a, -1, 0, 0, -1);
            initEndProduction__23stOperatorRuleAdventureFi(arg0, 2);
            *(f32*)(arg0 + 0xcc) = lbl_27_rodata_40C8;
            return 1;
        }
        break;
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_25246C {
extern "C" {
extern u8* g_GameGlobal;

int fn_27_25246C() {
    u8* p = *(u8**)(g_GameGlobal + 0x30);
    u8 raw = p[0x5fa];
    if ((s8)raw < 0) {
        return 0;
    }

    int r6 = 0;
    if (((p[0x4924] >> 7) & 1) != 0 &&
        ((p[0x606] >> 7) & 1) != 0) {
        if ((s8)raw != 0) {
            goto end;
        }
        if (((p[0x606] >> 6) & 1) != 0) {
            goto inner_set;
        }
        if (((p[0x606] >> 5) & 1) == 0) {
            goto end;
        }
inner_set:
        r6 = 1;
        goto end;
    }
    if ((s8)raw != 0) {
        goto end;
    }
    if (((p[0x606] >> 6) & 1) == 0) {
        goto end;
    }
    r6 = 1;
end:
    if (r6 != 0) {
        p[0x605] = 3;
        return 1;
    }
    return 0;
}
}
}

