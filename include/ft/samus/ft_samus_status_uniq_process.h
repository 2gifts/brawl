#pragma once

#include <so/status/so_status_module_impl.h>
#include <types.h>

class soModuleAccesser;

// Work variable ids used by the status processes of Samus. HYPOTHESIS: names come from how each id is read and written.
struct ftSamusWork {
    enum Flag {
        SpecialHi_Launched = 0x22000010,
        SpecialHi_Released = 0x22000011,
        SpecialHi_Grounded = 0x22000012,
    };
    enum Float {
        SpecialHi_ExitLandingLag = 0x11000000,
        SpecialHi_ExitControlXRatio = 0x11000001,
        SpecialLw_Angle = 0x11000013,
    };
    // The down special (Bomb). HYPOTHESIS: names follow the use.
    enum LwFlag {
        SpecialLw_GoAir = 0x22000010,       // the ground status hands over to the air status (0x11e)
        SpecialLw_ControlActive = 0x22000011, // the controller energy (type 10) steers the move
        SpecialLw_BrakeActive = 0x22000012,
        SpecialLw_LandCheck = 0x22000013,   // stick down lands the move (status 0x12)
        SpecialLw_DropBomb = 0x22000014,    // a bomb (article 1) is generated and shot
        SpecialLw_Setup = 0x22000015,
        SpecialLw_InitPending = 0x1200003e, // the common set-up still has to run
    };
    // The Charge Shot. HYPOTHESIS: names follow the use.
    enum SFlag {
        SpecialS_Shoot = 0x22000010,      // the shot article is generated and shot
        SpecialS_Charged = 0x22000011,
        SpecialS_Setup = 0x22000012,
        SpecialS_InitPending = 0x1200003e,
    };
    enum SInt {
        SpecialS_ChargeMin = 0x5a04,      // constant compared with the charge variable below
        SpecialS_ChargeVariable = 0x520b,
    };
    enum LwInt {
        SpecialLw_ItemVisibility = 0x10000043,
    };
};

// Status kinds of the down special that are only known by number. HYPOTHESIS: names are placeholders for the order.
struct ftSamusStatus {
    enum Kind {
        SpecialLw_Unk11E = 0x11e,
        SpecialLw_Unk120 = 0x120,
        SpecialLw_Unk121 = 0x121,
    };
};

// Ids of the ExtendParam constants (ftSamusExtendParamAccesser, base id 3999 + float index). HYPOTHESIS: names follow the use.
struct ftSamusParam {
    enum Float {
        SpecialHi_ControlXRatio = 0xfa7,
        SpecialHi_StickThreshold = 0xfa8,
        SpecialHi_LandingLag = 0xfa9,
    };
};

// The ExtendParam group (ftData::extendParam[1]) read directly by the Screw Attack. HYPOTHESIS: names follow the use.
struct ftSamusExtendParamSpecialHi {
    float riseSpeedX;       // multiplied by the facing direction when the attack starts
    float controlAccelMul;  // set on the controller energy of the air launch
    float controlSpeedXMax; // HYPOTHESIS: horizontal speed target/limit of the controller energy in the air
    float riseSpeedY;       // vertical speed of the air launch
};

// The ExtendParam group (ftData::extendParam[2]) read directly by the Bomb. HYPOTHESIS: names follow the use.
struct ftSamusExtendParamSpecialLw {
    float jumpSpeedY;       // 0x00: vertical speed when the ground status hands over to the air status
    float unk04;            // 0x04: vertical speed of the air entry
    float controlSpeedXMax; // 0x08: horizontal speed target factor of the controller energy
    float unk0C;            // 0x0c
    float controlAccelMul;  // 0x10: acceleration multiplier of the controller energy
    float unk14;            // 0x14
    float groundSpeedXMul;  // 0x18: horizontal speed factor when entered on the ground
    float airSpeedXMul;     // 0x1c: horizontal speed factor when entered in the air
    float unk20[3];         // 0x20..0x2b
    float landStickY;       // 0x2c: stick deflection below which the move is landed
    float unk30[2];         // 0x30..0x37
    float launchSpeed;      // 0x38: speed of the twin status (0x121), turned by the angle work float
    float unk3C;            // 0x3c
    float airEndSpeed;      // 0x40: acceleration/speed factors of the twin status (0x121)
};

// MATCH-ONLY: soArticle stays incomplete here; its virtual isNull() (slot 2) is called through this view.
class ftSamusArticleNullView {
public:
    virtual bool isNull() const;
};

// The ExtendParam group (ftData::extendParam[3]) read directly by the Charge Shot. HYPOTHESIS: names follow the use.
struct ftSamusExtendParamSpecialS {
    float unk00;
    float speedXDivisor; // the horizontal speed the move was started with is divided by it
    float airBrakeX;     // horizontal brake of the stop energy in the air
};

// Status kinds of the Charge Shot that are only known by number. HYPOTHESIS: the even kinds are the ground/air starts and
// the odd kinds the air twins; 0x124 and up shoot the second article.
struct ftSamusSpecialSStatus {
    enum Kind {
        Unk122 = 0x122,
        Unk123 = 0x123,
        Unk124 = 0x124,
        Unk125 = 0x125,
    };
};

// Screw Attack.
class ftSamusStatusUniqProcessSpecialHi : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessSpecialHi() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

// Screw Attack's ground-to-air twin of the down special (Bomb): ground version.
class ftSamusStatusUniqProcessSpecialGroundLw : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessSpecialGroundLw() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

class ftSamusStatusUniqProcessSpecialAirLw : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessSpecialAirLw() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

// Charge Shot.
class ftSamusStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

// Zero Laser.
class ftSamusStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

extern ftSamusStatusUniqProcessSpecialHi g_ftSamusStatusUniqProcessSpecialHi;
extern ftSamusStatusUniqProcessSpecialGroundLw g_ftSamusStatusUniqProcessSpecialGroundLw;
extern ftSamusStatusUniqProcessSpecialAirLw g_ftSamusStatusUniqProcessSpecialAirLw;
extern ftSamusStatusUniqProcessSpecialS g_ftSamusStatusUniqProcessSpecialS;
extern ftSamusStatusUniqProcessFinal g_ftSamusStatusUniqProcessFinal;
