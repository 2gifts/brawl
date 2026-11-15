#pragma once

#include <types.h>

class stEventRule {
public:
    // HYPOTHESIS: constructor takes a flag; layout of the base members is guessed.
    stEventRule(int flag);
    virtual ~stEventRule();
    // HYPOTHESIS: the entry ID and player index are forwarded by stOperatorRuleMelee.
    // HYPOTHESIS: returns the flag byte as is (no bool normalization in the original).
    virtual u8 notifyEventDead(int entryId, u32 playerIndex);
    virtual void notifyEventStartStage();
    virtual void notifyEventStartGame();
    // HYPOTHESIS: the argument is the signed corps member index.
    virtual void notifyEventDeadCorpsMember(int index);
    virtual void notifyEventYoshiEgg();
    virtual void notifyEventPikminBloom();
    // Called every frame; true once the event is decided (unk8/unkC receive the outcome, as Stage::isEventEnd writes them).
    virtual bool chechEventRule();

    // HYPOTHESIS: what notifyEventDead reports for every fighter death.
    u8 unk4;
    // HYPOTHESIS: the event state and decision handed to Stage::isEventEnd.
    int unk8;
    int unkC;
};

// Event rules with a decision of their own.
class stEventRuleStageEnd : public stEventRule {
public:
    stEventRuleStageEnd();
    virtual ~stEventRuleStageEnd();
    virtual bool chechEventRule();
};

class stEventRuleCoinGetter : public stEventRule {
public:
    stEventRuleCoinGetter();
    virtual ~stEventRuleCoinGetter();
    virtual bool chechEventRule();

    // HYPOTHESIS: the per-player record block, the team mode flag and the coin goal.
    void* m_players; // 0x10
    int m_teamMode;  // 0x14
    int m_goal;      // 0x18
};

class stEventRuleCorpsOrder : public stEventRule {
public:
    stEventRuleCorpsOrder();
    virtual ~stEventRuleCorpsOrder();
    virtual void notifyEventDeadCorpsMember(int index);
    virtual bool chechEventRule();

    int m_nextIndex;  // 0x10: the member expected to fall next
    bool m_outOfOrder; // 0x14
};

class stEventRuleDeepDungeon : public stEventRule {
public:
    stEventRuleDeepDungeon();
    virtual ~stEventRuleDeepDungeon();
    virtual bool chechEventRule();
};

class stEventRulePoweredSuit : public stEventRule {
public:
    stEventRulePoweredSuit();
    virtual ~stEventRulePoweredSuit();
    virtual bool chechEventRule();

    u32 unk10;
    int unk14;
};

class stEventRuleEvent40 : public stEventRule {
public:
    stEventRuleEvent40();
    virtual ~stEventRuleEvent40();
    virtual u8 notifyEventDead(int entryId, u32 playerIndex);
    virtual bool chechEventRule();
};

class stEventRuleYoshiEgg : public stEventRule {
public:
    stEventRuleYoshiEgg();
    virtual ~stEventRuleYoshiEgg();
    virtual void notifyEventYoshiEgg();
    virtual bool chechEventRule();

    int m_count; // 0x10: two eggs decide the event
};

class stEventRuleDeadReason : public stEventRule {
public:
    stEventRuleDeadReason(int reason);
    virtual ~stEventRuleDeadReason();
    virtual u8 notifyEventDead(int entryId, u32 playerIndex);
    virtual bool chechEventRule();

    int m_state; // 0x10: 1 once the death matched the reason
};

class stEventRulePikminBloom : public stEventRule {
public:
    stEventRulePikminBloom();
    virtual ~stEventRulePikminBloom();
    virtual void notifyEventPikminBloom();
    virtual bool chechEventRule();

    bool m_bloomed; // 0x10
};

class stEventRuleDemoCamera : public stEventRule {
public:
    stEventRuleDemoCamera();
    virtual ~stEventRuleDemoCamera();
    virtual void notifyEventStartGame();
    virtual bool chechEventRule();

    bool m_started;  // 0x10
    int m_order[4];  // 0x18: the camera cuts in a shuffled order
    u32 unk28;
    int unk2C;
};

class stEventRuleFoxSetFinal : public stEventRule {
public:
    stEventRuleFoxSetFinal();
    virtual ~stEventRuleFoxSetFinal();
    virtual void notifyEventStartGame();
};
