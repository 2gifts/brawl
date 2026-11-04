#pragma once

#include <types.h>

// HYPOTHESIS: parameter types of the WiFi upload evaluator that mlFtEvent forwards damage and KO events to.
class nteUpdateEvaluator {
public:
    nteUpdateEvaluator() { reset(); }
    ~nteUpdateEvaluator() { }
    static nteUpdateEvaluator* getInstance();
    static nteUpdateEvaluator* createInstance();
    static void dropInstance();
    void reset();
    void DamagePostProcess();
    void notifyEventOnDamage(int playerNo, u32 hp);
    void notifyEventBeat(int playerNo1, int playerNo2);

private:
    u32 unk0;
    int unk4;
    u32 unk8;
    int unkC;
    int unk10;
    u32 unk14[4];
    u32 unk24[4];
};
