#include <cm/cm_camera_controller.h>
#include <ef/ef_manager.h>
#include <ef/ef_screen.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_3d_scene_event.h>
#include <gf/gf_camera.h>
#include <gf/gf_copyefb.h>
#include <gf/gf_display.h>
#include <gf/gf_draw.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_loupe.h>
#include <gf/gf_memory_pool.h>
#include <gm/gm_global.h>
#include <mt/mt_matrix.h>
#include <nw4r/g3d/g3d_camera.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/g3d/g3d_scnroot.h>
#include <nw4r/ut/ut_Color.h>
#include <ph/ph_Interface.h>
#include <sr/sr_common.h>
#include <types.h>
#include <math.h>
#include <string.h>

extern nw4r::g3d::LightObj* g_extraLight; // HYPOTHESIS: lbl_8059FF84, no reader in this unit

// HYPOTHESIS: the pointer at 0x40 is the ScnRoot that the anim scene was added to.
extern "C" void RemoveAnmScn__Q34nw4r3g3d7ScnRootFv(nw4r::g3d::ScnRoot* scnRoot);
extern "C" nw4r::g3d::ScnGroup* fn_801AB6CC(MEMAllocator* allocator, u32* size, u32 capacity);

// Defined in gf_light.cpp (the loupe state).
extern unsigned char lbl_80494E00[0xe8];

// HYPOTHESIS: the loupe "restore list" in gf_light.cpp: the ScnObjs createLoupeTexture detaches from their layers,
// renderXlu puts them back at the end of the frame.
struct gfLoupeRestoreEntry {
    u8 m_layer;
    nw4r::g3d::ScnObj* m_scnObj;
};
struct gfLoupeRestoreList {
    u8 m_count;
    gfLoupeRestoreEntry m_entries[0x1D];
};

// Defined in the second part of this unit.
static void ClearDepthBuffer(int width, int height);

void G3DState_Invalidate(u32 flags);

// HYPOTHESIS: the callees are unnamed in the symbols; the sora map calls them efManager::render(int layer) and
// efScreen::render(int layer).
extern "C" void fn_8005B838(efManager* effectManager, int layer);
extern "C" void fn_8005DC24(efScreen* screen, int layer);


void gfSceneRoot::create() {
    g_gfSceneRoot = new (Heaps::System) gfSceneRoot;
}

// Resets a light to "no color, at (0, 10, 0), pointing along (-1, -1, -1), disabled".
static inline nw4r::g3d::LightObj* InitDefaultLight(nw4r::g3d::LightObj* light) {
    // BYTES: dir is declared before pos so that the two stack slots end up in the original order.
    Vec3f dir;
    Vec3f pos;
    light->Clear();
    nw4r::ut::Color color(0, 0, 0, 0);
    light->InitLightColor(color);
    pos.m_x = 0.0f;
    pos.m_y = 10.0f;
    pos.m_z = 0.0f;
    light->InitLightPos(pos.m_x, pos.m_y, pos.m_z);
    dir.m_x = -1.0f;
    dir.m_y = -1.0f;
    dir.m_z = -1.0f;
    light->InitLightDir(dir.m_x, dir.m_y, dir.m_z);
    light->Disable();
    return light;
}

gfSceneRoot::gfSceneRoot()
    : gfTask("gfScnRoot", Category_None, 13, 1, true), unk48(NULL), unk4C(NULL), unk50(0), m_anmScnRes(NULL), unk58(0),
      unk5C(NULL), unk60(NULL), m_transformFlag(), m_shadowEnabled(0), unk464_5(0), m_shadowRendering(0),
      m_skipAnim(0), unk464_0(1), unk465_7(0), unk465_6(0), unk465_3(0), unk468(), m_shadowTextures(NULL), m_shadowInfo(NULL),
      m_shadowPitch(90.0f), m_shadowYaw(0.0f), m_shadowRoll(0.0f) {
    // BYTES: unkC is written as the top byte of a word (read-modify-write with rlwimi).
    struct LayerWord {
        u32 m_unkC : 8;
        u32 m_rest : 24;
    };
    reinterpret_cast<LayerWord*>(&unk38C[7].unkC)->m_unkC = 5;
    reinterpret_cast<LayerWord*>(&unk38C[8].unkC)->m_unkC = 5;
    g_extraLight = InitDefaultLight(&m_extraLight);
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::System);
    u32 size;
    unk40 = nw4r::g3d::ScnRoot::Construct(allocator, &size, 0x138, 0x140, 0x20, 0x20);
    unk44 = nw4r::g3d::ScnRoot::Construct(allocator, &size, 0x138, 0x140, 0x20, 0x20);
    unk48 = nw4r::g3d::ScnRoot::Construct(allocator, &size, 8, 8, 0x20, 0x20);
    gfLoupe::initialize();
    for (int i = 0; i < 7; i++) {
        unk38C[i].unk0 = unk40;
    }
    unk38C[7].unk0 = unk44;
    unk38C[8].unk0 = unk44;
    unk38C[0].m_drawEffect = 1;
    unk38C[4].m_drawEffect = 1;
    unk38C[7].m_drawEffect = 1;
    for (u32 i = 0; i < 10; i++) {
        nw4r::g3d::LightObj* light = &m_lightObjs[i];
        InitDefaultLight(light);
    }
    nw4r::g3d::LightObj* lights = unk40->GetLightSetting().GetLightObjArray();
    for (u32 i = 0; i < 10; i++) {
        LightSlot* slot = &m_lightSlots[i];
        slot->m_light = NULL;
        slot->m_used = 0;
        slot->m_released = 1;
        slot->m_unk7 = 0x14;
        slot->m_unk8 = 0x14;
        slot->m_unkA = 0;
        slot->m_unk9 = 0;
        slot->m_unk5 = 0x16 + i;
        slot->m_unk6 = 0x12 + i;
        nw4r::g3d::LightObj* light = &lights[static_cast<u8>(0x12 + i)];
        InitDefaultLight(light);
    }
    for (int i = 0; i < 9; i++) {
        m_layerEfb[i].m_enable = 0;
        m_layerEfb[i].m_efbIndex = -1;
    }
    m_layerEfb[3].m_enable = 1;
    m_layerEfb[3].m_efbIndex = 1;
    m_layerEfb[7].m_enable = 1;
    m_layerEfb[7].m_efbIndex = 0;
}

gfSceneRoot::~gfSceneRoot() {
    removeResAnmScn();
    removeInfoCamResAnm();
    unk40->Destroy();
    unk44->Destroy();
    unk48->Destroy();
    // HYPOTHESIS: the original destroys the loupe/shadow scene twice (the second call is in the binary).
    unk48->Destroy();
    g_gfSceneRoot = NULL;
}

void gfSceneRoot::addEventNotify(gfSceneRootEvent* event) {
    unk468.addTail(event);
}

void gfSceneRoot::removeEventNotify(gfSceneRootEvent* event) {
    unk468.remove(event);
}

void gfSceneRoot::setUseCameraAnim(bool useCameraAnim) {
    unk464_0 = !useCameraAnim;
}

bool gfSceneRoot::add(LayerType layer, nw4r::g3d::ScnMdl* scnMdl) {
    bool added = false;
    nw4r::g3d::ScnGroup* group = unk38C[layer].unk4;
    if (group != NULL) {
        added = group->Insert(group->sceneItemsCount, scnMdl);
        if (added) {
            unk38C[layer].unkE++;
        }
    }
    return added;
}

// MATCH-ONLY: placeholder virtuals so that unk3C sits at vtable offset 0x3C
class gfSceneRootLayerObj {
public:
    virtual void unk08();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    // HYPOTHESIS: each layer slot at 0x394 + i * 0x18 has a virtual at 0x1C that takes the frame.
    virtual void unk1C(float frame);
    virtual void unk20();
    virtual void unk24();
    virtual void unk28();
    virtual void unk2C();
    virtual void unk30();
    virtual void unk34();
    virtual void unk38();
    virtual bool unk3C(nw4r::g3d::ScnMdl* scnMdl);
};

bool gfSceneRoot::remove(nw4r::g3d::ScnMdl* scnMdl) {
    char* cur = reinterpret_cast<char*>(this);
    bool ret = true;
    for (int i = 0; i < 9; i++) {
        gfSceneRootLayerObj* obj = *reinterpret_cast<gfSceneRootLayerObj**>(cur + 0x390);
        bool found = false;
        if (obj != NULL) {
            found = obj->unk3C(scnMdl);
            if (found) {
                *reinterpret_cast<u16*>(cur + 0x39A) -= 1;
            }
        }
        if (found) {
            ret = true;
            break;
        }
        cur += 0x18;
    }
    return ret;
}

