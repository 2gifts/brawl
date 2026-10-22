#pragma once
// Local BrawlHeaders shadow: adds the non-virtual player-cursor, pad-mask and lock methods of
// stOperatorInfo (sora_melee st_operator_info.o) with the unsigned per-player arrays they use.

#include <StaticAssert.h>
#include <gm/gm_lib.h>
#include <st/loader/st_loader_player.h>
#include <st/operator/st_operator.h>
#include <types.h>

class stOperatorInfo : public stOperator {
public:
    stLoaderPlayer* m_loaderPlayers[MAX_PLAYERS];
    u8 m_playersCursorStatus[MAX_PLAYERS];
    u8 m_playersNameCursorStatus[MAX_PLAYERS];
    u8 m_72[MAX_PLAYERS];
    s32 m_7c[MAX_PLAYERS];
    float m_98[MAX_PLAYERS];
    u8 m_playersLoupeStatus[MAX_PLAYERS];
    u8 m_bb;
    u8 m_bc;
    u32 m_c0;
    u8 m_c4;
    u32 m_c8;
    u32 m_cc;
    char _d0[0x1C];
    s32 m_ec[8];
    u8 m_10c;
    float m_110;
    s32 m_114;
    bool m_118;
    bool m_119;
    bool m_11a;

    stOperatorInfo(const char* name);
    virtual void processBegin();
    virtual void processEnd();
    virtual ~stOperatorInfo();
    virtual void startOperator();
    virtual void stopOperatorRequest();
    virtual bool isEnableOperation();
    virtual u32 getEndType();
    virtual u32 getErrorLevel();
    virtual bool isError();
    virtual void setPlayerCursorClear(u8 index);
    // HYPOTHESIS: unsigned status (the setters compare it unsigned against the stored byte).
    virtual void setPlayerCursorStatus(u8 index, u8 status);
    virtual void setPlayerNameCursorStatus(u8 index, u8 status);
    virtual void setPlayerLoupeStatus(u8 index, u8 status);
    virtual void updatePlayerCursor(bool);

    // HYPOTHESIS: returns the local pause trigger only (the pad number output is discarded).
    static int getPauseTriggerAndPadNo();
    void setLocalPlayerPadMask();
    void setLocalPlayerPadMaskWifi();
    void lockPlayer();
    void unlockPlayer();
    void clearPlayerCursor();
    void checkOnamaeForcePut();
    void startEventCursor();
};
static_assert(sizeof(stOperatorInfo) == 0x11c, "Class is wrong size!");
