#include <revolution/MTX/mtx.h>
#include <revolution/GX/GXHardware.h>
#include <gm/gm_global.h>
#include <gf/gf_camera.h>
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

// TODO: clean up, give variables real names
namespace jj_8000CEDC {
extern "C" {
void fn_8000CEDC(int *p) {
    *p = 0;
}
}
}

namespace jj_8000D518 {
struct Inner {
    unsigned char pad[0x38];
    unsigned short value;
};

struct Outer {
    unsigned char pad[0x20];
    Inner *inner;
};

extern "C" unsigned short fn_8000D518(Outer *arg0) {
    return arg0->inner->value;
}
}

namespace jj_8000D524 {
extern "C" unsigned short fn_8000D524(void *arg)
{
    return *(unsigned short *)((char *)*(void **)((char *)arg + 0x20) + 0x3c);
}
}

namespace jj_8000CEE8 {
// Release a heap block when requested, preserving its pointer.
extern "C" void __dl__FPv(void*);

extern "C" {
void* fn_8000CEE8(void* arg0, int arg1) {
    if (arg0 != 0 && arg1 > 0) {
        __dl__FPv(arg0);
    }
    return arg0;
}
}
}

namespace jj_8000D1B8 {
class Class_8042B6D0 {
public:
    virtual void fn_8004805C();
    virtual void fn_8004781C();
    virtual void fn_80046A28();
    virtual void fn_8004810C();
    virtual void fn_800480DC();
    virtual void fn_801AB1C4();
    virtual void fn_801B269C();
    virtual void fn_801B26E4();
    virtual void fn_801AAFDC();
    virtual void fn_801AAFE8();
    virtual void fn_801AB290();
    virtual void fn_801B2EEC();
    virtual void fn_801B3050();
    virtual int fn_801B3248(int arg);
};

struct Entry {
    Class_8042B6D0* obj;
    char pad[0x0a - 4];
    unsigned short cnt;
    char pad2[0x18 - 0x0c];
};

struct Owner {
    char pad[0x390];
    Entry table[16];
};

extern "C" int fn_8000D1B8(Owner* self, int idx, int arg) {
    int ret = 0;
    Class_8042B6D0* obj = self->table[idx].obj;
    if (obj != 0) {
        int r = obj->fn_801B3248(arg);
        ret = r;
        if (r != 0) {
            self->table[idx].cnt = self->table[idx].cnt - 1;
        }
    }
    return ret;
}
}

// TODO: clean up, give variables real names
namespace jj_8001038C {
extern "C" {
void fn_801A7550(void*, void*);
void fn_801A7558(void*);
void fn_801A7A5C(void*, float, float, float, float, float, float);
void fn_801A7ABC(void*, float, float, float, float);
void fn_801A7B58(void*, float, float);
void fn_801A8048(void*);
void fn_801A80CC(void*);
void fn_801A8194(void*);
void fn_801A81B8(void*);
void fn_8001025C(s32*);
void fn_801EC158(void*);
void fn_801F51DC(void*, unsigned int);
void fn_801EFB10();
void fn_801EF280(int, int);
void fn_801EFB44(int, int, int, int, int);
void fn_801F52E4(int);
void fn_801F54D0(float, float);
void GXSetZMode(int, int, int);
void GXBegin(int, int, int);
extern f32 lbl_805A1324, lbl_805A1330, lbl_805A1360, lbl_805A1384, lbl_805A1388;
}
extern "C" void fn_8001038C(s32 arg0, s32 arg1) {
    Mtx sp10;
    u8 sp40[0x110];
    s32 spC;
    s32 sp8;
    f32 temp_f31;
    f32 temp_f30;
    fn_801A7550(&spC, &sp40);
    fn_801A7558(&spC);
    fn_801A7A5C(&spC, lbl_805A1324, (f32)arg1, lbl_805A1324, (f32)arg0, lbl_805A1330, lbl_805A1384);
    fn_801A7ABC(&spC, lbl_805A1324, lbl_805A1324, (f32)arg0, (f32)arg1);
    fn_801A7B58(&spC, lbl_805A1324, lbl_805A1388);
    fn_801A8048(&spC);
    fn_801A80CC(&spC);
    fn_801A8194(&spC);
    fn_801A81B8(&spC);
    sp8 = 0;
    fn_8001025C(&sp8);
    GXSetZMode(0, 7, 0);
    fn_801EC158(&sp10);
    fn_801F51DC(&sp10, 0);
    fn_801EFB10();
    fn_801EF280(9, 1);
    fn_801EFB44(7, 9, 1, 4, 0);
    temp_f31 = (f32)arg1;
    temp_f30 = (f32)arg0;
    fn_801F52E4(0);
    fn_801F54D0(lbl_805A1330, lbl_805A1324);
    GXBegin(0x98, 7, 4);
    WGPIPE.f = lbl_805A1324;
    WGPIPE.f = lbl_805A1324;
    WGPIPE.f = lbl_805A1360;
    WGPIPE.f = temp_f30;
    WGPIPE.f = lbl_805A1324;
    WGPIPE.f = lbl_805A1360;
    WGPIPE.f = lbl_805A1324;
    WGPIPE.f = temp_f31;
    WGPIPE.f = lbl_805A1360;
    WGPIPE.f = temp_f30;
    WGPIPE.f = temp_f31;
    WGPIPE.f = lbl_805A1360;
}
}

