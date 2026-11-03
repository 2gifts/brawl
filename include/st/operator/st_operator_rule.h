#pragma once

// Supersedes include/lib/BrawlHeaders/Brawl/Include/st/operator/st_operator_rule.h (shadows it via -I include).
// Adds exchangeGmCharacterKind2FigureId (sora_melee 0x24810C), which the BrawlHeaders submodule lacks. Keep in sync.
// Also adds the replay-save helpers and the fighter-kind table lookup of st_operator_rule.o, and defines the
// virtual accessors inline: the original emits them after the out-of-line members in reverse declaration order.

#include <StaticAssert.h>
#include <gm/gm_lib.h>
#include <st/operator/st_operator.h>
#include <types.h>

class stEventRule;

class stOperatorRule : public stOperator {
public:
    enum DecisionKind {
        Decision_Timeup = 0x0,
        Decision_Gameset = 0x1,
        Decision_Complete = 0x2,
        Decision_Failure = 0x3,
        Decision_Success = 0x4
    };

    // HYPOTHESIS: maps a fighter kind to the id of its figure (trophy); the result is used as the figure item's variation.
    static u32 exchangeGmCharacterKind2FigureId(gmCharacterKind kind);
    // HYPOTHESIS: same table as exchangeGmCharacterKind2FigureId; 0x23 for kinds outside it.
    static u32 exchangeGmCharacterKindToMuCharKind(gmCharacterKind kind);

    bool canSaveReplay();
    void startSaveReplay();
    void checkDispCorps();

    void* m_operatorBgm;
    void* m_operatorNetwork;
    void* m_operatorController;
    union {
        char _20[32];
        struct {
            stEventRule* unk54;
            char unk58[0x1C];
        };
    };
    union {
        struct {
            bool m_isGameSet;
            bool unk75;
            char _76[0x3E];
        };
        struct {
            u8 unk74[4];
            u32 unk78;
            int unk7C[4];
            float unk8C[4];
            char unk9C[0x18];
        };
    };
    u32 unkB4;
    u32 unkB8;
    bool unkBC;
    char _BD[0xF];

    stOperatorRule();
    virtual void processBegin();
    virtual ~stOperatorRule();
    virtual void setOperatorBgm(void* bgm) { m_operatorBgm = bgm; }
    virtual void setOperatorNetwork(void* network) { m_operatorNetwork = network; }
    virtual void setOperatorController(void* controller) { m_operatorController = controller; }
    virtual bool isGameSet() { return m_isGameSet; }
    virtual bool isEnd();
    virtual void startOperator();
    virtual void calcScore();
    virtual bool isSuddenDeath();
    virtual void adjustResultInfo() { }
    virtual void notifyEventKnockout(u32);
    virtual bool notifyEventDead(int entryId, u32 playerIndex, int);
    virtual void notifyEventStartStage();
    virtual void notifyEventSetupCorps();
    virtual void notifyEventBeat(u32 playerIndex1, u32 playerIndex2);
    virtual void notifyEventYoshiEgg();
    virtual void notifyEventPikminBloom();
    virtual void nortifyNetworkOtherNodeExit();
    virtual void networkWriteRanking() { }
    virtual void setOpAppearanceHelper(void*) { }
};
static_assert(sizeof(stOperatorRule) == 204, "Class is wrong size!");
