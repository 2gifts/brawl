// Local BrawlHeaders shadow: adds scene event, camera and resource animation declarations.
#pragma once

// Shadows include/Brawl/Include/gf/gf_3d_scene.h (adds setInfoCamResAnm / removeInfoCamResAnm).

#include <StaticAssert.h>
#include <stddef.h>
#include <gf/gf_callback.h>
#include <gf/gf_task.h>
#include <nw4r/g3d/g3d_anmscn.h>
#include <nw4r/g3d/g3d_light.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <mt/mt_vector.h>
#include <revolution/GX.h>
#include <types.h>
#include <ut/ut_list.h>

class gfSceneRootEvent;

namespace nw4r {
namespace g3d {
class ScnRoot;
class ScnGroup;
class ScnObj;
} // namespace g3d
} // namespace nw4r

class gfSceneRoot : public gfTask {
public:
    gfSceneRoot();
    static void create();
    static void drawFrameBufferReverse(int direction);

    enum LayerType {
        Layer_Ground = 0x0,
        Layer_Ground_And_Chara = 0x1,
        Layer_Chara = 0x2,
        Layer_All = 0x3,
        Layer_Fighter = 0x3,
        Layer_Stage_Object = 0x3,
        Layer_Ground2 = 0x3,
        Layer_Effect = 0x4,
        Layer_Effect_Fighter = 0x5,
        Layer_Special_Stage_Object = 0x6,
        Layer_Effect2D = 0x7,
        Layer_Info = 0x8,
    };

    // Offsets below come from the constructor, destructor and the render/anim functions of gf_3d_scene.cpp.
    // 0x40..0x48: ScnRoots built by the constructor (Construct(..., 0x138, 0x140, 0x20, 0x20) for 0x40 and 0x44,
    // (8, 8, 0x20, 0x20) for 0x48). 0x40 holds layers 0-6 (stage scene), 0x44 holds layers 7-8 (info scene),
    // 0x48 is the scene used for the loupe and shadow textures.
    nw4r::g3d::ScnRoot* unk40;
    nw4r::g3d::ScnRoot* unk44;
    nw4r::g3d::ScnRoot* unk48;
    nw4r::g3d::ResAnmScnData* unk4C; // data of the ResAnmScn behind m_anmScnRes
    u32 unk50;                       // data of the ResAnmScn behind unk5C (info camera)
    nw4r::g3d::AnmScnRes* m_anmScnRes; // 0x54, scene animation set on unk40
    u32 unk58;                       // cleared by removeResAnmScn
    nw4r::g3d::AnmScnRes* unk5C;     // info camera animation set on unk44
    nw4r::g3d::AnmScnRes* unk60;     // final scene animation set on unk40
    // HYPOTHESIS: the top byte of the word at 0x64 is the direction handed to drawFrameBufferReverse (renderXlu reads
    // it signed and calls it before layer 8 when it is not 0; st_tengan sets it to 1 and mu_g3d_projection tests it).
    // The original bit-by-bit flag names were never used, only m_mask is.
    union TransformFlag {
        u32 m_mask;
        struct {
            s32 m_reverseDirection : 8;
            u32 unk64_8 : 24;
        };
    } m_transformFlag;

    // HYPOTHESIS: per-layer record (layout from the layer functions; the name is not from the map).
    struct Layer {
        Layer();
        ~Layer();
        nw4r::g3d::ScnRoot* unk0;  // the ScnRoot the layer draws with (unk40 for layers 0-6, unk44 for 7-8)
        nw4r::g3d::ScnGroup* unk4; // group of the layer's ScnMdls (G3dProc / add / remove go through it)
        nw4r::g3d::AnmScnRes* unk8;
        u8 unkC;                   // 5 for layers 7 and 8, 0 otherwise
        union {
            struct {
                u8 unkD_7 : 1;
                u8 unkD_6 : 1;
                u8 unkD_4 : 2;
                u8 unkD_0 : 4;
            };
            struct {
                u8 m_display : 1;     // 0x80, layer is drawn (setLayerDispStatus)
                u8 m_clearDepth : 1;  // 0x40, ClearDepthBuffer before the layer (setClearDepthBufferState)
                u8 m_drawEffect : 1;  // 0x20, efManager/efScreen render after the layer (layers 0, 4, 7)
                u8 m_copyEFB : 1;     // 0x10, drawCopyEFB after the layer (layers 3 and 6, by gfCopyEFBMgr mode)
                u8 m_unkD_0 : 4;
            };
        };
        u16 unkE;                  // number of ScnMdls added (add/remove count it, clear zeroes it)
        u32 unk10;
        u32 unk14;
    };
    static_assert(sizeof(Layer) == 0x18, "Layer is wrong size!");