bool gfSceneRoot::remove(LayerType layer, nw4r::g3d::ScnMdl* scnMdl) {
    bool removed = false;
    nw4r::g3d::ScnGroup* group = unk38C[layer].unk4;
    if (group != NULL) {
        removed = group->Remove(scnMdl);
        if (removed) {
            unk38C[layer].unkE--;
        }
    }
    return removed;
}

void gfSceneRoot::setLayerDispStatus(LayerType layer, bool status) {
    unk38C[layer].unkD_7 = status;
}

bool gfSceneRoot::clear(LayerType layer) {
    bool result = false;
    nw4r::g3d::ScnGroup* group = unk38C[layer].unk4;
    if (group != NULL) {
        while (!group->Empty()) {
            group->PopBack();
        }
        unk38C[layer].unkE = 0;
        result = true;
    }
    return result;
}

void gfSceneRoot::setResAnmScn(nw4r::g3d::ResAnmScn* anmScn, int heap) {
    if (anmScn->IsValid()) {
        removeResAnmScn();
        unk4C = anmScn->ptr();
        u32 size;
        MEMAllocator* allocator = gfHeapManager::getMEMAllocator(static_cast<Heaps::HeapType>(heap));
        m_anmScnRes = nw4r::g3d::AnmScnRes::Construct(allocator, &size, *anmScn, false);
        setAnmScnResToScnRoot3D(m_anmScnRes);
    } else {
        removeResAnmScn();
        unk465_6 = false;
    }
}

void gfSceneRoot::setAnmScnResToScnRoot3D(nw4r::g3d::AnmScnRes* anmScn) {
    unk40->SetAnmScn(anmScn);
    gfCameraManager* cameraManager = gfCameraManager::getManager();
    unk40->CalcWorld();
    nw4r::g3d::Camera camera = unk40->GetCamera(0);
    cameraManager->m_cameras[0].m_near = camera.ref().projNear;
    cameraManager->m_cameras[0].m_far = camera.ref().projFar;
    u32 numLightSet = anmScn->GetLightSetMaxRefNumber();
    u32 numDiffuseLight = anmScn->GetDiffuseLightMaxRefNumber();
    unk465_6 = 0;
    // HYPOTHESIS: a scene animation with exactly 0x16 light sets carries the zone lights (set 0x14 holds the default).
    if (numLightSet > 0x15 && static_cast<u8>(numLightSet - 0x15) == 1) {
        unk465_6 = 1;
        m_zoneLightObjBase = 0xC;
    }
    nw4r::g3d::LightSetting* setting = &unk40->GetLightSetting();
    nw4r::g3d::LightObj* lights = setting->GetLightObjArray();
    nw4r::g3d::LightSet zoneSet = setting->GetLightSet(0x14);
    nw4r::g3d::LightObj* zoneLight = zoneSet.GetLightObj(0);
    if (zoneLight != NULL) {
        for (int i = 0; i < 10; i++) {
            lights[static_cast<u8>(0x12 + i)] = *zoneLight;
        }
    }
}

void gfSceneRoot::removeResAnmScn() {
    if (m_anmScnRes != NULL) {
        RemoveAnmScn__Q34nw4r3g3d7ScnRootFv(*reinterpret_cast<nw4r::g3d::ScnRoot**>(reinterpret_cast<char*>(this) + 0x40));
        m_anmScnRes->Destroy();
    }
    if (*reinterpret_cast<s32*>(reinterpret_cast<char*>(this) + 0x4C) != 0) {
        *reinterpret_cast<s32*>(reinterpret_cast<char*>(this) + 0x4C) = 0;
    }
    m_anmScnRes = NULL;
    *reinterpret_cast<s32*>(reinterpret_cast<char*>(this) + 0x58) = 0;
}

void gfSceneRoot::setFinalSceneAnm(nw4r::g3d::ResAnmScn* anmScn) {
    if (unk60 != NULL) {
        removeFinalSceneAnm();
    }
    if (anmScn->IsValid()) {
        u32 size;
        MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::FighterTechqniq);
        unk60 = nw4r::g3d::AnmScnRes::Construct(
            allocator, &size, *anmScn, false);
        setAnmScnResToScnRoot3D(unk60);
    }
}

void gfSceneRoot::removeFinalSceneAnm() {
    if (unk60 != NULL) {
        RemoveAnmScn__Q34nw4r3g3d7ScnRootFv(unk40);
        unk60->Destroy();
    }
    unk60 = NULL;
    if (m_anmScnRes != NULL) {
        setAnmScnResToScnRoot3D(m_anmScnRes);
    }
}

void gfSceneRoot::setInfoCamResAnm(nw4r::g3d::ResAnmScn* anmScn) {
    if (unk50 != 0) {
        removeInfoCamResAnm();
    }
    if (anmScn->IsValid()) {
        unk50 = *reinterpret_cast<u32*>(anmScn);
        MEMAllocator* allocator;
        if (g_HeapInfos[Heaps::InfoInstance].m_name == NULL) {
            allocator = gfHeapManager::getMEMAllocator(Heaps::MenuInstance);
        } else {
            allocator = gfHeapManager::getMEMAllocator(Heaps::InfoInstance);
        }
        u32 size;
        unk5C = nw4r::g3d::AnmScnRes::Construct(allocator, &size, *anmScn, false);
        unk44->SetAnmScn(unk5C);
    }
}

void gfSceneRoot::removeInfoCamResAnm() {
    if (unk5C != NULL) {
        RemoveAnmScn__Q34nw4r3g3d7ScnRootFv(unk44);
        unk5C->Destroy();
    }
    if (unk50 != 0) {
        unk50 = 0;
    }
    unk5C = NULL;
}

// HYPOTHESIS: 0x54 points at an object whose virtual at 0x1C is called with no argument.
// MATCH-ONLY: the leading unused virtuals place unk1C at vtable offset 0x1C.
struct gfSceneRootAnmObj {
    virtual void unk00();
    virtual void unk04();
    virtual void unk08();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk1C();
};

void gfSceneRoot::setCurrentFrame(float frame) {
    gfSceneRootAnmObj* obj = *reinterpret_cast<gfSceneRootAnmObj**>(reinterpret_cast<char*>(this) + 0x54);
    if (obj != nullptr) {
        obj->unk1C();
        s32 i = 0;
        char* cur = reinterpret_cast<char*>(this);
        do {
            gfSceneRootLayerObj* slot = *reinterpret_cast<gfSceneRootLayerObj**>(cur + 0x394);
            if (slot != nullptr) {
                slot->unk1C(frame);
            }
            i += 1;
            cur += 0x18;
        } while (i < 9);
    }
}

// Steps a scene animation at the given rate scaled by the stage motion ratios.
static inline void StepSceneAnim(nw4r::g3d::AnmScnRes* anmScn, float rate) {
    rate *= g_GameGlobal->m_stageData->m_motionRatio;
    rate *= g_GameGlobal->m_stageData->m_motionSubRatio;
    anmScn->SetUpdateRate(rate);
    anmScn->UpdateFrame();
}

void gfSceneRoot::processAnim() {
    if (!g_gfSceneRoot->m_skipAnim) {
        if (!g_gfSceneRoot->unk465_3) {
            nw4r::g3d::AnmScnRes* anmScn = unk60;
            if (anmScn == NULL) {
                anmScn = m_anmScnRes;
                if (anmScn != NULL) {
                    StepSceneAnim(anmScn, 1.0f / g_GameGlobal->getSlowRate());
                }
            } else if (anmScn != NULL) {
                StepSceneAnim(anmScn, 1.0f / g_GameGlobal->getSlowRate());
            }
        }
        nw4r::g3d::AnmScnRes* infoAnmScn = unk5C;
        float rate = 1.0f;
        if (infoAnmScn != NULL) {
            rate *= g_GameGlobal->m_stageData->m_motionRatio;
            rate *= g_GameGlobal->m_stageData->m_motionSubRatio;
            infoAnmScn->SetUpdateRate(rate);
            infoAnmScn->UpdateFrame();
        }
    }
    for (int i = 0; i < 10; i++) {
        LightSlot* slot = &m_lightSlots[i];
        if (slot->m_used) {
            if (slot->m_unk7 != slot->m_unk8) {
                slot->m_unk9++;
            }
            if (slot->m_unk9 > 3) {
                slot->m_unk9 = 0;
                slot->m_unk8 = slot->m_unk7;
            }
            if (slot->m_unk7 != slot->m_unk8) {
                if (slot->m_unk7 > slot->m_unk8) {
                    slot->m_unkA = slot->m_unk9;
                } else {
                    slot->m_unkA = 4 - slot->m_unk9;
                }
            } else {
                slot->m_unkA = 0;
            }
        }
    }
}

