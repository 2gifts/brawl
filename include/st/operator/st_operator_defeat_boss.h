#pragma once

// HYPOTHESIS: the enum names (after the map names of the phase functions they select) and the field names.

#include <StaticAssert.h>
#include <ef/ef_screen_handle.h>
#include <st/operator/st_operator.h>
#include <st/stage.h>
#include <types.h>

class stOperatorDefeatBoss : public stOperator {
public:
    enum Phase {
        Phase_DefeatbossIdle = 0x0,
        Phase_Init = 0x1,
        Phase_FreezeInit = 0x2,
        Phase_FreezeUpdate = 0x3,
        Phase_FreezeEnd = 0x4,
        Phase_SlowInit = 0x5,
        Phase_SlowUpdate = 0x6,
        Phase_SlowEnd = 0x7,
        Phase_LostInit = 0x8,
        Phase_LostUpdate = 0x9,
        Phase_LostEnd = 0xA,
        Phase_FadeInit = 0xB,
        Phase_FadeUpdate = 0xC,
        Phase_FadeEnd = 0xD,
        Phase_Done = 0xE,
    };

    int m_phase;                              // 0x48
    u32 m_frame;                              // 0x4C
    bool unk50;                               // 0x50 HYPOTHESIS: a trigger of enemy kind 0x3A (Tabuu) exists
    bool unk51;                               // 0x51
    efScreenHandle m_screenHandle;            // 0x52
    stDestroyBossParamCommon m_param;         // 0x54

    virtual void processBegin();
    virtual ~stOperatorDefeatBoss();

    void startOperator();
    void phaseDefeatbossIdle();
    void phaseInit();
    void phaseFreezeInit();
    void phaseFreezeUpdate();
    void phaseFreezeEnd();
    void phaseSlowInit();
    void phaseSlowUpdate();
    void phaseSlowEnd();
    void phaseLostInit();
    void phaseLostUpdate();
    void phaseLostEnd();
    void phaseFadeInit();
    void phaseFadeUpdate();
    void phaseFadeEnd();
    void phaseDone();
};
static_assert(sizeof(stOperatorDefeatBoss) == 0x6C, "Class is wrong size!");