namespace jj_80010598 {
extern "C" {
void fn_801A7550(void*, void*);
void fn_801A7558(void*);
void fn_801A7A5C(void*, float, float, float, float, float, float);
void fn_801A7ABC(void*, float, float, float, float);
void fn_801A8048(void*);
void fn_801A80CC(void*);
void fn_801A8194(void*);
void fn_801A81B8(void*);
void fn_8001025C(s32*);
void GXSetZCompLoc(int);
void GXSetZMode(int, int, int);
void fn_801F471C(int);
void fn_801F55EC(int);
void fn_801F51DC(void*, unsigned int);
void fn_801EFB10();
void fn_801EF280(int, int);
void fn_801EFB44(int, int, int, int, int);
void fn_801F52E4(int);
void GXBegin(int, int, int);
extern f32 lbl_805A1324, lbl_805A1330, lbl_805A138C;
}
extern "C" void fn_80010598(s32 arg0, s32 arg1) {
    Matrix sp10(true);
    u8 sp40[0x110];
    s32 spC;
    s32 sp8;
    f32 temp_f31;
    f32 temp_f30;
    fn_801A7550(&spC, &sp40);
    fn_801A7558(&spC);
    fn_801A7A5C(&spC, lbl_805A1324, (f32)arg1, lbl_805A1324, (f32)arg0, lbl_805A1324, lbl_805A1330);
    fn_801A7ABC(&spC, lbl_805A1324, lbl_805A1324, (f32)arg0, (f32)arg1);
    fn_801A8048(&spC);
    fn_801A80CC(&spC);
    fn_801A8194(&spC);
    fn_801A81B8(&spC);
    sp8 = 0;
    fn_8001025C(&sp8);
    GXSetZCompLoc(1);
    GXSetZMode(1, 7, 1);
    fn_801F471C(0);
    fn_801F55EC(1);
    sp10.setIdentity();
    fn_801F51DC(&sp10, 0);
    fn_801EFB10();
    fn_801EF280(9, 1);
    fn_801EFB44(7, 9, 1, 4, 0);
    temp_f31 = (f32)arg1;
    temp_f30 = (f32)arg0;
    fn_801F52E4(0);
    GXBegin(0x98, 7, 4);
    WGPIPE.f = lbl_805A1324;
    WGPIPE.f = lbl_805A1324;
    WGPIPE.f = lbl_805A138C;
    WGPIPE.f = temp_f30;
    WGPIPE.f = lbl_805A1324;
    WGPIPE.f = lbl_805A138C;
    WGPIPE.f = lbl_805A1324;
    WGPIPE.f = temp_f31;
    WGPIPE.f = lbl_805A138C;
    WGPIPE.f = temp_f30;
    WGPIPE.f = temp_f31;
    WGPIPE.f = lbl_805A138C;
    fn_801F471C(1);
}
}