void gfSceneRoot::processFixCamera() {
    gfLoupe::setup();
}

void gfSceneRoot::layerUpdateFrame(int layer) {
    unk38C[layer].unk4->G3dProc(8, 0, 0);
}

// HYPOTHESIS: the ScnRoot keeps its LightObj array (0x44 bytes each, first 0x12 reserved) at 0x287C.
nw4r::g3d::LightObj* gfSceneRoot::getLightObj(u8 p1) {
    char* lights = *reinterpret_cast<char**>(reinterpret_cast<char*>(unk40) + 0x287C);
    return reinterpret_cast<nw4r::g3d::LightObj*>(lights + (p1 + 0x12) * 0x44);
}

void gfSceneRoot::updateZoneLightSet(u8 p1, u8 p2) {
    LightSlot* slot = &m_lightSlots[p1];
    if (slot->m_unk7 != p2) {
        slot->m_unk8 = slot->m_unk7;
        slot->m_unk7 = p2;
        slot->m_unk9 = 0;
    }
}

// Copies the light indices 1 to 7 of a light set to another one.
static inline void CopyLightIndices(nw4r::g3d::LightSet& dst, nw4r::g3d::LightSet& src) {
    for (int j = 1; j < 8; j++) {
        s8 index = src.GetData()->idxLight[j];
        dst.GetData()->idxLight[j] = index;
    }
}

void gfSceneRoot::renderPre() {
    // HYPOTHESIS: G3dProc tasks 1 to 3 are the calc world, material and vertex passes of the layer group.
    for (int i = 0; i < 8; i++) {
        Layer* layer = &unk38C[i];
        if (unk38C[i].unkE != 0 && layer->m_display && unk38C[i].unk4->sceneItemsCount != 0) {
            gfCameraManager::getManager();
            unk38C[i].unk4->G3dProc(1, 0, 0);
            unk38C[i].unk4->G3dProc(2, 0, 0);
            unk38C[i].unk4->G3dProc(3, 0, 0);
        }
    }
    unk40->SetCurrentCamera(0);
    unk40->CalcWorld();
    if (unk464_0 || CameraController::getInstance()->isForceEnableCamControll3D()) {
        gfCameraManager* cameraManager = gfCameraManager::getManager();
        nw4r::g3d::Camera camera = unk40->GetCamera(0);
        cameraManager->m_cameras[0].setNwCamera(&camera);
    }
    Layer* infoLayer = &unk38C[8];
    if (infoLayer->unkE != 0 && infoLayer->m_display && infoLayer->unk4->sceneItemsCount != 0) {
        gfCameraManager::getManager();
        infoLayer->unk4->G3dProc(1, 0, 0);
        infoLayer->unk4->G3dProc(2, 0, 0);
        infoLayer->unk4->G3dProc(3, 0, 0);
    }
    unk44->SetCurrentCamera(0);
    unk44->CalcWorld();
    if (unk465_7) {
        gfCameraManager* cameraManager = gfCameraManager::getManager();
        nw4r::g3d::Camera camera = unk44->GetCamera(0);
        cameraManager->m_cameras[5].setNwCamera(&camera);
    }
    nw4r::g3d::LightSet zoneSet = unk40->GetLightSetting().GetLightSet(0x14);
    if (zoneSet.IsValid()) {
        for (u32 i = 0; i < 4; i++) {
            nw4r::g3d::LightObj* light = zoneSet.GetLightObj(i);
            GXColor* color = &m_zoneColors[i];
            if (light != NULL) {
                light->GetLightColor(color);
            } else {
                color->r = 0;
                color->g = 0;
                color->b = 0;
                color->a = 0xFF;
            }
        }
    } else {
        for (int i = 0; i < 4; i++) {
            m_zoneColors[i].r = 0;
            m_zoneColors[i].g = 0;
            m_zoneColors[i].b = 0;
            m_zoneColors[i].a = 0xFF;
        }
    }
    if (unk465_6) {
        nw4r::g3d::LightSetting* setting = &unk40->GetLightSetting();
        nw4r::g3d::LightSet zoneFrom = setting->GetLightSet(0x14);
        nw4r::g3d::LightSet zoneTo = setting->GetLightSet(0x15);
        nw4r::g3d::LightObj* lights = setting->GetLightObjArray();
        for (int i = 1; i < 4; i++) {
            float ratio = i * 0.25f;
            int index = m_zoneLightObjBase + (i - 1) * 2 + 1;
            for (u32 j = 1; j < 3; j++) {
                nw4r::g3d::LightObj* from = zoneFrom.GetLightObj(j);
                nw4r::g3d::LightObj* to = zoneTo.GetLightObj(j);
                nw4r::g3d::LightObj* out = &lights[index - 1];
                if (from != NULL) {
                    *out = *from;
                    if (to != NULL) {
                        blendLightColor(ratio, from, to, out);
                    }
                }
                index++;
            }
        }
    }
    for (int i = 0; i < 6; i++) {
        nw4r::g3d::Fog fog = unk40->GetFog(i);
        GXFogType type;
        f32 startZ;
        f32 endZ;
        f32 nearZ;
        f32 farZ;
        GXColor color;
        fog.GetFog(&type, &startZ, &endZ, &nearZ, &farZ, &color);
        if (type == GX_FOG_NONE) {
            fog.Init();
        }
    }
    nw4r::g3d::LightSetting* setting = &unk40->GetLightSetting();
    nw4r::g3d::LightSet defaultSet = setting->GetLightSet(0x14);
    s8 ambLightIdx = defaultSet.GetData()->idxAmbLight;
    for (int i = 0; i < 10; i++) {
        LightSlot* slot = &m_lightSlots[i];
        if (slot->m_used) {
            nw4r::g3d::LightSet lightSet = setting->GetLightSet(slot->m_unk5);
            lightSet.GetData()->idxLight[0] = slot->m_unk6;
            lightSet.GetData()->idxAmbLight = ambLightIdx;
            if (slot->m_released) {
                if (unk465_6) {
                    if (slot->m_unkA == 0) {
                        nw4r::g3d::LightSet zoneSet = setting->GetLightSet(slot->m_unk7);
                        CopyLightIndices(lightSet, zoneSet);
                    } else {
                        u8 base = m_zoneLightObjBase + (slot->m_unkA - 1) * 2;
                        for (u32 j = 1; j < 3; j++) {
                            lightSet.SelectLightObj(static_cast<u8>(j), static_cast<u8>(base + j - 1));
                        }
                    }
                } else {
                    CopyLightIndices(lightSet, defaultSet);
                }
            } else {
                for (u32 j = 1; j < 8; j++) {
                    lightSet.SelectLightObj(static_cast<u8>(j), -1);
                }
            }
            lightSet.GetData()->idxLight[7] = -1;
        }
    }
    m_callBacks.process();
    phInterface::notifiAllEndCalcWorldTimingC();
}

void gfSceneRoot::renderOpa() {
}

// HYPOTHESIS: bytes 0x500 and 0x506 are two copies of the use-copy-FB flag.
void gfSceneRoot::setUseCopyFB(u8 p1) {
    struct Byte {
        u8 m_value : 8;
    };
    reinterpret_cast<Byte*>(reinterpret_cast<u8*>(this) + 0x500)->m_value = p1;
    reinterpret_cast<Byte*>(reinterpret_cast<u8*>(this) + 0x506)->m_value = p1;
}