    // HYPOTHESIS: light handle returned by allocLightHandle (slot index and a byte copied from the slot).
    struct LightHandle {
        u8 m_index;
        u8 m_unk1;
        // HYPOTHESIS: inline accessor
        bool isValid() const { return m_index != 0xFF; }
    };
    // HYPOTHESIS: per-light slot at 0x68 + i * 0xC (layout from allocLightHandle/releaseLightHandle/processAnim/renderPre).
    struct LightSlot {
        nw4r::g3d::LightObj* m_light; // 0x0
        u8 m_used : 1;                // 0x4 & 0x80
        u8 m_released : 1;            // 0x4 & 0x40, set while free (ctor, release); renderPre then deselects the zone lights
        u8 m_flags : 6;
        u8 m_unk5;                    // 0x5, index of the slot's LightSet in the ScnRoot's LightSetting (0x16 + i)
        u8 m_unk6;                    // 0x6, index of the slot's LightObj in the ScnRoot's LightSetting (0x12 + i)
        u8 m_unk7;                    // 0x7, zone LightSet in use (0x14 = none), set by updateZoneLightSet
        u8 m_unk8;                    // 0x8, previous zone LightSet
        u8 m_unk9;                    // 0x9, blend counter 0..4 (processAnim)
        u8 m_unkA;                    // 0xA, blend step derived from the counter (processAnim), 0 = none
        u8 m_unkB;
    };
    static_assert(sizeof(LightSlot) == 0xC, "LightSlot is wrong size!");

    // HYPOTHESIS: stand-in with the layout of nw4r::g3d::LightObj (flag word + GXLightObj, 0x44 bytes); the shadow
    // LightObj in include/nw4r/include is empty. The two 0x44-byte arrays of the class are built with the weak
    // LightObj constructor/destructor (fn_8000CEDC / fn_8000CEE8), so a draft has to give them the real type.
    struct LightObjData {
        u32 m_flag;
        u8 m_gxLightObj[0x40];
    };
    static_assert(sizeof(LightObjData) == 0x44, "LightObjData is wrong size!");

    // HYPOTHESIS: two bytes per layer at 0x4F8 (layer index i at 0x4F8 + i * 2).
    struct LayerEfb {
        u8 m_enable;  // != 0: renderXlu calls gfCopyEFBMgr::getEFB(m_efbIndex) after the layer (0x4FE and 0x506 start as 1)
        s8 m_efbIndex; // -1 = none (setUseCopyFB writes the enable byte of layers 4 and 7)
    };

    // HYPOTHESIS: entry of the shadow texture array at m_shadowTextures (0x10 bytes each).
    struct ShadowTexture {
        GXTexObj* m_texObj;           // 0x0
        u16 m_width;                  // 0x4
        u16 m_height;                 // 0x6
        u8 m_numObjs;                 // 0x8
        u8 unk9[3];
        nw4r::g3d::ScnObj** m_objs;   // 0xC, ScnObjs drawn into the texture
    };
    static_assert(sizeof(ShadowTexture) == 0x10, "ShadowTexture is wrong size!");
    // HYPOTHESIS: description the shadow is rendered from (pointed to by m_shadowInfo).
    struct ShadowInfo {
        u32 m_numTextures;  // 0x0
        u32 m_mode;         // 0x4, 0 = fixed top-down camera, else fit the camera to the box below (initShadow's 2nd argument)
        Vec3f m_boxMin;     // 0x8
        Vec3f m_boxMax;     // 0x14
        u32 m_debugTexture; // 0x20, index of the texture ShowShadowTexture draws on screen
    };
    static_assert(sizeof(ShadowInfo) == 0x24, "ShadowInfo is wrong size!");

