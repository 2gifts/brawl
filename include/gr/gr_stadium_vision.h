#pragma once

#include <StaticAssert.h>
#include <gr/gr_stadium.h>
#include <types.h>

// The big screen of Pokemon Stadium 2 ("stadium vision"). Only the part the stage reaches into is described; the class
// lives in sora_melee (create at 0x27AAF8, setDisplay at 0x27ADF8).
// HYPOTHESIS: every field name below; the offsets are the ones stDxPStadium writes/reads and the constructor sets. The
// base is grStadium (the constructor and destructor call grStadium's).
class Message;

class grStadiumVision : public grStadium {
public:
    void* m_miscData;           // 0x15C: archive "Misc" entry 0x28 (set by the stage)
    u8 m_isDisplay : 1;         // 0x160, 0x80: the screen shows its text (setDisplay)
    u8 m_flag6 : 1;
    u8 m_flag5 : 1;
    u8 m_flag4 : 1;
    u8 m_flags : 4;
    char _161[3];
    int m_screenKind;           // 0x164: which picture the screen shows (0 none, 4 normal, 8 grass, 9 fire, 0xA water, 0xB rock)
    u8 m_screenMode;            // 0x168: 0xD / 0xE for the two "player view" modes, otherwise the vision kind
    char _169[3];               // 0x169
    float m_unk16C;             // 0x16C
    Message* m_message;         // 0x170: five text buffers drawn on the screen nodes
    float m_unk174;             // 0x174
    float m_unk178;             // 0x178
    float m_unk17C;             // 0x17C
    float m_unk180;             // 0x180
    float m_unk184;             // 0x184
    float m_unk188;             // 0x188
    float m_unk18C;             // 0x18C
    s16 m_unk190;               // 0x190
    s16 m_unk192;               // 0x192
    s16 m_unk194;               // 0x194
    s16 m_unk196;               // 0x196
    s16 m_unk198;               // 0x198
    s16 m_unk19A;               // 0x19A
    s16 m_unk19C;               // 0x19C
    u8 m_unk19E;                // 0x19E
    char _19F;

    grStadiumVision(const char* taskName);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void update(float deltaFrame);
    virtual ~grStadiumVision();

    static grStadiumVision* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
    void setDisplay(bool display);
};
static_assert(sizeof(grStadiumVision) == 0x1A0, "Class is wrong size!");
