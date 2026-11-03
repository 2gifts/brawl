#pragma once

#include <types.h>

class stEventRule {
public:
    virtual ~stEventRule();
    // HYPOTHESIS: the entry ID and player index are forwarded by stOperatorRuleMelee.
    virtual bool notifyEventDead(int entryId, u32 playerIndex);
    virtual void notifyEventStartStage();
    virtual void notifyEventStartGame();
    // HYPOTHESIS: the argument is the signed corps member index.
    virtual void notifyEventDeadCorpsMember(int index);
    virtual void notifyEventYoshiEgg();
    virtual void notifyEventPikminBloom();
};
