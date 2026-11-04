#pragma once

#include <types.h>

class stEventRule {
public:
    // HYPOTHESIS: constructor takes a flag; layout of the base members is guessed.
    stEventRule(int flag);
    virtual ~stEventRule();
    // HYPOTHESIS: the entry ID and player index are forwarded by stOperatorRuleMelee.
    virtual bool notifyEventDead(int entryId, u32 playerIndex);
    virtual void notifyEventStartStage();
    virtual void notifyEventStartGame();
    // HYPOTHESIS: the argument is the signed corps member index.
    virtual void notifyEventDeadCorpsMember(int index);
    virtual void notifyEventYoshiEgg();
    virtual void notifyEventPikminBloom();

    // HYPOTHESIS: base members, only the one at 0x8 is written by derived rules.
    int unk4;
    int unk8;
    int unkC;
};
