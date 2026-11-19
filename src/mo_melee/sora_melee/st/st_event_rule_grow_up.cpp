#include <types.h>
#include <st/st_event_rule_grow_up.h>

extern "C" u32 lbl_27_data_54290;

stEventRuleGrowUp::stEventRuleGrowUp(int param) : stEventRule(1), m_param(param), m_value(lbl_27_data_54290), m_count(0) {
    m_value = lbl_27_data_54290;
    m_count = 0;
    unk8 = 0;
}

stEventRuleGrowUp::~stEventRuleGrowUp() {
}

#include <ft/ft_manager.h>
#include <revolution/OS/OSError.h>
// TODO: clean up, give variables real names
namespace jj_25D54C {
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
    virtual void isEnd();
    virtual bool isEventEnd(int, int*, int*);
};
extern "C" {
extern Stage* g_Stage;
extern char lbl_27_data_54290[];
void setInfiniteScaling__9ftManagerFiQ27Fighter11ScalingKindQ27Fighter11ScalingType(ftManager*, int, int, int);
void setFighterOperationCpuType__9ftManagerFii(ftManager*, int, int);
}
struct State { char pad[8]; int value; int other; int kind; int data; int state; };
extern "C" int fn_27_25D54C(State* self) {
    char* d = lbl_27_data_54290;
    int id;
    switch (self->state) {
    case 0:
        self->data = *(int*)(d + 4);
        self->state = 0x34;
        goto a;
    la:
        if (g_Stage) g_Stage->isEventEnd(0, &self->value, &self->other);
        self->data = *(int*)(d + 8);
        self->state = 0x3d;
        break;
    case 0x34:
    case 0x3d:
    a:
        if (self->value == 0) goto la;
        OSReport(d + 0x14);
        if (self->kind == 0x28) {
            id = g_ftManager->getEntryId(0);
            if (g_ftManager->isValidEntryId(id)) setInfiniteScaling__9ftManagerFiQ27Fighter11ScalingKindQ27Fighter11ScalingType(g_ftManager,id,0,-1);
            id = g_ftManager->getEntryId(2);
            if (g_ftManager->isValidEntryId(id)) setInfiniteScaling__9ftManagerFiQ27Fighter11ScalingKindQ27Fighter11ScalingType(g_ftManager,id,0,-1);
            id = g_ftManager->getEntryId(3);
            if (g_ftManager->isValidEntryId(id)) setInfiniteScaling__9ftManagerFiQ27Fighter11ScalingKindQ27Fighter11ScalingType(g_ftManager,id,0,-1);
        }
        goto b;
    lb:
        if (g_Stage) g_Stage->isEventEnd(0, &self->value, &self->other);
        self->data = *(int*)(d + 12);
        self->state = 0x4f;
        break;
    case 0x4f:
    b:
        if (self->value == 1) goto lb;
        OSReport(d + 0x44);
        if (self->kind == 0x28) {
            id = g_ftManager->getEntryId(0);
            if (g_ftManager->isValidEntryId(id)) setInfiniteScaling__9ftManagerFiQ27Fighter11ScalingKindQ27Fighter11ScalingType(g_ftManager,id,2,0);
            id = g_ftManager->getEntryId(2);
            if (g_ftManager->isValidEntryId(id)) setInfiniteScaling__9ftManagerFiQ27Fighter11ScalingKindQ27Fighter11ScalingType(g_ftManager,id,1,1);
            id = g_ftManager->getEntryId(3);
            if (g_ftManager->isValidEntryId(id)) setInfiniteScaling__9ftManagerFiQ27Fighter11ScalingKindQ27Fighter11ScalingType(g_ftManager,id,1,1);
        }
        if (self->kind == 0x26) {
            id = g_ftManager->getEntryId(1);
            if (id != -1) setFighterOperationCpuType__9ftManagerFii(g_ftManager,id,0x1c);
        }
        goto c;
    lc:
        if (g_Stage) g_Stage->isEventEnd(0, &self->value, &self->other);
        self->data = *(int*)(d + 16);
        self->state = 0x6a;
        break;
    case 0x6a:
    c:
        if (self->value == 2) goto lc;
        OSReport(d + 0x78);
        if (self->kind == 0x28) {
            id = g_ftManager->getEntryId(0);
            if (g_ftManager->isValidEntryId(id)) setInfiniteScaling__9ftManagerFiQ27Fighter11ScalingKindQ27Fighter11ScalingType(g_ftManager,id,1,1);
            id = g_ftManager->getEntryId(2);
            if (g_ftManager->isValidEntryId(id)) setInfiniteScaling__9ftManagerFiQ27Fighter11ScalingKindQ27Fighter11ScalingType(g_ftManager,id,2,0);
            id = g_ftManager->getEntryId(3);
            if (g_ftManager->isValidEntryId(id)) setInfiniteScaling__9ftManagerFiQ27Fighter11ScalingKindQ27Fighter11ScalingType(g_ftManager,id,2,0);
        }
        goto a;
    }
    return 0;
}
}

