#pragma once

#include <StaticAssert.h>
#include <ft/ft_manager.h>
#include <types.h>
#include <gm/gm_global_record.h>

class gmGlobalModeMelee;
class stOperatorInfo;
class stOperatorRule;

class mlFtEvent : public ftOutsideEventObserver {
public:
    // HYPOTHESIS: the match settings, set from g_GameGlobal->m_modeMelee and by setCondition.
    gmGlobalModeMelee* m_condition;
    char unk10[0x1C];
    // HYPOTHESIS: the match operators that receive the cursor/loupe states and the knockouts.
    stOperatorInfo* m_operatorInfo;
    stOperatorRule* m_operatorRule;
    bool unk34_80 : 1;
    bool unk34_40 : 1;
    bool unk34_20 : 1;
    bool unk34_10 : 1;
    u8 unk34_0f : 4;
    u32 unk38;

    static mlFtEvent* create();
    mlFtEvent();

    int getInfoAreaNo(int playerNo);
    // HYPOTHESIS: maps the damage of one hit to a controller rumble rate.
    float getDamageInfoRumbleRate(float damage);
    // HYPOTHESIS: parameter type from the field it sets.
    void setCondition(gmGlobalModeMelee* condition);

    virtual void notifyEventOnDamage(int entryId, u32 hp, soDamage* damage);
    virtual void notifyEventSetDamage(int entryId, float, u32 percent, bool, bool);
    virtual void notifyEventRecover(int entryId, int);
    virtual void notifyEventBeat(int entryId1, int entryId2);
    virtual void notifyEventSuicide(int entryId);
    virtual void notifyEventChangeStart(int entryId, int playerNo, int activeInstanceIndex, ftKind);
    virtual void notifyEventChangeEnd(int entryId, int playerNo, int activeInstanceIndex, ftKind);
    virtual void notifyEventAddDragoonParts(int entryId, int);
    virtual void notifyEventCompDragoonParts(int entryId);
    virtual void notifyEventRemoveDragoonParts(int entryId, int);
    virtual void notifyEventResetDragoonParts(int entryId);
    virtual void notifyEventReEntryRequestFighter();
    virtual void notifyEventRemoveEntry(int entryId);
    virtual void notifyEventFinalSlow(int entryId, float, int);
    virtual void notifyEventFinalSlowCancel(int entryId);
    virtual void notifyEventFinalStop(int entryId);
    virtual void notifyEventFinalStopCancel(int entryId);
    virtual void notifyEventGetItem(int entryId, itKind kind, int itVariation, int genParamId, int instanceId);
    virtual void notifyEventSucceedHit(int entryId, u32 consecutiveHits, float totalDamage);
    virtual void notifyEventPikminMakeBloomAll(int entryId);
    virtual void notifyEventOnInput(int entryId);
    virtual void notifyEventAppeal(int entryId, int appeal);
    virtual void notifyEventKnockout(int entryId);
    virtual void notifyEventHeartSwapStart(int entryId1, int entryId2);
    virtual void notifyEventHeartSwapEnd(int, int);
    virtual ~mlFtEvent();
    virtual void destroy();
    virtual bool isInfoManager();
};
static_assert(sizeof(mlFtEvent) == 0x3C, "Class is wrong size!");

class mlFtEventResult : public mlFtEvent {
public:
    virtual ~mlFtEventResult();
    virtual void notifyEventResultWin(int entryId, int winAnim);
    virtual bool isInfoManager();
};

class mlFtEventTraining : public mlFtEvent {
public:
    // HYPOTHESIS: the damage the training HUD shows, set by notifyEventOnDamage.
    int m_damage;
    int unk40;
    int unk44;
    gmGlobalRecord::CharaNumData* unk48;
    int unk4C;

    virtual void notifyEventOnDamage(int entryId, u32 hp, soDamage* damage);
    virtual void notifyEventSucceedHit(int entryId, u32 consecutiveHits, float totalDamage);
    virtual ~mlFtEventTraining();
};
static_assert(sizeof(mlFtEventTraining) == 0x50, "Class is wrong size!");
