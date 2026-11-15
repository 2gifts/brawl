#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <types.h>

class ftAudience {
public:
    virtual ~ftAudience() { }
    virtual void activate();
    virtual void deactivate();
    virtual void reqHandclapDead(float, bool);
    virtual float correctCheerReaction(float, float);
    virtual void reqCallFgmStopCheer();
    virtual void checkCheer(float, int entryId1, int entryId2);
    virtual bool checkTumultRange(float);
    virtual bool checkTumultLanding(float, float, int entryId);
    virtual bool checkTumultFallSpecial(float, int entryId);
    virtual void checkCheerDefeat(int entryId);
};
static_assert(sizeof(ftAudience) == 4, "Class is wrong size!");

// HYPOTHESIS: the audience parameter block (only the fields read so far are named).
struct ftAudienceData {
    float m_handclapDamage0; // 0x0
    float m_handclapDamage1; // 0x4
    float m_handclapDamage2; // 0x8
    float m_cheerReactionMin; // 0xC
    float m_cheerReactionMax; // 0x10
    float m_cheerReactionMul; // 0x14
    float m_comboFrame; // 0x18
    int m_callDamage; // 0x1C
    int m_tumultInterval; // 0x20
    int m_callCountMin; // 0x24
    int m_callCountMax; // 0x28
    float m_tumultEdgeMargin; // 0x2C
    float m_fallHeight2; // 0x30
    float m_fallHeight1; // 0x34
    float m_fallHeightMin; // 0x38
    int m_tumultFallCount; // 0x3C
    float m_tumultFallHeight; // 0x40
};

class ftAudienceImpl : public gfTask, public ftAudience {
public:
    ftAudienceData* m_data; // 0x44
    int m_48;
    int m_frame; // 0x4C
    float m_50;
    int m_callEntryId; // 0x54
    int m_tumultTimer; // 0x58
    u32 m_callSeId; // 0x5C
    int m_callCount; // 0x60
    int m_fallCount; // 0x64
    s32 m_cheerDefeatSe; // 0x68
    s32 m_handclapSe; // 0x6C
    s32 m_tumultSe; // 0x70
    s32 m_callSe; // 0x74
    u32 m_soundGroup; // 0x78
    bool m_isActive; // 0x7C
    bool m_7d;
    bool m_7e;

    ftAudienceImpl();
    virtual void processFixCamera();
    virtual ~ftAudienceImpl();

    virtual void activate();
    virtual void deactivate();
    virtual void reqHandclapDead(float, bool);
    virtual float correctCheerReaction(float, float);
    virtual void reqCallFgmStopCheer();
    virtual void checkCheer(float, int entryId1, int entryId2);
    virtual bool checkTumultRange(float);
    virtual bool checkTumultLanding(float, float, int entryId);
    virtual bool checkTumultFallSpecial(float, int entryId);
    virtual void checkCheerDefeat(int entryId);

    void updateCall();
    void updateTumult();
    bool checkCall(float damage, int entryId);
};
static_assert(sizeof(ftAudienceImpl) == 128, "Class is wrong size!");