void gfSceneRoot::renderXlu() {
    gfCopyEFBMgr* copyEFBMgr = gfCopyEFBMgr::getInstance();
    GXSetClipMode(GX_CLIP_ENABLE);
    unk38C[6].m_copyEFB = 0;
    unk38C[3].m_copyEFB = 0;
    gfCopyEFBMgr* currentCopyEFBMgr = gfCopyEFBMgr::getInstance();
    if (currentCopyEFBMgr != NULL) {
        switch (currentCopyEFBMgr->m_104) {
        case 1:
            unk38C[6].m_copyEFB = 1;
            break;
        case 2:
            unk38C[3].m_copyEFB = 1;
            break;
        }
    }
    if (m_shadowEnabled) {
        renderShadow();
    }
    unk468.notify(0);
    createLoupeTexture();
    unk468.notify(0);
    for (u32 i = 0; i < 9; i++) {
        if (static_cast<u8>(i) == Layer_Info && m_transformFlag.m_reverseDirection != 0) {
            drawFrameBufferReverse(m_transformFlag.m_reverseDirection);
        }
        if (unk38C[static_cast<u8>(i)].unkE != 0) {
            if (copyEFBMgr != NULL && static_cast<u8>(i) == Layer_Special_Stage_Object) {
                gfFrameBuffer* frameBuffer = gfFrameBuffer::getInstance();
                ClearDepthBuffer(frameBuffer->m_efbWidth, frameBuffer->m_efbHeight);
            }
            if (unk38C[static_cast<u8>(i)].m_clearDepth) {
                gfFrameBuffer* frameBuffer = gfFrameBuffer::getInstance();
                ClearDepthBuffer(frameBuffer->m_efbWidth, frameBuffer->m_efbHeight);
            }
            if (unk38C[static_cast<u8>(i)].m_display && unk38C[static_cast<u8>(i)].unk4->sceneItemsCount != 0) {
                unk38C[static_cast<u8>(i)].unk0->Insert(unk38C[static_cast<u8>(i)].unk0->sceneItemsCount, unk38C[static_cast<u8>(i)].unk4);
                unk38C[static_cast<u8>(i)].unk0->CalcView();
                unk38C[static_cast<u8>(i)].unk0->GatherDrawScnObj();
                unk38C[static_cast<u8>(i)].unk0->ZSort();
                G3DState_Invalidate(0x7FF);
                unk38C[static_cast<u8>(i)].unk0->DrawOpaAndXlu();
                nw4r::g3d::ScnRoot* scnRoot = unk38C[static_cast<u8>(i)].unk0;
                while (!scnRoot->Empty()) {
                    scnRoot->PopBack();
                }
            }
        }
        if (unk38C[static_cast<u8>(i)].m_drawEffect) {
            if (static_cast<u8>(i) == Layer_Ground) {
                fn_8005B838(efManager::getInstance(), 0);
            } else if (static_cast<u8>(i) == Layer_Effect) {
                fn_8005B838(efManager::getInstance(), 1);
            } else if (static_cast<u8>(i) == Layer_Effect2D) {
                fn_8005B838(efManager::getInstance(), 2);
            }
        }
        if (unk38C[static_cast<u8>(i)].m_copyEFB && copyEFBMgr != NULL && !unk465_5) {
            u32 colorWord = copyEFBMgr->m_108;
            GXSetZMode(false, GX_LEQUAL, false);
            GXPixModeSync();
            // BYTES: the word at 0x6C of the manager is handed over as a GXColor.
            union {
                u32 m_word;
                GXColor m_color;
            } color;
            color.m_word = colorWord;
            copyEFBMgr->drawCopyEFB(0, &color.m_color);
            GXSetZMode(true, GX_LEQUAL, true);
        }
        fn_8005DC24(g_efScreen, static_cast<u8>(i));
        if (m_layerEfb[static_cast<u8>(i)].m_enable != 0 && m_layerEfb[static_cast<u8>(i)].m_efbIndex >= 0 && copyEFBMgr != NULL) {
            copyEFBMgr->getEFB(m_layerEfb[static_cast<u8>(i)].m_efbIndex);
        }
        if (static_cast<u8>(i) == Layer_Ground) {
            gfLoupe::grabLoupeBGColor();
        }
    }
    gfLoupeRestoreList* restoreList = reinterpret_cast<gfLoupeRestoreList*>(lbl_80494E00);
    if (restoreList->m_count != 0) {
        for (u32 i = 0; i < restoreList->m_count; i++) {
            nw4r::g3d::ScnGroup* group = unk38C[restoreList->m_entries[i].m_layer].unk4;
            group->Insert(group->sceneItemsCount, restoreList->m_entries[i].m_scnObj);
        }
        restoreList->m_count = 0;
    }
    gfCameraManager::getManager()->m_cameras[0].setGX();
}

void gfSceneRoot::setClearDepthBufferState(LayerType layer, bool clear) {
    unk38C[layer].unkD_6 = clear;
}

// HYPOTHESIS: the map names this helper nw4r::g3d::ScnRoot::resetSceneSub; it is only called from this unit.
// Resets the fogs of the scene and turns the first four lights of every light set black and off.
static void resetSceneSub(nw4r::g3d::ScnRoot* scnRoot) {
    for (int i = 0;; i++) {
        nw4r::g3d::Fog fog = scnRoot->GetFog(i);
        if (!fog.IsValid()) {
            break;
        }
        fog.SetZ(1000.0f, 10000.0f);
        fog.SetFogColor(nw4r::ut::Color(0, 0, 0, 0));
        fog.Init();
    }
    nw4r::g3d::LightSetting* setting = &scnRoot->GetLightSetting();
    u32 numLightSet = setting->GetNumLightSet();
    for (u32 i = 0; i < numLightSet; i++) {
        nw4r::g3d::LightSet lightSet = setting->GetLightSet(i);
        for (u32 j = 0; j < 4; j++) {
            nw4r::g3d::LightObj* light = lightSet.GetLightObj(j);
            if (light != NULL) {
                light->InitLightColor(nw4r::ut::Color(0, 0, 0, 0));
                light->Disable();
            }
        }
    }
}

void gfSceneRoot::resetScene() {
    resetSceneSub(unk40);
}

// HYPOTHESIS: out = from + (to - from) * ratio per color channel (paired singles, u8 quantized).
void gfSceneRoot::blendLightColor(f32 ratio, nw4r::g3d::LightObj* from, nw4r::g3d::LightObj* to, nw4r::g3d::LightObj* out) {
    GXColor colorFrom;
    GXColor colorTo;
    GXColor colorOut;
    from->GetLightColor(&colorFrom);
    to->GetLightColor(&colorTo);
    register f32 fr0, fr1, fr2, fr3;
    register f32 r = ratio;
    register GXColor* pFrom = &colorFrom;
    register GXColor* pTo = &colorTo;
    register GXColor* pOut = &colorOut;
    asm {
        psq_l  fr2, 0(pFrom), 0, 2
        psq_l  fr0, 0(pTo), 0, 2
        psq_l  fr3, 2(pFrom), 0, 2
        psq_l  fr1, 2(pTo), 0, 2
        ps_sub fr0, fr0, fr2
        ps_sub fr1, fr1, fr3
        ps_madds0 fr2, fr0, r, fr2
        ps_madds0 fr3, fr1, r, fr3
        psq_st fr2, 0(pOut), 0, 2
        psq_st fr3, 2(pOut), 0, 2
    }
    out->InitLightColor(colorOut);
}

void gfSceneRoot::renderDebug() {
}

gfSceneRoot::Layer::Layer() {
    unk0 = 0;
    *reinterpret_cast<u32*>(&unkC) &= 0x00FFFFFF;
    unk4 = NULL;
    unkD_7 = 1;
    unkD_6 = 0;
    unkD_4 = 0;
    unk8 = NULL;
    unkE = 0;
    unk10 = 0;
    unk14 = 0;
    u32 size;
    unk4 = fn_801AB6CC(gfHeapManager::getMEMAllocator(Heaps::System), &size, 0x140);
}

gfSceneRoot::Layer::~Layer() {
    nw4r::g3d::ScnGroup* group = unk4;
    while (!group->Empty()) {
        group->PopBack();
    }
    unk4->Destroy();
    unk0 = 0;
}