    LightSlot m_lightSlots[10];      // 0x68
    u8 unkE0;                        // 0xE0
    u8 m_zoneLightObjBase;           // 0xE1, first LightObj index of the zone-light blend (0xC when the scene anim has 0x16 light sets)
    u8 unkE2[2];                     // 0xE2
    LightObjData m_lightObjs[10];    // 0xE4, default-initialised in the constructor (zero color, pos (0,10,0), dir (-1,-1,-1))
    Layer unk38C[9];                 // 0x38C, indexed by LayerType
    u8 unk464_7 : 1;                 // 0x80
    u8 m_shadowEnabled : 1;          // 0x40, renderXlu calls renderShadow
    u8 unk464_5 : 3;                 // 0x20..0x08
    u8 m_shadowRendering : 1;        // 0x04, set while renderShadow runs
    u8 m_skipAnim : 1;               // 0x02, processAnim skips the animation updates (HYPOTHESIS: pause)
    u8 unk464_0 : 1;                 // 0x01, set = camera animation is not used (setUseCameraAnim)
    u8 unk465_7 : 1;                 // 0x80, renderPre sets the info gfCamera (gfCameraManager + 0x604)
    u8 unk465_6 : 1;                 // 0x40, zone light blend active (set when the scene anim has 0x16 light sets)
    u8 unk465_5 : 1;                 // 0x20, renderXlu skips drawCopyEFB
    u8 unk465_4 : 1;                 // 0x10
    u8 unk465_3 : 1;                 // 0x08, HYPOTHESIS: cleared while a Final Smash camera animation plays (ftCaptainFinalSubRoutine); processAnim skips the main animation while set
    u8 unk465_0 : 3;
    u8 unk466[2];
    utList unk468;                   // 0x468, gfSceneRootEventList (notify is called on it by renderXlu/renderShadow)
    ShadowTexture* m_shadowTextures; // 0x474, array of ShadowInfo::m_numTextures entries; written outside this unit
    ShadowInfo* m_shadowInfo;        // 0x478
    float m_shadowPitch;             // 0x47C, degrees; initShadow places the shadow camera with sin/cos of these
    float m_shadowYaw;               // 0x480
    float m_shadowRoll;              // 0x484, camera twist of the shadow camera
    LightObjData m_extraLight;       // 0x488, initialised like m_lightObjs; also pointed to by lbl_8059FF84
    gfCallBackList m_callBacks;      // 0x4CC, process() is called at the end of renderPre
    GXColor m_zoneColors[4];         // 0x4D0, colors of the lights of LightSet 0x14 (zero/0xFF alpha when missing)
    u8 unk4E0[0x18];                 // 0x4E0
    LayerEfb m_layerEfb[9];          // 0x4F8
    u8 unk50A[2];                    // 0x50A

    virtual void processAnim();
    virtual void processFixCamera();
    virtual void renderPre();
    virtual void renderOpa();
    virtual void renderXlu();
    virtual void renderDebug();
    virtual ~gfSceneRoot();

