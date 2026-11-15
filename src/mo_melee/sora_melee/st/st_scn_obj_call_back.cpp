#include <memory.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <nw4r/g3d/g3d_scnroot.h>
#include <revolution/GX/GXTypes.h>
#include <st/st_scn_obj_call_back.h>
#include <string.h>
#include <types.h>

namespace nw4r {
namespace g3d {
// HYPOTHESIS: the parts of the nw4r light object and fog data the callback touches (the shared headers do not describe
// them; this unit does not include them).
class LightObj {
public:
    void InitLightColor(GXColor color);
    void InitLightPos(f32 x, f32 y, f32 z);
    void InitLightDir(f32 x, f32 y, f32 z);

    u8 _0[0x44];
};

struct FogData {
    int m_type;      // 0x0
    f32 m_startZ;    // 0x4
    f32 m_endZ;      // 0x8
    f32 m_nearZ;     // 0xC
    f32 m_farZ;      // 0x10
    GXColor m_color; // 0x14
};
} // namespace g3d
} // namespace nw4r

// HYPOTHESIS: the light setting of the scene root (offsets are the words the callback reads).
struct stScnRootLightView {
    u8 _0[0x2878];
    u16 m_lightObjNum;                    // 0x2878
    u8 _287A[2];
    nw4r::g3d::LightObj* m_lightObjs;     // 0x287C
    GXColor* m_ambColors;                 // 0x2880
};

void ScnObj_EnableCallbackExecOp(nw4r::g3d::ScnObj*, u32);
void ScnObj_EnableCallbackTiming(nw4r::g3d::ScnObj*, u32);

stScnObjCallBack::stScnObjCallBack() {
    m_ambLightNum = 0;
    m_difLightNum = 0;
    m_specLightNum = 0;
    m_fogNum = 0;
    m_ambLights = NULL;
    m_difLights = NULL;
    m_specLights = NULL;
    m_fogs = NULL;
}

stScnObjCallBack::~stScnObjCallBack() {
    if (m_ambLights != NULL) {
        delete[] m_ambLights;
    }
    m_ambLights = NULL;
    m_ambLightNum = 0;
    if (m_difLights != NULL) {
        delete[] m_difLights;
    }
    m_difLights = NULL;
    m_difLightNum = 0;
    if (m_specLights != NULL) {
        delete[] m_specLights;
    }
    m_specLights = NULL;
    m_specLightNum = 0;
    if (m_fogs != NULL) {
        delete[] m_fogs;
    }
    m_fogs = NULL;
    m_fogNum = 0;
}

void stScnObjCallBack::initialize(nw4r::g3d::ResAnmScn* scn) {
    u32 fogNum, difLightNum, specLightNum, ambLightNum;
    fogNum = scn->GetResAnmFogNumEntries();
    specLightNum = scn->ptr()->m_specLightNum;
    difLightNum = scn->GetResAnmLightNumEntries();
    ambLightNum = scn->GetResAnmAmbLightNumEntries();
    m_ambLightNum = ambLightNum;
    m_ambLights = new (Heaps::StageResource) stAmbLightParamSet[ambLightNum];
    m_difLightNum = difLightNum;
    m_difLights = new (Heaps::StageResource) stDifLightParamSet[difLightNum];
    m_specLightNum = specLightNum;
    m_specLights = new (Heaps::StageResource) stSpecLightParamSet[specLightNum];
    m_fogNum = fogNum;
    m_fogs = new (Heaps::StageResource) stFogParamSet[fogNum];
    memset(m_ambLights, 0, ambLightNum * sizeof(stAmbLightParamSet));
    memset(m_difLights, 0, difLightNum * sizeof(stDifLightParamSet));
    memset(m_specLights, 0, specLightNum * sizeof(stSpecLightParamSet));
    memset(m_fogs, 0, fogNum * sizeof(stFogParamSet));
}

void stScnObjCallBack::SetCallBackCondition(nw4r::g3d::ScnObj* obj) {
    ScnObj_EnableCallbackTiming(obj, 1);
    ScnObj_EnableCallbackExecOp(obj, 1);
}

void stScnObjCallBack::setFogType(int index, int type) {
    if (type == m_fogs[index].m_type) {
        return;
    }
    m_fogs[index].m_flags |= 1;
    m_fogs[index].m_type = type;
}

void stScnObjCallBack::setFogStartEnd(int index, float start, float end) {
    if ((start != m_fogs[index].m_start) | (end != m_fogs[index].m_end)) {
        m_fogs[index].m_flags |= 2;
        m_fogs[index].m_start = start;
        m_fogs[index].m_end = end;
    }
}

// HYPOTHESIS: an inline inequality of the color (the result goes through a bool).
static inline bool stFogColorDiffers(const stFogColor& a, stFogColor b) {
    return (a.m_r != b.m_r) | (a.m_g != b.m_g) | (a.m_b != b.m_b) | (a.m_a != b.m_a);
}

void stScnObjCallBack::setFogColor(int index, const stFogColor* color) {
    stFogParamSet* fog = &m_fogs[index];
    if (stFogColorDiffers(fog->m_color, *color)) {
        fog->m_flags |= 8;
        m_fogs[index].m_color = *color;
    }
}

