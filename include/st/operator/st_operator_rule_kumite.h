#pragma once

#include <st/operator/st_operator_rule_melee.h>
#include <types.h>

class stOperatorRuleKumite : public stOperatorRuleMelee {
public:
    u8 unk25C[4];
    // HYPOTHESIS: list length, current index, and character bytes used by createRandomMyCharaList.
    int unk260;
    int unk264;
    u8 unk268[0x40];

    void createRandomMyCharaList(u32);

    // HYPOTHESIS: forwards the stOperatorRuleMelee constructor argument.
    stOperatorRuleKumite(u8);
    virtual ~stOperatorRuleKumite();
    virtual void calcScore();
    virtual bool isSuddenDeath();
    virtual bool notifyEventDead(int entryId, u32 playerIndex, int);
    virtual void notifyEventStartStage();
    virtual void notifyEventSetupCorps();
    virtual void calcHighScore();
    virtual bool getNextCorpsPutMemberCharaKind(u8*, u8*);
    virtual void putCorpsPutMember(void*, u32 characterKind, u32 costumeId, u8, bool);
};