namespace jj_800107A8 {


extern "C" {

void fn_801A7550(void* self, void* value);
void fn_801A7558(void* self);
void fn_801A7A5C(void* self, float f1, float f2, float f3, float f4, float f5, float f6);
void fn_801A7ABC(void* self, float f1, float f2, float f3, float f4);
void fn_801A7B58(void* self, float f1, float f2);
void fn_801A8048(void* self);
void fn_801A80CC(void* self);
void fn_801A8194(void* self);
void fn_801A81B8(void* self);
void fn_801F54D0(float x, float y);
void fn_8001025C(s32* p);
void fn_801EC158(void* p);
void fn_801F51DC(void* p, unsigned int v);
void fn_801EFB10(void);
void fn_801EF280(int, int);
void fn_801EFB44(int, int, int, int, int);
void fn_801F52E4(int);
void GXSetZMode(int, int, int);
void GXSetLineWidth(int, int);
void GXBegin(int, int, int);

extern f32 lbl_805A1324;
extern f32 lbl_805A1330;
extern f32 lbl_805A135C;
extern f32 lbl_805A1380;

}

extern "C" void fn_800107A8(s32 arg0, s32 arg1) {
    Mtx sp10;
    u8 sp40[0x110];
    s32 spC;
    s32 sp8;
    f32 temp_f31;
    f32 temp_f30;

    fn_801A7550(&spC, &sp40);
    fn_801A7558(&spC);
    fn_801A7A5C(&spC, lbl_805A1324, (f32)arg1, lbl_805A1324, (f32)arg0, lbl_805A1324, lbl_805A1330);
    fn_801A7ABC(&spC, lbl_805A1324, lbl_805A1324, (f32)arg0, (f32)arg1);
    fn_801A7B58(&spC, lbl_805A1324, lbl_805A1380);
    fn_801A8048(&spC);
    fn_801A80CC(&spC);
    fn_801A8194(&spC);
    fn_801A81B8(&spC);
    fn_801F54D0(lbl_805A1330, lbl_805A1324);
    sp8 = 0;
    fn_8001025C(&sp8);
    GXSetZMode(1, 7, 1);
    fn_801EC158(&sp10);
    fn_801F51DC(&sp10, 0);
    fn_801EFB10();
    fn_801EF280(9, 1);
    fn_801EFB44(7, 9, 1, 4, 0);
    GXSetLineWidth(0x18, 0);
    temp_f31 = (f32)arg1;
    temp_f30 = (f32)arg0;
    fn_801F52E4(0);
    GXBegin(0xB0, 7, 5);
    WGPIPE.f = lbl_805A135C;
    WGPIPE.f = lbl_805A135C;
    WGPIPE.f = lbl_805A1324;
    WGPIPE.f = temp_f30;
    WGPIPE.f = lbl_805A135C;
    WGPIPE.f = lbl_805A1324;
    WGPIPE.f = temp_f30;
    WGPIPE.f = temp_f31;
    WGPIPE.f = lbl_805A1324;
    WGPIPE.f = lbl_805A135C;
    WGPIPE.f = temp_f31;
    WGPIPE.f = lbl_805A1324;
    WGPIPE.f = lbl_805A135C;
    WGPIPE.f = lbl_805A135C;
    WGPIPE.f = lbl_805A1324;
}
}

