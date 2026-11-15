#pragma once

#include <StaticAssert.h>
#include <nw4r/g3d/g3d_anmscn.h>
#include <nw4r/g3d/g3d_scnobjcallback.h>
#include <types.h>

// The per-frame overrides of a stage's scene animation lights and fogs (sora_melee st_scn_obj_call_back.o). Each set
// keeps flag bits of what was changed since the scene root last applied it.

struct stAmbLightParamSet {
    u32 m_flags;   // 0x0, 1: color
    float m_color[4];
};
static_assert(sizeof(stAmbLightParamSet) == 0x14, "Class is wrong size!");

class stDifLightParamSet {
public:
    u32 m_flags;   // 0x0, 1: color, 2: position, 4: direction
    float m_color[4];
    float m_pos[3];
    float m_dir[3];

    stDifLightParamSet();
};
static_assert(sizeof(stDifLightParamSet) == 0x2C, "Class is wrong size!");

class stSpecLightParamSet {
public:
    u8 m_data[0x20];

    stSpecLightParamSet();
};
static_assert(sizeof(stSpecLightParamSet) == 0x20, "Class is wrong size!");

struct stFogColor {
    float m_r;
    float m_g;
    float m_b;
    float m_a;
};

struct stFogParamSet {
    u32 m_flags;      // 0x0, 1: type, 2: start/end, 4: 0x14 pair, 8: color
    int m_fogIndex;   // 0x4, the scene root fog it overrides
    int m_type;       // 0x8
    float m_start;    // 0xC
    float m_end;      // 0x10
    float unk14;      // 0x14
    float unk18;      // 0x18
    stFogColor m_color; // 0x1C
};
static_assert(sizeof(stFogParamSet) == 0x2C, "Class is wrong size!");

class stScnObjCallBack : public nw4r::g3d::IScnObjCallback {
public:
    u32 m_ambLightNum;              // 0x4
    u32 m_difLightNum;              // 0x8
    u32 m_specLightNum;             // 0xC
    u32 m_fogNum;                   // 0x10
    stAmbLightParamSet* m_ambLights; // 0x14
    stDifLightParamSet* m_difLights; // 0x18
    stSpecLightParamSet* m_specLights; // 0x1C
    stFogParamSet* m_fogs;          // 0x20

    stScnObjCallBack();
    virtual ~stScnObjCallBack();
    virtual void ExecCallback_CALC_WORLD(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param, void* arg);
    virtual void SetCallBackCondition(nw4r::g3d::ScnObj* obj);

    void initialize(nw4r::g3d::ResAnmScn* scn);
    void setFogType(int index, int type);
    void setFogStartEnd(int index, float start, float end);
    void setFogColor(int index, const stFogColor* color);
    void addFog(const stFogParamSet* fogs);
};
static_assert(sizeof(stScnObjCallBack) == 0x24, "Class is wrong size!");
