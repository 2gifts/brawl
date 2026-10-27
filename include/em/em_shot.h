#pragma once

// The shot helpers the enemies' article activators share: the shot description an enemy reads from its parameters, the
// work block the helpers fill, and the helpers themselves. They live in the Enemy unit, which is not reconstructed yet,
// so the helpers keep their address names.

#include <em/em_scaled_value.h>
#include <mt/mt_vector.h>
#include <so/so_module_accesser.h>
#include <types.h>

class Enemy;
class wnemSimple;
class wnemSwHolder;

// HYPOTHESIS: field roles from the enemy activators that read them; the description comes from the enemy's constant
// parameters.
struct emShotParam {
    u8 unk0[0x8];
    s32 m_type;            // 0x8, 2 selects the second launch position and the flag of fn_41_47544
    u32 m_shotKind;        // 0xC, Primid: 1/2 boomerang shots, 4 a plain wnemSimple shot; Spar: 0/2 aimed shots
    u32 m_life;            // 0x10, overwritten from a scaled constant when m_lifeConstantId names one
    float m_speed;         // 0x14
    u8 unk18[0x44];
    float m_flySpeedY;     // 0x5C, HYPOTHESIS: the Hammer Bro's hammer takes it as its vertical speed
    u8 unk60[0x14];
    s32 m_lifeConstantId;  // 0x74, an indefinite constant id; only ids above ConstantId_CommonLast name one
};

// The indefinite constant block named by emShotParam::m_lifeConstantId.
// HYPOTHESIS: only the scaled lifetime at 0x18 is read.
struct emShotLifeConstant {
    u8 unk0[0x18];
    emScaledValue m_life;  // 0x18
};

// A flag derived from the shot description (true when its m_type is 2).
struct emShotFlag {
    bool m_value;
    emShotFlag() : m_value(false) { }
};

// The work block of the shot helpers (0x34 bytes of the Enemy unit's bss).
// HYPOTHESIS: field roles from the activators' writes.
struct emShotWork {
    emShotParam* m_param;
    u8 unk4[0x2];
    bool m_unk6;           // cleared by the boomerang Primid before fn_41_4768C
    u8 unk7;
    void* m_unk8;          // handed to fn_41_4768C by the boomerang Primid
    u8 unkC[0x4];
    float m_randomAngle;   // 0x10, Botron: a random angle in [0, 90) degrees
    u8 unk14[0x14];
    void* m_constantParam; // 0x28, Botron: an indefinite constant of the shot
    u8 unk2C[0x8];
};
extern emShotWork lbl_41_bss_504;

// The launch description of an enemy weapon, at +0x4 of the shot work; Enemy::shoot / addParts reset it before they
// generate the article (the reset is a weak copy of the Enemy unit).
// HYPOTHESIS: field roles; unk4 is overwritten with the third argument of Enemy::shoot.
// TODO(layout): embed it in emShotWork (whose unk4..m_constantParam overlap it) and move the shot users onto its fields.
struct emWeaponParamWork {
    bool unk0;
    bool unk1;
    bool unk2;
    int unk4;
    float unk8[7];
    int unk24;
    void* m_addressPack; // 0x28, the enemy's last anim cmd address pack
    void reset() {
        unk0 = false;
        unk1 = false;
        unk2 = true;
        unk24 = 0;
        unk4 = -1;
        unk8[0] = unk8[1] = unk8[2] = 0.0f;
        unk8[3] = unk8[4] = 0.0f;
        unk8[5] = unk8[6] = 0.0f;
        m_addressPack = NULL;
    }
};
inline emWeaponParamWork* getWeaponParamWork() {
    return reinterpret_cast<emWeaponParamWork*>(&lbl_41_bss_504.unk4);
}

// HYPOTHESIS: the work int holds the parameter id of the shot being fired (it is handed straight to fn_41_473DC).
static const int WorkInt_ShotParamId = 0x10000007;
// HYPOTHESIS: an optional constant id of the shot (Botron).
static const int WorkInt_ShotConstantId = 0x10000001;
// HYPOTHESIS: the x and y of the point an aimed shot flies at (Spar, Kokkon).
static const int WorkFloat_ShotTargetX = 0x1100000a;
static const int WorkFloat_ShotTargetY = 0x1100000b;
// ValueFloat_LevelScale (em/em_scaled_value.h): the scaled shot lifetimes grow with it, and the Primid hands it to its
// plain shot as the life argument of the wnemSimple activation.
// The last constant id that names no indefinite constant block: only larger ids are read with getConstantIndefinite.
// The activators compare with > (signed); >= 44000 compiles to a different compare.
static const s32 ConstantId_CommonLast = 43999;

extern "C" {
// Reads the shot description of the given parameter id into the work block and returns it.
emShotParam* fn_41_473DC(soModuleAccesser* acc, int paramId);
// The launch position of the shot.
Vec3f fn_41_47470(emShotParam* param, soModuleAccesser* acc);
void fn_41_47544(emShotFlag* flag, emShotParam* param);
void fn_41_4768C(void* target, emShotParam* param, soModuleAccesser* acc, bool, int);
void fn_41_47A2C(emShotParam* param, soModuleAccesser* acc, bool);
// Aims the shot from the launch position at the target point (a normalized direction in the xy plane).
void fn_41_47C64(emShotParam* param, soModuleAccesser* acc, Vec3f* from, Vec3f* to, int);
// Launches a wnemSwHolder / wnemSimple article from the launch position.
void fn_41_47D8C(Vec3f* pos, bool flag, wnemSwHolder* article, emShotParam* param, soModuleAccesser* acc);
void fn_41_47E80(Vec3f* pos, bool flag, wnemSimple* article, emShotParam* param, soModuleAccesser* acc);
// wnemSimple's activation.
void fn_41_3091C(wnemSimple* article, emShotWork* work, int founderTaskId, int ownerId, int, bool flag, Vec3f* pos,
                 int team, float lr, float speed, int, u32 life, s16);
}
