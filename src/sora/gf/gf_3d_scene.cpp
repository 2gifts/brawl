#include <gf/gf_3d_scene.h>
#include <gf/gf_heap_manager.h>
#include <nw4r/g3d/g3d_scnroot.h>
#include <sr/sr_common.h>
#include <types.h>
#include <gf/gf_display.h>
#include <gf/gf_copyefb.h>
#include <nw4r/g3d/g3d_camera.h>
#include <nw4r/ut/ut_Color.h>
#include <mt/mt_matrix.h>

extern const float lbl_805A1364;
#include <gf/gf_3d_scene_event.h>

void gfSceneRoot::addEventNotify(gfSceneRootEvent* event) {
    unk468.addTail(event);
}

void gfSceneRoot::removeEventNotify(gfSceneRootEvent* event) {
    unk468.remove(event);
}

void gfSceneRoot::setUseCameraAnim(bool useCameraAnim) {
    unk464_0 = !useCameraAnim;
}

void gfSceneRoot::setClearDepthBufferState(LayerType layer, bool clear) {
    unk38C[layer].unkD_6 = clear;
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

void gfSceneRoot::create() {
    g_gfSceneRoot = new (Heaps::System) gfSceneRoot;
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


namespace nw4r {
    namespace g3d {
        class ScnRoot;
    }
}

// HYPOTHESIS: the pointer at 0x40 is the ScnRoot that the anim scene was added to.
extern "C" void RemoveAnmScn__Q34nw4r3g3d7ScnRootFv(nw4r::g3d::ScnRoot* scnRoot);

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




gfSceneRoot::Layer::~Layer() {
    nw4r::g3d::ScnGroup* group = unk4;
    while (!group->Empty()) {
        group->PopBack();
    }
    unk4->Destroy();
    unk0 = 0;
}

#include <nw4r/g3d/g3d_scngroup.h>

void gfSceneRoot::layerUpdateFrame(int layer) {
    unk38C[layer].unk4->G3dProc(8, 0, 0);
}

void gfSceneRoot::renderOpa() {
}

void gfSceneRoot::renderDebug() {
}

void gfSceneRoot::updateZoneLightSet(u8 p1, u8 p2) {
    LightSlot* slot = &m_lightSlots[p1];
    if (slot->m_unk7 != p2) {
        slot->m_unk8 = slot->m_unk7;
        slot->m_unk7 = p2;
        slot->m_unk9 = 0;
    }
}

// HYPOTHESIS: the ScnRoot keeps its LightObj array (0x44 bytes each, first 0x12 reserved) at 0x287C.
nw4r::g3d::LightObj* gfSceneRoot::getLightObj(u8 p1) {
    char* lights = *reinterpret_cast<char**>(reinterpret_cast<char*>(unk40) + 0x287C);
    return reinterpret_cast<nw4r::g3d::LightObj*>(lights + (p1 + 0x12) * 0x44);
}

// HYPOTHESIS: bytes 0x500 and 0x506 are two copies of the use-copy-FB flag.
void gfSceneRoot::setUseCopyFB(u8 p1) {
    struct Byte {
        u8 m_value : 8;
    };
    reinterpret_cast<Byte*>(reinterpret_cast<u8*>(this) + 0x500)->m_value = p1;
    reinterpret_cast<Byte*>(reinterpret_cast<u8*>(this) + 0x506)->m_value = p1;
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

#include <gf/gf_memory_pool.h>
#include <gf/gf_loupe.h>

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

// HYPOTHESIS: the callee is a helper in this unit that resets the lights and fogs of a ScnRoot.
extern "C" void fn_8000E5A8(nw4r::g3d::ScnRoot* scnRoot);

void gfSceneRoot::resetScene() {
    fn_8000E5A8(unk40);
}

extern "C" nw4r::g3d::ScnGroup* fn_801AB6CC(MEMAllocator* allocator, u32* size, u32 capacity);

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

// HYPOTHESIS: the LightObj shadow here is empty, so the two LightObj color calls are declared by symbol.
extern "C" void GetLightColor__Q34nw4r3g3d8LightObjCFP8_GXColor(nw4r::g3d::LightObj* obj, GXColor* color);
extern "C" void InitLightColor__Q34nw4r3g3d8LightObjF8_GXColor(nw4r::g3d::LightObj* obj, GXColor color);

// HYPOTHESIS: out = from + (to - from) * ratio per color channel (paired singles, u8 quantized).
void gfSceneRoot::blendLightColor(f32 ratio, nw4r::g3d::LightObj* from, nw4r::g3d::LightObj* to, nw4r::g3d::LightObj* out) {
    GXColor colorFrom;
    GXColor colorTo;
    GXColor colorOut;
    GetLightColor__Q34nw4r3g3d8LightObjCFP8_GXColor(from, &colorFrom);
    GetLightColor__Q34nw4r3g3d8LightObjCFP8_GXColor(to, &colorTo);
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
    InitLightColor__Q34nw4r3g3d8LightObjF8_GXColor(out, colorOut);
}

void gfSceneRoot::processFixCamera() {
    gfLoupe::setup();
}