// Draws the loupe background texture as a quad of the given radius, scrolled by (scrollU, scrollV).
// TEMPORARY (part split): not static yet, an unused static function is dropped by the compiler; it becomes static once part B
// (createLoupeTexture) is merged in.
static void drawLoupeBG(GXColor color, float radius, float scrollU, float scrollV) {
    Matrix matrix;
    GXTexObj* texture = gfLoupe::getBGTexture();
    if (texture != NULL) {
        float left = -radius;
        float diameter = 2.0f * radius;
        GXLoadPosMtxImm(matrix.m, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        GXSetZMode(true, GX_ALWAYS, false);
        gfDrawSetVtxPosColorTexPrimEnvironment();
        GXLoadTexObj(texture, GX_TEXMAP0);
        GXClearVtxDesc();
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
        GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
        GXSetClipMode(GX_CLIP_DISABLE);
        GXSetCullMode(GX_CULL_NONE);
        GXSetChanCtrl(GX_COLOR0A0, false, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
        GXSetChanMatColor(GX_COLOR0A0, color);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
        GXSetZMode(false, GX_ALWAYS, false);
        GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, 4);
        GXPosition3f32(left, radius, -10.0f);
        GXTexCoord2f32(scrollU, scrollV);
        GXPosition3f32(left, radius - diameter, -10.0f);
        GXTexCoord2f32(scrollU, 0.5f + scrollV);
        GXPosition3f32(left + diameter, radius, -10.0f);
        GXTexCoord2f32(0.5f + scrollU, scrollV);
        GXPosition3f32(left + diameter, radius - diameter, -10.0f);
        GXTexCoord2f32(0.5f + scrollU, 0.5f + scrollV);
        GXSetClipMode(GX_CLIP_ENABLE);
        GXSetCullMode(GX_CULL_BACK);
    }
}

namespace nw4r {
namespace g3d {
namespace G3DState {
const GXRenderModeObj* GetRenderModeObj();
}
void G3dReset();
} // namespace g3d
} // namespace nw4r

struct _FILE;
typedef struct _FILE FILE;
extern "C" void abort();
extern "C" int fprintf(FILE* file, const char* format, ...);
// HYPOTHESIS: stderr is the third entry of the MSL __files table (0x50 bytes per FILE), as the stderr macro expands.
extern "C" u8 __files[];
#define gfShadowStderr (reinterpret_cast<FILE*>(__files + 0xA0))

extern u32 lbl_805A0010;
extern u32 lbl_805A0014;
extern gfLoupe::LoupeInfo lbl_80494EE8[7];



// HYPOTHESIS: declared by symbol; the local ScnRoot shadow has no ForceDrawMode (ResMdlDrawMode is an enum, 4 =
// ignore material, 2 = sort translucent by Z).
extern "C" void ForceDrawMode__Q34nw4r3g3d7ScnRootFbQ34nw4r3g3d14ResMdlDrawMode(nw4r::g3d::ScnRoot* scnRoot, bool force, int mode);
// HYPOTHESIS: LightSetting::Import(const LightSetting&), declared by symbol until the light header declares it.
extern "C" void Import__Q34nw4r3g3d12LightSettingFRCQ34nw4r3g3d12LightSetting(nw4r::g3d::LightSetting* self,
                                                                          const nw4r::g3d::LightSetting* other);
extern "C" void* GXGetTexObjData(const GXTexObj* obj);
extern "C" void G3DState_Invalidate__FUl(u32 flags);

// BYTES: the active GX header lists GXTevMode out of the SDK's order (its GX_PASSCLR is 3 and GX_REPLACE is 2); the
// original passes the SDK values 4 (pass the rasterized color) and 3 (replace with the texture).
#define GX_TEVOP_PASSCLR static_cast<GXTevMode>(4)
#define GX_TEVOP_REPLACE static_cast<GXTevMode>(3)

// BYTES: ps_sub + ps_sel that selects into the difference's own register (mt_ps_minmax.h uses a second register).
// MWCC has no paired-single intrinsic, so these are asm like the 2D collision helpers.
static inline float psMaxInPlace(register float a, register float b) {
    register float result;
    // clang-format off
    asm {
        ps_sub result, a, b
        ps_sel result, result, a, b
    }
    // clang-format on
    return result;
}

static inline float psMinInPlace(register float a, register float b) {
    register float result;
    // clang-format off
    asm {
        ps_sub result, a, b
        ps_sel result, result, b, a
    }
    // clang-format on
    return result;
}

// File-local helpers defined further down in the original order.
static void SetupSimpleMaterial(GXColor color);
static void DrawEFBClear(int width, int height);
static void DrawNonShadowBorder(int width, int height);
static void ShowShadowTexture(GXTexObj* texObj);

// HYPOTHESIS: renderShadow inlines a std::vector-like list of texture data pointers (data, size, capacity) that
// remembers which shadow textures were already rendered this frame, so a texture shared by several entries is drawn
// once. The length error and allocation failure strings and the growth policy below are those of the MSL vector
// (stderr + abort because the unit has no exceptions); the member names are not from the map.
static inline const u32& gfShadowListMax(const u32& a, const u32& b) {
    return a < b ? b : a;
}

// HYPOTHESIS: the temporary buffer the growth path builds before swapping it into the list.
struct gfShadowListBuffer {
    void** m_data;
    u32 m_count;
    u32 m_capacity;
    u32* m_capacityRef;
    u32 m_offset;
    gfShadowListBuffer(u32* capacityRef) : m_data(NULL), m_count(0), m_capacity(0), m_capacityRef(capacityRef), m_offset(0) { }
    ~gfShadowListBuffer() {
        if (m_data != NULL) {
            m_count = 0;
            operator delete(m_data);
        }
    }
};

class gfShadowTexList;
static inline void gfShadowListGrow(gfShadowTexList* list, void* texData);

class gfShadowTexList {
public:
    gfShadowTexList() : m_data(NULL), m_capacity(0), m_size(0) { }
    ~gfShadowTexList() {
        if (m_data != NULL) {
            m_size -= m_size;
            operator delete(m_data);
        }
    }
    bool contains(void* texData) const {
        bool notFound = false;
        u32 i = 0;
        while (i < m_size) {
            if (m_data[i] == texData) {
                break;
            }
            i++;
        }
        if (i == m_size) {
            notFound = true;
        }
        return notFound;
    }
    void push_back(void* texData) {
        if (m_size < m_capacity) {
            m_size = m_size + 1;
            m_data[m_size - 1] = texData;
        } else {
            gfShadowListGrow(this, texData);
        }
    }
    void** m_data;
    u32 m_size;
    u32 m_capacity;
};

// HYPOTHESIS: MSL vector growth: 60 percent below 0x15555555 elements, 50 percent below 0x2AAAAAAA, else the
// maximum of 0x3FFFFFFF.
static inline void gfShadowListGrow(gfShadowTexList* list, void* texData) {
    u32 oldCapacity = list->m_capacity;
    u32 maxGrow = 0x3FFFFFFF - oldCapacity;
    if (maxGrow < 1) {
        fprintf(gfShadowStderr, "vector length error");
        abort();
    }
    gfShadowListBuffer buffer(&list->m_capacity);
    u32 growFast;
    u32 growSlow;
    u32 needed = list->m_size + 1 - list->m_capacity;
    if (needed > maxGrow) {
        fprintf(gfShadowStderr, "vector length error");
        abort();
    }
    u32 newCapacity;
    if (oldCapacity < 0x15555555) {
        growFast = (oldCapacity + 1) * 3 / 5;
        newCapacity = oldCapacity + gfShadowListMax(growFast, needed);
    } else if (oldCapacity < 0x2AAAAAAA) {
        growSlow = (oldCapacity + 1) / 2;
        newCapacity = oldCapacity + gfShadowListMax(growSlow, needed);
    } else {
        newCapacity = 0x3FFFFFFF;
    }
    if (newCapacity > 0x3FFFFFFF) {
        fprintf(gfShadowStderr, "vector length error");
        abort();
    }
    void** newData = static_cast<void**>(operator new(newCapacity * 4));
    if (newData == NULL) {
        fprintf(gfShadowStderr, "Memory allocation failure");
        abort();
    }
    buffer.m_offset = list->m_size;
    buffer.m_capacity = newCapacity;
    *(newData + list->m_size + buffer.m_count) = texData;
    buffer.m_data = newData;
    buffer.m_count++;
    void** begin = list->m_data;
    u32 oldCount = (begin + list->m_size) - begin;
    buffer.m_offset = list->m_size - oldCount;
    memcpy(newData + buffer.m_offset, begin, oldCount * 4);
    memset(begin, 0, oldCount * 4);
    u32 addedCount = buffer.m_count;
    void** oldData = list->m_data;
    u32 oldCapacityValue = list->m_capacity;
    u32 bufferCapacity = buffer.m_capacity;
    list->m_data = newData;
    list->m_capacity = bufferCapacity;
    buffer.m_capacity = oldCapacityValue;
    buffer.m_data = oldData;
    list->m_size = addedCount + oldCount;
    buffer.m_count = 0;
}

void gfSceneRoot::createLoupeTexture() {
    nw4r::g3d::ScnRoot* scene = unk48;
    Import__Q34nw4r3g3d12LightSettingFRCQ34nw4r3g3d12LightSetting(&scene->m_lightSetting, &unk40->m_lightSetting);
    while (!scene->Empty()) {
        scene->PopBack();
    }
    u16 width = lbl_805A0010;
    u16 height = lbl_805A0014;
    nw4r::g3d::Camera camera = scene->GetCamera(0);
    nw4r::g3d::Camera::PostureInfo posture;
    camera.GetPosture(&posture);
    posture.tp = nw4r::g3d::Camera::POSTURE_AIM;
    posture.cameraTwist = 0.0f;
    camera.SetPosture(posture);
    camera.SetScissor(0, 0, width, height);
    float viewHeight = height;
    float viewWidth = width;
    camera.SetViewport(0.0f, 0.0f, viewWidth, viewHeight);
    camera.SetViewportZRange(0.0f, 1.0f);
    camera.GXSetViewport();
    camera.GXSetScissor();
    camera.GXSetScissorBoxOffset();
    GXColor clearColor = gfFrameBuffer::getInstance()->m_clearColor;
    GXSetCopyClear(clearColor, 0xFFFFFF);
    scene->SetCurrentCamera(0);
    lbl_80494E00[0] = 0;
    gfLoupeRestoreList* restore = reinterpret_cast<gfLoupeRestoreList*>(lbl_80494E00);
    for (u8 i = 0; i < 7; i++) {
        void* image = gfLoupe::getImage(i);
        if (image != NULL) {
            GXColor bgColor = gfLoupe::getBGColor(i);
            if (gfLoupe::isEnable(i)) {
                gfLoupe::LoupeInfo* info = &lbl_80494EE8[i];
                float radius = gfLoupe::getLoupeRadius(i);
                radius = psMaxInPlace(radius, 0.1f);
                u32 numObjs = gfLoupe::getScnObjNum(i);
                Vec3f center = *gfLoupe::getLoupeCenter(i);
                float scaled = 1.2f * radius;
                float distance = psMaxInPlace(50.0f, scaled);
                float scaledDistance = 2.0f * distance;
                float far = psMaxInPlace(200.0f, scaledDistance);
                camera.SetPosition(center.m_x, center.m_y, center.m_z + distance);
                posture.cameraTarget.x = center.m_x;
                posture.cameraTarget.y = center.m_y;
                posture.cameraTarget.z = center.m_z;
                camera.SetPosture(posture);
                camera.SetOrtho(radius, -radius, -radius, radius, 1.0f, far);
                camera.GXSetProjection();
                drawLoupeBG(bgColor, radius, info->m_bgScrollU, info->m_bgScrollV);
                if (numObjs != 0) {
                    gfLoupe::ObjInfo* objInfo = gfLoupe::getObjInfoArray(i);
                    for (u32 j = 0; j < numObjs; j++) {
                        nw4r::g3d::ScnObj* obj = objInfo[j].m_scnObj;
                        if (obj != NULL) {
                            obj->DetachFromParent();
                            scene->Insert(scene->Size(), obj);
                        }
                    }
                    G3DState_Invalidate__FUl(0x7FF);
                    scene->CalcView();
                    scene->GatherDrawScnObj();
                    scene->DrawOpa();
                    scene->DrawXlu();
                    while (!scene->Empty()) {
                        scene->PopBack();
                    }
                    for (u32 j = 0; j < numObjs; j++) {
                        gfSceneRoot::LayerType layer = objInfo[j].m_layer;
                        nw4r::g3d::ScnObj* obj = objInfo[j].m_scnObj;
                        if (obj != NULL) {
                            restore->m_entries[restore->m_count].m_layer = layer;
                            restore->m_entries[restore->m_count].m_scnObj = obj;
                            restore->m_count++;
                        }
                    }
                }
                GXSetZMode(false, GX_ALWAYS, true);
                GXSetTexCopySrc(0, 0, width, height);
                GXSetTexCopyDst(width, height, GX_TF_RGB565, false);
                GXCopyTex(image, true);
            }
        }
    }
    GXPixModeSync();
    while (!scene->Empty()) {
        scene->PopBack();
    }
}

void gfSceneRoot::initShadow(int mode, const Vec3f* boxMin, const Vec3f* boxMax) {
    Vec3f corners[8];
    nw4r::g3d::Camera::PostureInfo posture;
    if (mode == 0) {
        nw4r::g3d::Camera camera = unk40->GetCamera(7);
        camera.SetOrtho(10.0f, -10.0f, -10.0f, 10.0f, 1.0f, 1000.0f);
        Vec3f target(0.0f, 0.0f, 0.0f);
        posture.tp = nw4r::g3d::Camera::POSTURE_AIM;
        posture.cameraTarget.x = target.m_x;
        posture.cameraTarget.y = target.m_y;
        posture.cameraTarget.z = target.m_z;
        posture.cameraTwist = 0.0f;
        camera.SetPosition(0.0f, 0.0f, 10.0f);
        camera.SetPosture(posture);
    } else {
        float distance = gfCameraManager::getManager()->m_cameras[0].unkCC;
        Vec3f center;
        Vec3f hi;
        Vec3f lo;
        Vec3f sum;
        Vec3fAdd(&sum, boxMin, boxMax);
        Vec3f scaled;
        Vec3fScale(&scaled, &sum, 0.5f);
        center = scaled;
        corners[0].set(boxMax->m_x, boxMax->m_y, boxMax->m_z);
        corners[1].set(boxMax->m_x, boxMax->m_y, boxMin->m_z);
        corners[2].set(boxMax->m_x, boxMin->m_y, boxMax->m_z);
        corners[3].set(boxMax->m_x, boxMin->m_y, boxMin->m_z);
        corners[4].set(boxMin->m_x, boxMin->m_y, boxMax->m_z);
        corners[5].set(boxMin->m_x, boxMin->m_y, boxMin->m_z);
        corners[6].set(boxMin->m_x, boxMax->m_y, boxMax->m_z);
        corners[7].set(boxMin->m_x, boxMax->m_y, boxMin->m_z);
        // MATCH-ONLY: the original also fills a second, never read array of the eight corners in reverse order (24 more
        // stores and 0x60 bytes of stack right below the temporaries). Its source is unknown; a plain local array is
        // dropped by the optimizer and laid out differently, the volatile one keeps the stores and the frame size.
        volatile float unusedCorners[8][3];
        unusedCorners[7][0] = boxMax->m_x; unusedCorners[7][1] = boxMax->m_y; unusedCorners[7][2] = boxMax->m_z;
        unusedCorners[6][0] = boxMax->m_x; unusedCorners[6][1] = boxMax->m_y; unusedCorners[6][2] = boxMin->m_z;
        unusedCorners[5][0] = boxMax->m_x; unusedCorners[5][1] = boxMin->m_y; unusedCorners[5][2] = boxMax->m_z;
        unusedCorners[4][0] = boxMax->m_x; unusedCorners[4][1] = boxMin->m_y; unusedCorners[4][2] = boxMin->m_z;
        unusedCorners[3][0] = boxMin->m_x; unusedCorners[3][1] = boxMin->m_y; unusedCorners[3][2] = boxMax->m_z;
        unusedCorners[2][0] = boxMin->m_x; unusedCorners[2][1] = boxMin->m_y; unusedCorners[2][2] = boxMin->m_z;
        unusedCorners[1][0] = boxMin->m_x; unusedCorners[1][1] = boxMax->m_y; unusedCorners[1][2] = boxMax->m_z;
        unusedCorners[0][0] = boxMin->m_x; unusedCorners[0][1] = boxMax->m_y; unusedCorners[0][2] = boxMin->m_z;
        nw4r::g3d::Camera camera = unk40->GetCamera(7);
        camera.SetOrtho(0.5f, -0.5f, -0.5f, 0.5f, 1.0f, 1000.0f);
        camera.SetViewport(0.0f, 0.0f, 1.0f, 1.0f);
        posture.tp = nw4r::g3d::Camera::POSTURE_AIM;
        posture.cameraTarget.x = center.m_x;
        posture.cameraTarget.y = center.m_y;
        posture.cameraTarget.z = center.m_z;
        posture.cameraTwist = m_shadowRoll;
        float sinPitch = sin(0.017453292f * m_shadowPitch);
        float cosPitch = cos(0.017453292f * m_shadowPitch);
        float sinYaw = sin(0.017453292f * m_shadowYaw);
        float cosYaw = cos(0.017453292f * m_shadowYaw);
        sin(0.017453292f * m_shadowRoll);
        cos(0.017453292f * m_shadowRoll);
        float posX = center.m_x + sinYaw * (distance * cosPitch);
        float posY = center.m_y + distance * sinPitch;
        float posZ = center.m_z + cosYaw * (distance * cosPitch);
        camera.SetPosition(posX, posY, posZ);
        camera.SetPosture(posture);
        Vec3f* in = corners;
        Vec3f* out = corners;
        for (u32 i = 0; i < 8; i++, in++, out++) {
            camera.Project(reinterpret_cast<nw4r::math::VEC3*>(out), *reinterpret_cast<nw4r::math::VEC3*>(in));
        }
        lo = corners[0];
        hi = corners[0];
        for (int i = 1; i < 8; i++) {
            hi.m_x = psMaxInPlace(hi.m_x, corners[i].m_x);
            hi.m_y = psMaxInPlace(hi.m_y, corners[i].m_y);
            hi.m_z = psMaxInPlace(hi.m_z, corners[i].m_z);
            lo.m_x = psMinInPlace(lo.m_x, corners[i].m_x);
            lo.m_y = psMinInPlace(lo.m_y, corners[i].m_y);
            lo.m_z = psMinInPlace(lo.m_z, corners[i].m_z);
        }
        float extentX = fabs(hi.m_x - lo.m_x);
        float extentY = fabs(hi.m_y - lo.m_y);
        float halfWidth = psMaxInPlace(1.5f * extentX * 0.5f, 10.0f);
        float halfHeight = psMaxInPlace(1.5f * extentY * 0.5f, 10.0f);
        nw4r::g3d::Camera finalCamera = unk40->GetCamera(7);
        finalCamera.SetOrtho(halfHeight, -halfHeight, -halfWidth, halfWidth, 1.0f, 1000.0f);
        posture.tp = nw4r::g3d::Camera::POSTURE_AIM;
        posture.cameraTarget.x = center.m_x;
        posture.cameraTarget.y = center.m_y;
        posture.cameraTarget.z = center.m_z;
        posture.cameraTwist = m_shadowRoll;
        finalCamera.SetPosition(posX, posY, posZ);
        finalCamera.SetPosture(posture);
    }
}

void gfSceneRoot::renderShadow() {
    if (m_shadowTextures != NULL && m_shadowInfo != NULL && m_shadowInfo->m_numTextures != 0) {
        m_shadowRendering = true;
        reinterpret_cast<gfSceneRootEventList*>(&unk468)->notify(1);
        initShadow(m_shadowInfo->m_mode, &m_shadowInfo->m_boxMin, &m_shadowInfo->m_boxMax);
        nw4r::g3d::ScnRoot* scene = unk48;
        nw4r::g3d::CameraData* shadowCameraData = &scene->GetCamera(7).ref();
        *shadowCameraData = unk40->GetCamera(7).ref();
        nw4r::g3d::Camera camera = scene->GetCamera(7);
        scene->SetCurrentCamera(7);
        gfShadowTexList rendered;
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
        for (u32 i = 0; i < m_shadowInfo->m_numTextures; i++) {
            ShadowTexture* shadow = &m_shadowTextures[i];
            float viewHeight = shadow->m_height;
            float viewWidth = shadow->m_width;
            camera.SetViewport(0.0f, 0.0f, viewWidth, viewHeight);
            camera.SetViewportZRange(0.0f, 0.99999f);
            GXSetZScaleOffset(1.0f, 0.0f);
            GXTexObj texObj = *m_shadowTextures[i].m_texObj;
            void* texData = GXGetTexObjData(&texObj);
            if (i == m_shadowInfo->m_debugTexture) {
                ShowShadowTexture(&texObj);
            }
            bool isNew = rendered.contains(texData);
            if (isNew) {
                nw4r::g3d::G3dReset();
                DrawEFBClear(m_shadowTextures[i].m_width, m_shadowTextures[i].m_height);
                if (m_shadowTextures[i].m_numObjs != 0) {
                    while (!scene->Empty()) {
                        scene->PopBack();
                    }
                    for (u32 j = 0; j < m_shadowTextures[i].m_numObjs; j++) {
                        scene->Insert(scene->Size(), m_shadowTextures[i].m_objs[j]);
                    }
                    scene->CalcView();
                    scene->GatherDrawScnObj();
                    ForceDrawMode__Q34nw4r3g3d7ScnRootFbQ34nw4r3g3d14ResMdlDrawMode(scene, true, 4);
                    // BYTES: a word constant (red 255, alpha 0 for the R8 copy), stored as one word.
                    u32 shadowColorWord = 0xFF000000;
                    SetupSimpleMaterial(*reinterpret_cast<GXColor*>(&shadowColorWord));
                    scene->DrawOpa();
                    ForceDrawMode__Q34nw4r3g3d7ScnRootFbQ34nw4r3g3d14ResMdlDrawMode(scene, false, 2);
                    DrawNonShadowBorder(m_shadowTextures[i].m_width, m_shadowTextures[i].m_height);
                }
                GXSetTexCopySrc(0, 0, m_shadowTextures[i].m_width, m_shadowTextures[i].m_height);
                GXSetTexCopyDst(m_shadowTextures[i].m_width, m_shadowTextures[i].m_height, GX_CTF_R8, false);
                GXCopyTex(texData, true);
                if (isNew) {
                    rendered.push_back(texData);
                }
                GXPixModeSync();
                GXInvalidateTexAll();
            }
        }
        scene->SetCurrentCamera(0);
        while (!scene->Empty()) {
            scene->PopBack();
        }
        m_shadowRendering = false;
    }
}

// BYTES: the original calls SetupSimpleMaterial out of line from all four helpers (and renderShadow); with the
// default auto-inlining it is folded into the helpers defined after it.
extern const float lbl_805A1364;

void gfSceneRoot::drawFrameBufferReverse(int direction) {
    gfFrameBuffer* frameBuffer = gfFrameBuffer::getInstance();
    int width = frameBuffer->m_efbWidth;
    int height = frameBuffer->m_efbHeight;
    nw4r::g3d::CameraData cameraData;
    nw4r::g3d::Camera camera(&cameraData);
    camera.Init();
    camera.SetOrtho(0.0f, height, 0.0f, width, lbl_805A1364, 10.0f);
    camera.SetViewport(0.0f, 0.0f, width, height);
    camera.GXSetViewport();
    camera.GXSetProjection();
    camera.GXSetScissor();
    camera.GXSetScissorBoxOffset();
    GXTexObj* texture = gfCopyEFBMgr::getInstance()->getCopyEFBTex(0);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXLoadTexObj(texture, GX_TEXMAP0);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 60, false, 125);
    GXSetNumTexGens(1);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetNumTevStages(1);
    GXSetNumIndStages(0);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetChanCtrl(GX_COLOR0A0, false, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanCtrl(GX_COLOR1A1, false, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanMatColor(GX_COLOR0A0, nw4r::ut::Color(0xFFFFFFFF));
    GXSetChanAmbColor(GX_COLOR0A0, nw4r::ut::Color(0xFFFFFFFF));
    GXSetNumChans(0);
    GXSetZCompLoc(false);
    GXSetZMode(false, GX_ALWAYS, true);
    GXSetClipMode(GX_CLIP_DISABLE);
    GXSetBlendMode(GX_BM_NONE, GX_BL_ZERO, GX_BL_ZERO, GX_LO_CLEAR);
    Matrix matrix;
    GXLoadPosMtxImm(matrix.m, GX_PNMTX0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    float bottom = height;
    float right = width;
    GXSetCurrentMtx(GX_PNMTX0);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    if (direction == 1) {
        GXPosition3f32(0.0f, 0.0f, -1.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(right, 0.0f, -1.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(right, bottom, -1.0f);
        GXTexCoord2f32(0.0f, 1.0f);
        GXPosition3f32(0.0f, bottom, -1.0f);
        GXTexCoord2f32(1.0f, 1.0f);
    } else {
        GXPosition3f32(0.0f, 0.0f, -1.0f);
        GXTexCoord2f32(0.0f, 1.0f);
        GXPosition3f32(right, 0.0f, -1.0f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(right, bottom, -1.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(0.0f, bottom, -1.0f);
        GXTexCoord2f32(0.0f, 0.0f);
    }
}

gfSceneRoot::LightHandle gfSceneRoot::allocLightHandle(nw4r::g3d::LightObj* light) {
    for (u8 i = 0; i < 10; i++) {
        LightSlot* slot = &m_lightSlots[i];
        if (!slot->m_used) {
            slot->m_light = light;
            slot->m_used = 1;
            LightHandle handle;
            handle.m_index = i;
            handle.m_unk1 = slot->m_unk5;
            return handle;
        }
    }
    LightHandle invalid;
    invalid.m_index = 0xFF;
    invalid.m_unk1 = 0x14;
    return invalid;
}

void gfSceneRoot::releaseLightHandle(LightHandle* handle) {
    if (handle->isValid()) {
        LightSlot& slot = m_lightSlots[handle->m_index];
        slot.m_light = NULL;
        slot.m_used = 0;
        slot.m_released = 1;
        slot.m_unk7 = 0x14;
        slot.m_unk8 = 0x14;
        slot.m_unkA = 0;
        slot.m_unk9 = 0;
        handle->m_index = 0xFF;
        handle->m_unk1 = 0x14;
    }
}

#pragma push
#pragma dont_inline on
static void SetupSimpleMaterial(GXColor color) {
    GXSetNumTexGens(0);
    GXSetTevOp(GX_TEVSTAGE0, GX_TEVOP_PASSCLR);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetNumTevStages(1);
    GXSetNumIndStages(0);
    GXSetChanCtrl(GX_COLOR0A0, false, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanCtrl(GX_COLOR1A1, false, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanMatColor(GX_COLOR0A0, color);
    GXSetChanAmbColor(GX_COLOR0A0, color);
    GXSetNumChans(1);
    GXSetZMode(true, GX_LEQUAL, true);
    GXSetBlendMode(GX_BM_NONE, GX_BL_ZERO, GX_BL_ZERO, GX_LO_COPY);
    gfDrawResetLight();
}

#pragma pop
static void DrawEFBClear(int width, int height) {
    nw4r::g3d::CameraData cameraData;
    nw4r::g3d::Camera camera(&cameraData);
    camera.Init();
    camera.SetOrtho(0.0f, height, 0.0f, width, 1.0f, 100.0f);
    camera.SetViewport(0.0f, 0.0f, width, height);
    camera.SetViewportZRange(0.0f, 0.9999f);
    camera.GXSetViewport();
    camera.GXSetProjection();
    camera.GXSetScissor();
    camera.GXSetScissorBoxOffset();
    GXColor color = {0, 0, 0, 0};
    SetupSimpleMaterial(color);
    GXSetZMode(false, GX_ALWAYS, false);
    Mtx matrix;
    PSMTXIdentity(matrix);
    GXLoadPosMtxImm(matrix, GX_PNMTX0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT7, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    float bottom = height;
    float right = width;
    GXSetCurrentMtx(GX_PNMTX0);
    GXSetZScaleOffset(1.0f, 0.0f);
    GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT7, 4);
    GXPosition3f32(0.0f, 0.0f, -10.0f);
    GXPosition3f32(right, 0.0f, -10.0f);
    GXPosition3f32(0.0f, bottom, -10.0f);
    GXPosition3f32(right, bottom, -10.0f);
}

static void ClearDepthBuffer(int width, int height) {
    nw4r::g3d::CameraData cameraData;
    nw4r::g3d::Camera camera(&cameraData);
    camera.Init();
    camera.SetOrtho(0.0f, height, 0.0f, width, 0.0f, 1.0f);
    camera.SetViewport(0.0f, 0.0f, width, height);
    camera.GXSetViewport();
    camera.GXSetProjection();
    camera.GXSetScissor();
    camera.GXSetScissorBoxOffset();
    GXColor color = {0, 0, 0, 0};
    SetupSimpleMaterial(color);
    GXSetZCompLoc(true);
    GXSetZMode(true, GX_ALWAYS, true);
    GXSetColorUpdate(false);
    GXSetClipMode(GX_CLIP_DISABLE);
    Matrix matrix;
    GXLoadPosMtxImm(matrix.m, GX_PNMTX0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT7, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    float bottom = height;
    float right = width;
    GXSetCurrentMtx(GX_PNMTX0);
    GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT7, 4);
    GXPosition3f32(0.0f, 0.0f, -0.99999f);
    GXPosition3f32(right, 0.0f, -0.99999f);
    GXPosition3f32(0.0f, bottom, -0.99999f);
    GXPosition3f32(right, bottom, -0.99999f);
    GXSetColorUpdate(true);
}

static void DrawNonShadowBorder(int width, int height) {
    nw4r::g3d::CameraData cameraData;
    nw4r::g3d::Camera camera(&cameraData);
    camera.Init();
    camera.SetOrtho(0.0f, height, 0.0f, width, 0.0f, 1.0f);
    camera.SetViewport(0.0f, 0.0f, width, height);
    camera.SetViewportZRange(0.0f, 0.99999f);
    camera.GXSetViewport();
    camera.GXSetProjection();
    camera.GXSetScissor();
    camera.GXSetScissorBoxOffset();
    GXSetZScaleOffset(1.0f, 0.0f);
    GXColor color = {0, 0, 0, 0};
    SetupSimpleMaterial(color);
    GXSetZMode(true, GX_ALWAYS, true);
    Mtx matrix;
    PSMTXIdentity(matrix);
    GXLoadPosMtxImm(matrix, GX_PNMTX0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT7, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetLineWidth(24, 0);
    float bottom = height;
    float right = width;
    GXSetCurrentMtx(GX_PNMTX0);
    GXBegin(GX_LINESTRIP, GX_VTXFMT7, 5);
    GXPosition3f32(0.5f, 0.5f, 0.0f);
    GXPosition3f32(right, 0.5f, 0.0f);
    GXPosition3f32(right, bottom, 0.0f);
    GXPosition3f32(0.5f, bottom, 0.0f);
    GXPosition3f32(0.5f, 0.5f, 0.0f);
}

static void ShowShadowTexture(GXTexObj* texObj) {
    const GXRenderModeObj* mode = nw4r::g3d::G3DState::GetRenderModeObj();
    nw4r::g3d::CameraData cameraData;
    nw4r::g3d::Camera camera(&cameraData);
    camera.Init();
    camera.SetOrtho(0.0f, mode->viHeight, 0.0f, mode->viWidth, 0.0f, 1.0f);
    camera.GXSetViewport();
    camera.GXSetProjection();
    camera.GXSetScissor();
    camera.GXSetScissorBoxOffset();
    GXLoadTexObj(texObj, GX_TEXMAP0);
    GXSetNumTexGens(1);
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetNumIndStages(0);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 60, false, 125);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevOp(GX_TEVSTAGE0, GX_TEVOP_REPLACE);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetZMode(true, GX_LEQUAL, true);
    GXSetBlendMode(GX_BM_NONE, GX_BL_ZERO, GX_BL_ZERO, GX_LO_COPY);
    Mtx matrix;
    PSMTXIdentity(matrix);
    GXLoadPosMtxImm(matrix, GX_PNMTX0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT7, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT7, GX_VA_TEX0, GX_TEX_ST, GX_U8, 0);
    float size = mode->viWidth / 3.0f;
    float left, right, top, bottom;
    left = mode->viWidth - size;
    right = mode->viWidth;
    top = mode->viHeight - size;
    bottom = mode->viHeight;
    GXSetCurrentMtx(GX_PNMTX0);
    GXBegin(GX_QUADS, GX_VTXFMT7, 4);
    GXPosition3f32(left, top, 0.0f);
    GXTexCoord2u8(0, 0);
    GXPosition3f32(right, top, 0.0f);
    GXTexCoord2u8(1, 0);
    GXPosition3f32(right, bottom, 0.0f);
    GXTexCoord2u8(1, 1);
    GXPosition3f32(left, bottom, 0.0f);
    GXTexCoord2u8(0, 1);
}