namespace jj_8000D8A8 {

extern f32 lbl_805A1330;

class Class_8042B6D0 {
public:
    virtual void fn_8004805C();
    virtual void fn_8004781C();
    virtual void fn_80046A28();
    virtual void fn_8004810C();
    virtual void fn_800480DC();
    virtual void fn_801AB1C4();
    virtual void fn_801B269C();
    virtual void fn_801B26E4(float);
    virtual void fn_801AAFDC();
    virtual void fn_801AAFE8();
};

struct Entry {
    char pad[4];
    u8 flags;
    char pad2[2];
    u8 current, previous, count, value;
    char pad3;
};

struct Object {
    char pad[0x54];
    Class_8042B6D0* a;
    char pad2[4];
    Class_8042B6D0* b;
    Class_8042B6D0* c;
    char pad3[4];
    Entry entries[10];
};

extern "C" void fn_8000D8A8(Object* self) {
    if (!((*((u8*)g_gfSceneRoot + 0x464) >> 1) & 1)) {
        if (!((*((u8*)g_gfSceneRoot + 0x465) >> 3) & 1)) {
            Class_8042B6D0* p = self->c;
            if (!p) {
                p = self->a;
                if (p) {
                    float rate = lbl_805A1330 / (float)g_GameGlobal->getSlowRate();
                    float motionRatio = g_GameGlobal->m_stageData->m_motionRatio;
                    float speed = rate * motionRatio;
                    speed *= g_GameGlobal->m_stageData->m_motionSubRatio;
                    p->fn_801B26E4(speed);
                    p->fn_801AAFE8();
                }
            } else if (p) {
                float rate = lbl_805A1330 / (float)g_GameGlobal->getSlowRate();
                float motionRatio = g_GameGlobal->m_stageData->m_motionRatio;
                float speed = rate * motionRatio;
                speed *= g_GameGlobal->m_stageData->m_motionSubRatio;
                p->fn_801B26E4(speed);
                p->fn_801AAFE8();
            }
        }

        Class_8042B6D0* p = self->b;
        float f = lbl_805A1330;
        if (p) {
            float motionRatio = g_GameGlobal->m_stageData->m_motionRatio;
            float speed = f * motionRatio;
            speed *= g_GameGlobal->m_stageData->m_motionSubRatio;
            p->fn_801B26E4(speed);
            p->fn_801AAFE8();
        }
    }

    for (int i = 0; i < 10; i++) {
        Entry* e = &self->entries[i];
        if ((e->flags >> 7) & 1) {
            if (e->current != e->previous)
                e->count++;
            if (e->count > 3) {
                e->count = 0;
                e->previous = e->current;
            }
            if (e->current != e->previous) {
                if (e->current > e->previous)
                    e->value = e->count;
                else
                    e->value = 4 - e->count;
            } else {
                e->value = 0;
            }
        }
    }
}
}