    void setCurrentFrame(float);
    void addEventNotify(gfSceneRootEvent* event);
    void removeEventNotify(gfSceneRootEvent* event);
    void setUseCameraAnim(bool useCameraAnim);
    void setClearDepthBufferState(LayerType layer, bool clear);
    void setResAnmScn(nw4r::g3d::ResAnmScn* anmScn, int heap);
    void removeResAnmScn();
    void setLayerDispStatus(LayerType layer, bool status);
    bool clear(LayerType layer);
    void setAnmScnResToScnRoot3D(nw4r::g3d::AnmScnRes* anmScn);
    void setFinalSceneAnm(nw4r::g3d::ResAnmScn* anmScn);
    void removeFinalSceneAnm();
    void setInfoCamResAnm(nw4r::g3d::ResAnmScn* anmScn);   // the camera of the Info layer (map: gfSceneRoot::setInfoCamResAnm)
    void removeInfoCamResAnm();
    nw4r::g3d::LightObj* getLightObj(u8 p1);
    void updateZoneLightSet(u8 p1, u8 p2);
    void setUseCopyFB(u8 p1);
    LightHandle allocLightHandle(nw4r::g3d::LightObj* light);
    void releaseLightHandle(LightHandle* handle);

    bool add(gfSceneRoot::LayerType lt, nw4r::g3d::ScnMdl* scnMdl);
    bool remove(nw4r::g3d::ScnMdl* scnMdl);
    // HYPOTHESIS: fn_8000D1B8 (map: gfSceneRoot__remove1), the per-layer overload of remove.
    bool remove(gfSceneRoot::LayerType lt, nw4r::g3d::ScnMdl* scnMdl);
    // HYPOTHESIS: fn_8000EB1C (map: gfSceneRoot__createLoupeTexture), renders every enabled loupe into its texture.
    void createLoupeTexture();
    // HYPOTHESIS: fn_8000EFE4 (map: gfSceneRoot__initShadow), sets up the shadow camera (camera 7 of unk40) from the box.
    void initShadow(int mode, const Vec3f* boxMin, const Vec3f* boxMax);
    // HYPOTHESIS: fn_8000F4F0 (map: gfSceneRoot__renderShadow), renders m_shadowTextures with the shadow camera.
    void renderShadow();
    void layerUpdateFrame(int layer);
    void resetScene();
    void blendLightColor(f32 ratio, nw4r::g3d::LightObj* from, nw4r::g3d::LightObj* to, nw4r::g3d::LightObj* out);
};
static_assert(sizeof(gfSceneRoot) == 1292, "Class is wrong size!");
static_assert(offsetof(gfSceneRoot, unk40) == 0x40, "gfSceneRoot::unk40 offset");
static_assert(offsetof(gfSceneRoot, m_transformFlag) == 0x64, "gfSceneRoot::m_transformFlag offset");
static_assert(offsetof(gfSceneRoot, m_anmScnRes) == 0x54, "gfSceneRoot::m_anmScnRes offset");
static_assert(offsetof(gfSceneRoot, m_lightSlots) == 0x68, "gfSceneRoot::m_lightSlots offset");
static_assert(offsetof(gfSceneRoot, m_zoneLightObjBase) == 0xE1, "gfSceneRoot::m_zoneLightObjBase offset");
static_assert(offsetof(gfSceneRoot, m_lightObjs) == 0xE4, "gfSceneRoot::m_lightObjs offset");
static_assert(offsetof(gfSceneRoot, unk38C) == 0x38C, "gfSceneRoot::unk38C offset");
static_assert(offsetof(gfSceneRoot, unk468) == 0x468, "gfSceneRoot::unk468 offset");
static_assert(offsetof(gfSceneRoot, m_shadowTextures) == 0x474, "gfSceneRoot::m_shadowTextures offset");
static_assert(offsetof(gfSceneRoot, m_shadowPitch) == 0x47C, "gfSceneRoot::m_shadowPitch offset");
static_assert(offsetof(gfSceneRoot, m_extraLight) == 0x488, "gfSceneRoot::m_extraLight offset");
static_assert(offsetof(gfSceneRoot, m_callBacks) == 0x4CC, "gfSceneRoot::m_callBacks offset");
static_assert(offsetof(gfSceneRoot, m_zoneColors) == 0x4D0, "gfSceneRoot::m_zoneColors offset");
static_assert(offsetof(gfSceneRoot, m_layerEfb) == 0x4F8, "gfSceneRoot::m_layerEfb offset");

extern gfSceneRoot* g_gfSceneRoot;
