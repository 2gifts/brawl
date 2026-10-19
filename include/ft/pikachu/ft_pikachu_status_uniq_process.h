#pragma once

#include <so/status/so_status_module_impl.h>
#include <types.h>

class soModuleAccesser;

// Work variable ids used by the status processes of Pikachu. HYPOTHESIS: names follow how each id is read and written.
struct ftPikachuWork {
    enum Int {
        QuickAttack_Count = 0x20000000,     // incremented whenever a Quick Attack ends
        QuickAttack_StartFrames = 0x20000002, // counted down while the first dash is in progress
        QuickAttack_EndState = 0x20000004,
        QuickAttack_WarpTimer = 0x10000001,
    };
    enum Float {
        QuickAttack_DirectionX = 0x21000004, // direction of the warp, also used as the speed scale by the transactor
        QuickAttack_DirectionY = 0x21000005,
        QuickAttack_ExitSpeedY = 0x11000000,
        QuickAttack_ExitSpeedX = 0x11000001,
    };
    enum Flag {
        QuickAttack_Ended = 0x12000003,
    };
};

// Status kinds of Pikachu's specials that are only known by number. HYPOTHESIS: names are placeholders.
struct ftPikachuStatus {
    enum Kind {
        QuickAttack_Unk116 = 0x116,
        QuickAttack_Unk117 = 0x117,
        QuickAttack_Unk118 = 0x118,
    };
};

// The ExtendParam group (ftData::extendParam[2]) read by Quick Attack. HYPOTHESIS: names follow the use.
struct ftPikachuExtendParamQuickAttack {
    int unk00;      // frames of the first dash
    int unk04;
    float unk08;    // gravity of the first dash
    float unk0C;
    float unk10;
    float unk14;
    float unk18;
    float unk1C;
    float unk20;
    float unk24;
    float unk28;
    float unk2C;
    float unk30;
    float unk34;
    float unk38;
    float unk3C;
    float unk40;
    int unk44;
    float unk48;
    int unk4C;
    float unk50;
    float unk54;
};

// Quick Attack: first dash (Start), warp (Warp) and stop (End).
class ftPikachuStatusUniqProcessSpecialQuickAttackStart : public soStatusUniqProcess {
public:
    virtual ~ftPikachuStatusUniqProcessSpecialQuickAttackStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftPikachuStatusUniqProcessSpecialQuickAttackWarp : public soStatusUniqProcess {
public:
    virtual ~ftPikachuStatusUniqProcessSpecialQuickAttackWarp() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

class ftPikachuStatusUniqProcessSpecialQuickAttackEnd : public soStatusUniqProcess {
public:
    virtual ~ftPikachuStatusUniqProcessSpecialQuickAttackEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

extern ftPikachuStatusUniqProcessSpecialQuickAttackStart g_ftPikachuStatusUniqProcessSpecialQuickAttackStart;
extern ftPikachuStatusUniqProcessSpecialQuickAttackWarp g_ftPikachuStatusUniqProcessSpecialQuickAttackWarp;
extern ftPikachuStatusUniqProcessSpecialQuickAttackEnd g_ftPikachuStatusUniqProcessSpecialQuickAttackEnd;
