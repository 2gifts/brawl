#pragma once

#include <StaticAssert.h>
#include <it/it_create.h>
#include <st/loader/st_loader_player.h>
#include <types.h>

class BaseItem;

class stLoaderSandBag : public stLoaderPlayer {
public:
    BaseItem* m_item;
    // HYPOTHESIS: passed to entryInfo and gates the player HUD appear/disappear.
    bool unk64;
    itKind m_itemKind;

    // HYPOTHESIS: the player id type follows stLoaderPlayer::m_playerId.
    stLoaderSandBag(u32 playerId);

    virtual void processBegin();
    virtual ~stLoaderSandBag();
    virtual void entryEntity();
    virtual void entryInfo(bool);
    virtual void removeEntity();
    virtual void lockEntity();
    virtual void unlockEntity();
    virtual void entryEntitySuddenDeath();
    virtual void entryEntityRebirth();
    virtual bool isEntry();
    virtual void setSuddenDeath();
    virtual void removeInfo();
};
static_assert(sizeof(stLoaderSandBag) == 0x6C, "Class is wrong size!");

// HYPOTHESIS: the argument block of IfPlayer::appear (main.dol 0x800E08A0); local name, fields unknown except where
// the loader fills them from the player init data.
struct stLoaderSandBagAppearInfo {
    int unk0;
    int m_state;
    int m_characterKind;
    u8 unkC;
    u8 m_colorFileNo;
    s8 unkE;
    int unk10;
    int m_damage;
    bool m_isStamina;
    int unk1C;
    int m_teamNo;
    u8 unk24;
    u8 unk25;
    int unk28;
    int unk2C[4];
    bool unk3C;
    int unk40;

    stLoaderSandBagAppearInfo()
        : unk0(0), m_state(0), m_characterKind(0), unkC(0), m_colorFileNo(0), unkE(-1), unk10(0), m_damage(0),
          m_isStamina(false), unk1C(3), m_teamNo(0), unk24(0), unk25(0), unk28(0), unk3C(true), unk40(0) {
        unk2C[0] = 0;
        unk2C[1] = 0;
        unk2C[2] = 0;
        unk2C[3] = 0;
    }
};
static_assert(sizeof(stLoaderSandBagAppearInfo) == 0x44, "Class is wrong size!");

// MATCH-ONLY: gmPlayerInitData read as the original does: the character kind as an unsigned byte, and the flag byte at
// 0x1B whose bit below m_isMetal the shared header leaves unnamed.
struct stLoaderSandBagInitDataView {
    u8 m_characterKind;
    char _1[0x1A];
    bool : 1;
    bool : 1;
    bool : 1;
    bool m_isMetal : 1;
    bool unk1B_3 : 1;
};

// HYPOTHESIS: two eight-byte constant objects, (0xff, 0) and (0xff, 1), that the original builds as file-local
// statics in the loader units; nothing reads them, they only make the static initializer of each file.
struct stLoaderSandBagMarker {
    int m_a;
    int m_b;
    stLoaderSandBagMarker(int a, int b) : m_a(a), m_b(b) { }
};
