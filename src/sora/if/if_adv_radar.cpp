#include <gf/gf_3d_scene.h>
#include <gm/gm_global.h>
#include <if/if_adv_radar.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
IfAdvRadar* lbl_805A0348[2] = { 0 };

IfAdvRadarData::IfAdvRadarData(void* arg1, void* arg2, void* arg3, void* arg4) {
    unk0 = arg3;
    nw4r::g3d::ResFile::Init(this);
    unk4 = arg1;
}

IfAdvRadar* IfAdvRadar::create(IfAdvRadarData* data, int mode) {
    if ((g_GameGlobal->m_advData->m_itemGenKind & 0xFF000000) != 0x2A000000 && mode == 0) {
        return NULL;
    }
    return lbl_805A0348[0] = new (Heaps::InfoInstance) IfAdvRadar(data, mode);
}

inline IfAdvRadar::~IfAdvRadar() {
    lbl_805A0348[0] = NULL;
}

void IfAdvRadar::destroy() {
    delete lbl_805A0348[0];
    lbl_805A0348[0] = NULL;
}

void IfAdvRadar::setPlayerNum(int count) {
    for (int player = 0; (u32)player < 2; ++player) {
        bool visible = player < count && unk408_6;
        nw4r::g3d::ScnMdl* model = unkAC[player]->m_sceneModel;
        if (visible != (model->m_parent == unk4)) {
            if (visible) {
                unk4->Insert(unk4->sceneItemsCount, model);
            } else {
                unk4->Remove(model);
            }
        }
    }
}

int IfAdvRadar::getStepType() {
    switch (g_GameGlobal->m_advData->m_itemGenKind & ~0xFF) {
    case 0x2A000100: return 2;
    case 0x2A000200: return 1;
    default: return 0;
    }
}

void IfAdvRadar::show() {
    nw4r::g3d::ScnGroup* group = g_gfSceneRoot->unk38C[gfSceneRoot::Layer_Info].unk4;
    if (unk4->m_parent != group) {
        refresh();
        group->Insert(group->sceneItemsCount, unk4);
    }
}

void IfAdvRadar::hide() {
    nw4r::g3d::ScnGroup* group = g_gfSceneRoot->unk38C[gfSceneRoot::Layer_Info].unk4;
    if (unk4->m_parent == group) {
        group->Remove(unk4);
    }
}

// TODO: clean up, give variables real names
extern "C" void fn_8010EDA8(void*);

extern "C" void fn_8010EFCC(void* self) {
    fn_8010EDA8(*reinterpret_cast<void**>(reinterpret_cast<u8*>(self) + 0xf0));
}

// TODO: clean up, give variables real names
namespace jj_8010ECB8 {
typedef unsigned int u32;

struct GameData {
    char pad[0x6d0];
    signed char flags[1];
};

struct GameGlobal {
    char pad[0x30];
    GameData* p;
};

extern "C" GameGlobal* g_GameGlobal;
extern "C" u32 getStepIndex__19adventureStaticDataFUl(u32);

// TODO: clean up, give variables real names
extern "C" bool fn_8010ECB8(u32 arg) {
    u32 idx = getStepIndex__19adventureStaticDataFUl(arg | 0x2a000100);
    int value = g_GameGlobal->p->flags[idx >> 3];
    int mask = 1 << (idx & 7);
    return (value & mask) != 0;
}
}

