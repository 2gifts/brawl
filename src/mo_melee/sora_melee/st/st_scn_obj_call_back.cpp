#include <memory.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <st/st_scn_obj_call_back.h>
#include <string.h>
#include <types.h>

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
