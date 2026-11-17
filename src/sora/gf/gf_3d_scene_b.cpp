#include <gf/gf_3d_scene.h>
#include <gf/gf_3d_scene_event.h>
#include <gf/gf_camera.h>
#include <gf/gf_draw.h>
#include <gf/gf_display.h>
#include <gf/gf_copyefb.h>
#include <gf/gf_loupe.h>
#include <nw4r/g3d/g3d_scnroot.h>
#include <nw4r/g3d/g3d_camera.h>
#include <nw4r/ut/ut_Color.h>
#include <mt/mt_matrix.h>
#include <math.h>
#include <string.h>
#include <types.h>

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

// Defined in gf_light.cpp (the loupe state).
extern unsigned char lbl_80494E00[0xe8];
extern u32 lbl_805A0010;
extern u32 lbl_805A0014;
extern gfLoupe::LoupeInfo lbl_80494EE8[7];

// HYPOTHESIS: gfSceneRoot.cpp defines drawLoupeBG (fn_8000E8C8) as a file-local helper; it is declared here
// until both parts of the unit are merged (no `this`, the color is passed by value).
static void drawLoupeBG(GXColor color, float radius, float scrollU, float scrollV);

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
static void ClearDepthBuffer(int width, int height);
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