namespace jj_8000E214 {
class Class_80466320;
class Class_80466278 {
public:
virtual void fn_801AC270(); virtual void fn_801AB9D8(); virtual void fn_801AC1AC(); virtual void fn_801AC328(); virtual void fn_801AC2F8(); virtual void fn_801AB788(); virtual void fn_801AAC9C(); virtual void fn_801AAE34(); virtual void fn_801AAFDC(); virtual void fn_801AAFE8(); virtual void fn_801AAA70(); virtual void fn_801ABE70(unsigned int, Class_80466320*); virtual void fn_801ABFFC(unsigned int);
char pad[0xe0]; unsigned int count;
};
class Class_80466320 {
public:
virtual void fn_801ADE54(); virtual void fn_801AC4A8(); virtual void fn_801AD064(); virtual void fn_801ADEEC(); virtual void fn_801ADEBC(); virtual void fn_801AB788(); virtual void fn_801AAC9C(); virtual void fn_801AAE34(); virtual void fn_801AAFDC(); virtual void fn_801AAFE8(); virtual void fn_801AAA70(); virtual void fn_801ABE70(unsigned int, void*); virtual void fn_801ABFFC(unsigned int);
char pad[0xe0]; unsigned int count;
};
struct Layer { Class_80466278* a; Class_80466320* b; char pad[5]; unsigned char flags; unsigned short active; char tail[8]; };
struct Capture { unsigned char enabled; signed char index; };
struct Root { char pad0[0x64]; union { int state; struct { signed int high : 8; unsigned int low : 24; } bits; }; char pad68[0x324]; Layer layers[9]; char flags; char flags2; char pad466[2]; gfSceneRootEventList events; char pad46c[0x8c]; Capture capture[9]; };
struct Dimensions { char pad[0x34]; unsigned short w; unsigned short h; };
struct Entry { unsigned char index; char pad[3]; void* item; };
struct Queue { unsigned char count; char pad[3]; Entry entries[1]; };
extern Queue lbl_80494E00; extern void* lbl_805A0110; extern void* g_efScreen;
extern "C" {
void fn_801F55EC(int); void fn_8000F4F0(Root*); void fn_8000EB1C(Root*); void fn_8000FD34(int); Dimensions* fn_80023AC8(); void fn_80010598(unsigned short, unsigned short); void fn_801ACBEC(Class_80466278*); void fn_801ACC64(Class_80466278*); void fn_801ACD50(Class_80466278*); void fn_801A12AC(int); void fn_801ACE98(Class_80466278*); void fn_8005AD04(); void fn_8005B838(void*, int); void fn_8005DC24(void*, unsigned char); void fn_801F0AF0(); void fn_80025314();
}
extern "C" void fn_8000E214(Root* self) {
    gfCopyEFBMgr* copy = gfCopyEFBMgr::getInstance();
    fn_801F55EC(0);
    self->layers[6].flags &= ~0x10;
    self->layers[3].flags &= ~0x10;
    gfCopyEFBMgr* current = gfCopyEFBMgr::getInstance();
    if (current) {
        switch (*(unsigned char*)((char*)current + 0x68)) {
        case 1: self->layers[6].flags |= 0x10; break;
        case 2: self->layers[3].flags |= 0x10; break;
        }
    }
    if ((self->flags >> 6) & 1) fn_8000F4F0(self);
    self->events.notify(0);
    fn_8000EB1C(self);
    self->events.notify(0);
    for (unsigned int i = 0; i < 9; i++) {
        Layer* layer = &self->layers[(unsigned char)i];
        if ((unsigned char)i == 8 && self->bits.high)
            fn_8000FD34(self->state >> 24);
        if (layer->active) {
            if (copy && (unsigned char)i == 6) {
                Dimensions* d = fn_80023AC8();
                fn_80010598(d->w, d->h);
            }
            if ((layer->flags >> 6) & 1) {
                Dimensions* d = fn_80023AC8();
                fn_80010598(d->w, d->h);
            }
            if (((layer->flags >> 7) & 1) && layer->b->count) {
                layer->a->fn_801ABE70(layer->a->count, layer->b);
                fn_801ACBEC(layer->a);
                fn_801ACC64(layer->a);
                fn_801ACD50(layer->a);
                fn_801A12AC(0x7ff);
                fn_801ACE98(layer->a);
                Class_80466278* a = layer->a;
                while (a->count) {
                    if (a->count) a->fn_801ABFFC(a->count - 1);
                }
            }
        }
        if ((layer->flags >> 5) & 1) {
            if ((unsigned char)i == 0) {
                if (!lbl_805A0110) fn_8005AD04();
                fn_8005B838(lbl_805A0110, 0);
            } else if ((unsigned char)i == 4) {
                if (!lbl_805A0110) fn_8005AD04();
                fn_8005B838(lbl_805A0110, 1);
            } else if ((unsigned char)i == 7) {
                if (!lbl_805A0110) fn_8005AD04();
                fn_8005B838(lbl_805A0110, 2);
            }
        }
        if (((layer->flags >> 4) & 1) && copy && !((self->flags2 >> 5) & 1)) {
            unsigned int color = *(unsigned int*)((char*)copy + 0x6c);
            GXSetZMode(0, GX_LEQUAL, 0);
            fn_801F0AF0();
            GXColor c = *(GXColor*)&color;
            copy->drawCopyEFB(0, &c);
            GXSetZMode(1, GX_LEQUAL, 1);
        }
        fn_8005DC24(g_efScreen, (unsigned char)i);
        if (self->capture[(unsigned char)i].enabled &&
            self->capture[(unsigned char)i].index >= 0 && copy)
            copy->getEFB(self->capture[(unsigned char)i].index);
        if ((unsigned char)i == 0) fn_80025314();
    }
    Queue* entry;
    Queue* queue = &lbl_80494E00;
    if (queue->count) {
        entry = queue;
        unsigned int i = 0;
        while (i < queue->count) {
            Class_80466320* b = self->layers[entry->entries[0].index].b;
            b->fn_801ABE70(b->count, entry->entries[0].item);
            entry = (Queue*)((char*)entry + 8);
            i++;
        }
        queue->count = 0;
    }
    ((gfCamera*)gfCameraManager::getManager())->setGX();
}
}

