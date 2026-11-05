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
