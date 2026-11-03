#pragma once

#include <types.h>

// HYPOTHESIS: parameter types of the WiFi upload evaluator that mlFtEvent forwards damage and KO events to.
class nteUpdateEvaluator {
public:
    static nteUpdateEvaluator* getInstance();
    void notifyEventOnDamage(int playerNo, u32 hp);
    void notifyEventBeat(int playerNo1, int playerNo2);
};
