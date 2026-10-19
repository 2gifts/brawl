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
    // Thunder. HYPOTHESIS: names follow the use.
    enum ThunderInt {
        Thunder_Situation = 0x20000002,       // situation kind the hit status last reacted to (-1 before the first update)
        Thunder_CloudHit = 0x20000003,
        Thunder_CloudPointer = 0x10000043,    // the cloud article, stored as an int
    };
    enum ThunderFlag {
        Thunder_CloudLogged = 0x22000012,     // the cloud holds a reference to the attack log of Pikachu
    };
    // Skull Bash. HYPOTHESIS: names follow the use.
    enum SkullBashInt {
        SkullBash_Frames = 0x20000000,        // charge frames, kept while the move is released
        SkullBash_ReleaseDamage = 0x20000001,
    };
    enum SkullBashFloat {
        SkullBash_ChargeRatio = 0x21000004,   // charge frames / the longest charge, limited to 0..1
    };
    enum SkullBashFlag {
        SkullBash_Charging = 0x22000010,
        SkullBash_Airborne = 0x22000012,
        SkullBash_PowerSet = 0x22000013,
        SkullBash_Stopped = 0x22000014,
    };
};

// Status kinds of Pikachu's specials that are only known by number. HYPOTHESIS: names are placeholders.
struct ftPikachuStatus {
    enum Kind {
        QuickAttack_Unk116 = 0x116,
        QuickAttack_Unk117 = 0x117,
        QuickAttack_Unk118 = 0x118,
        Thunder_Unk11D = 0x11d,
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

// Skull Bash: start (SkullBash), charge (Hold), the flash of the shortest charge (Weak), the dash (Attack) and the end.
class ftPikachuStatusUniqProcessSpecialSkullBash : public soStatusUniqProcess {
public:
    virtual ~ftPikachuStatusUniqProcessSpecialSkullBash() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

class ftPikachuStatusUniqProcessSpecialSkullBashHold : public soStatusUniqProcess {
public:
    virtual ~ftPikachuStatusUniqProcessSpecialSkullBashHold() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

class ftPikachuStatusUniqProcessSpecialSkullBashWeak : public soStatusUniqProcess {
public:
    virtual ~ftPikachuStatusUniqProcessSpecialSkullBashWeak() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

class ftPikachuStatusUniqProcessSpecialSkullBashAttack : public soStatusUniqProcess {
public:
    virtual ~ftPikachuStatusUniqProcessSpecialSkullBashAttack() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    static void stop(soModuleAccesser* moduleAccesser);
};

class ftPikachuStatusUniqProcessSpecialSkullBashEnd : public soStatusUniqProcess {
public:
    virtual ~ftPikachuStatusUniqProcessSpecialSkullBashEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

// Thunder: the bolt that is called down (Thunder) and the status entered when the cloud's bolt hits Pikachu (ThunderHit).
class ftPikachuStatusUniqProcessSpecialThunder : public soStatusUniqProcess {
public:
    virtual ~ftPikachuStatusUniqProcessSpecialThunder() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

class ftPikachuStatusUniqProcessSpecialThunderHit : public soStatusUniqProcess {
public:
    virtual ~ftPikachuStatusUniqProcessSpecialThunderHit() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    static void notifyReflect(soModuleAccesser* moduleAccesser);
    static void unref_log_group(soModuleAccesser* moduleAccesser);
};

extern ftPikachuStatusUniqProcessSpecialQuickAttackStart g_ftPikachuStatusUniqProcessSpecialQuickAttackStart;
extern ftPikachuStatusUniqProcessSpecialQuickAttackWarp g_ftPikachuStatusUniqProcessSpecialQuickAttackWarp;
extern ftPikachuStatusUniqProcessSpecialQuickAttackEnd g_ftPikachuStatusUniqProcessSpecialQuickAttackEnd;
extern ftPikachuStatusUniqProcessSpecialSkullBash g_ftPikachuStatusUniqProcessSpecialSkullBash;
extern ftPikachuStatusUniqProcessSpecialSkullBashHold g_ftPikachuStatusUniqProcessSpecialSkullBashHold;
extern ftPikachuStatusUniqProcessSpecialSkullBashWeak g_ftPikachuStatusUniqProcessSpecialSkullBashWeak;
extern ftPikachuStatusUniqProcessSpecialSkullBashAttack g_ftPikachuStatusUniqProcessSpecialSkullBashAttack;
extern ftPikachuStatusUniqProcessSpecialSkullBashEnd g_ftPikachuStatusUniqProcessSpecialSkullBashEnd;
extern ftPikachuStatusUniqProcessSpecialThunder g_ftPikachuStatusUniqProcessSpecialThunder;
extern ftPikachuStatusUniqProcessSpecialThunderHit g_ftPikachuStatusUniqProcessSpecialThunderHit;
