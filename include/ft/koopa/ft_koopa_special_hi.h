#pragma once
#include <ft/ft_common_data_accesser.h>
#include <types.h>

// HYPOTHESIS: the Whirling Fortress (Special Hi) extend-parameter group of Bowser and Giga Bowser: the air rise speed,
// gravity and fall limit, the ground/air drift speed and acceleration multipliers, and four frame counts read when the
// air status ends.
struct ftKoopaSpecialHiParam {
    float unk0;
    float m_airSpeedY;
    float m_gravity;
    float m_fallSpeedMax;
    float m_groundSpeedX;
    float m_airSpeedX;
    float m_groundAccelMul;
    float m_airAccelMul;
    int unk20;
    int unk24;
    int unk28;
    int unk2C;
};

// Bowser keeps the group at index 1, Giga Bowser at index 2.
static inline ftKoopaSpecialHiParam* getKoopaSpecialHiParam(ftKind kind, int index) {
    return static_cast<ftKoopaSpecialHiParam*>(g_ftCommonDataAccesser.getData(kind)->extendParam[index]);
}

// True while a work flag is off. BYTES: the result of the inline helper is a compiler temporary, which is what gives the
// item visibility flag its early saved register in the Special Hi initStatus functions.
static inline bool isKoopaFlagOff(soWorkManageModule& work, u32 flag) {
    return !work.isFlag(flag);
}

// Requests a motion change. BYTES: the parameter block is built as the argument (a temporary placed after the earlier
// temporaries of the caller) before the motion module is fetched.
static inline void changeKoopaMotion(soModuleAccesser* moduleAccesser, const soMotionChangeParam& param) {
    moduleAccesser->getMotionModule().changeMotionRequest(const_cast<soMotionChangeParam*>(&param));
}