void stScnObjCallBack::addFog(const stFogParamSet* fogs) {
    for (u32 i = 0; i < m_fogNum; i++, fogs++) {
        m_fogs[i].m_flags = 0;
        m_fogs[i].m_fogIndex = fogs->m_fogIndex;
        if (fogs->m_type != m_fogs[i].m_type) {
            m_fogs[i].m_flags |= 1;
            m_fogs[i].m_type = fogs->m_type;
        }
        float start = fogs->m_start;
        float end = fogs->m_end;
        if (((float)__fabs(start) > 0.0001f) | ((float)__fabs(end) > 0.0001f)) {
            m_fogs[i].m_flags |= 2;
            m_fogs[i].m_start += start;
            m_fogs[i].m_end += end;
        }
        float x = fogs->unk14;
        float y = fogs->unk18;
        if (((float)__fabs(x) > 0.0001f) | ((float)__fabs(y) > 0.0001f)) {
            m_fogs[i].m_flags |= 4;
            m_fogs[i].unk14 += x;
            m_fogs[i].unk18 += y;
        }
        if (((float)__fabs(fogs->m_color.m_r) > 0.0001f) | ((float)__fabs(fogs->m_color.m_g) > 0.0001f) |
            ((float)__fabs(fogs->m_color.m_b) > 0.0001f) | ((float)__fabs(fogs->m_color.m_a) > 0.0001f))
        {
            m_fogs[i].m_flags |= 8;
            stFogColor add = fogs->m_color;
            stFogParamSet* fog = &m_fogs[i];
            fog->m_color.m_r += add.m_r;
            fog->m_color.m_g += add.m_g;
            fog->m_color.m_b += add.m_b;
            fog->m_color.m_a += add.m_a;
        }
    }
}

void stScnObjCallBack::ExecCallback_CALC_WORLD(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param, void* arg) {
    if (obj != NULL && timing == nw4r::g3d::ScnObj::CALLBACK_TIMING_A) {
        stScnRootLightView* root = reinterpret_cast<stScnRootLightView*>(obj);
        u32 lightObjNum = root->m_lightObjNum;
        GXColor* ambColor = root->m_ambColors;
        for (u32 i = 0; i < m_ambLightNum; i++, ambColor++) {
            if (i < lightObjNum) {
                if (m_ambLights[i].m_flags & 1) {
                    ambColor->r = m_ambLights[i].m_color[0];
                    ambColor->g = m_ambLights[i].m_color[1];
                    ambColor->b = m_ambLights[i].m_color[2];
                    ambColor->a = m_ambLights[i].m_color[3];
                }
                m_ambLights[i].m_flags = 0;
            }
        }
        nw4r::g3d::LightObj* light = root->m_lightObjs;
        for (u32 i = 0; i < m_difLightNum; i++, light++) {
            if (i < lightObjNum) {
                if (m_difLights[i].m_flags & 1) {
                    GXColor color = { (u8)m_difLights[i].m_color[0], (u8)m_difLights[i].m_color[1], (u8)m_difLights[i].m_color[2],
                                      (u8)m_difLights[i].m_color[3] };
                    light->InitLightColor(color);
                }
                if (m_difLights[i].m_flags & 2) {
                    light->InitLightPos(m_difLights[i].m_pos[0], m_difLights[i].m_pos[1], m_difLights[i].m_pos[2]);
                }
                if (m_difLights[i].m_flags & 4) {
                    light->InitLightDir(m_difLights[i].m_dir[0], m_difLights[i].m_dir[1], m_difLights[i].m_dir[2]);
                }
                m_difLights[i].m_flags = 0;
            }
        }
        nw4r::g3d::ScnRoot* scnRoot = reinterpret_cast<nw4r::g3d::ScnRoot*>(obj);
        for (u32 i = 0; i < m_fogNum; i++) {
            nw4r::g3d::FogData* fog = scnRoot->GetFog(m_fogs[i].m_fogIndex).ptr();
            if ((m_fogs[i].m_flags & 1) && fog != NULL) {
                fog->m_type = m_fogs[i].m_type;
            }
            if (m_fogs[i].m_flags & 2) {
                if (m_fogs[i].m_start >= m_fogs[i].m_end) {
                    m_fogs[i].m_start = m_fogs[i].m_end - 1.0f;
                }
                if (fog != NULL) {
                    fog->m_startZ = m_fogs[i].m_start;
                    fog->m_endZ = m_fogs[i].m_end;
                }
            }
            if ((m_fogs[i].m_flags & 4) && fog != NULL) {
                fog->m_nearZ = m_fogs[i].unk14;
                fog->m_farZ = m_fogs[i].unk18;
            }
            if (m_fogs[i].m_flags & 8) {
                GXColor color = { (u8)m_fogs[i].m_color.m_r, (u8)m_fogs[i].m_color.m_g, (u8)m_fogs[i].m_color.m_b,
                                  (u8)m_fogs[i].m_color.m_a };
                if (fog != NULL) {
                    fog->m_color = color;
                }
            }
        }
    }
}
