#include <types.h>
// Brawl physics wrapper translation unit ph_shape.o (main.dol 0x80095FC4-0x800962E0).
// Not yet decompiled. Functions in address order with their map names:
//   0x80095FC4   132  __ct
//   0x80096048   128  __dt
//   0x800960C8   164  getMatrixPosture
//   0x8009616C   372  draw
#include <havok/hkEntity.h>
#include <havok/hkMemory.h>
#include <havok/hkQuaternion.h>
#include <havok/hkRigidBody.h>
#include <havok/hkRotation.h>
#include <havok/hkVector4.h>
#include <havok/hkWorld.h>
#include <ph/ph_shape.h>
#include <revolution/GX.h>

// HYPOTHESIS: source object of getMatrixPosture; float blocks at 0xE0 and 0x120.
struct phPostureSource {
    u8 unk00[0xE0];
    hkReal m_e0[4];
    u8 unkF0[0x30];
    hkReal m_120[4];
};

// HYPOTHESIS: the target writes the composed matrix through the shape's own address (fn_80283E28 = hkMatrix4::set).
struct hkMatrix4 {
    void set(const hkRotation& r);
};

// Same allocation as HK_DECLARE_REF_ALLOCATOR(0x2B) for hkRigidBody (size and class id from the target).
static hkRigidBody* allocRigidBody() {
    hkReferencedObject* p = (hkReferencedObject*)hkMemory::getInstance().allocateChunk(0x1F0, 0x2B);
    p->m_memSizeAndFlags = 0x1F0;
    return (hkRigidBody*)p;
}

phShape::phShape(int flag) {
    m_rigidBody = NULL;
    hkReferencedObject* allocation = (hkReferencedObject*)hkMemory::getInstance().allocateChunk(0x1F0, 0x2B);
    allocation->m_memSizeAndFlags = 0x1F0;
    hkRigidBody* rigidBody = (hkRigidBody*)allocation;
    m_rigidBody = new (rigidBody) hkRigidBody(flag);
}

phShape::~phShape() {
    if (m_rigidBody != NULL) {
        hkWorld* world = m_rigidBody->m_world;
        if (world != NULL) {
            world->removeEntity(m_rigidBody);
        }
        m_rigidBody = NULL;
    }
}

void phShape::getMatrixPosture(phPostureSource* const* src) {
    phPostureSource* p = *src;
    hkQuaternion q(p->m_120[0], p->m_120[1], p->m_120[2], p->m_120[3]);
    hkQuaternion unused(p->m_e0[0], p->m_e0[1], p->m_e0[2], p->m_e0[3]);
    hkRotation rot;
    rot.set(q);
    ((hkMatrix4*)this)->set(rot);
}

// Unnamed helpers called from draw (names not recovered yet).
extern "C" void fn_801F471C(int);
extern "C" void fn_801F4748(int);
// Fog colour and parameters (data not recovered yet).
extern u8 lbl_805A1D08;
extern u8 lbl_805A1D09;
extern u8 lbl_805A1D0A;
extern u8 lbl_805A1D0B;
extern float lbl_805A1D0C;
extern float lbl_805A1D10;
extern float lbl_805A1D14;

void G3DState_Invalidate(unsigned long flags);

void phShape::draw() {
    G3DState_Invalidate(0x7FF);
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    fn_801F471C(1);
    fn_801F4748(0);
    GXSetBlendMode(GX_BM_BLEND, (GXBlendFactor)4, (GXBlendFactor)5, (GXLogicOp)5);
    GXSetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_GREATER, 0);
    GXSetZMode(1, GX_LEQUAL, 1);
    GXSetZCompLoc(0);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, (GXTevMode)4);
    GXSetChanCtrl(GX_COLOR0A0, 0, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumChans(1);
    GXSetCullMode(GX_CULL_NONE);
    GXColor fogColor;
    fogColor.r = lbl_805A1D08;
    fogColor.g = lbl_805A1D09;
    fogColor.b = lbl_805A1D0A;
    fogColor.a = lbl_805A1D0B;
    GXSetFog(GX_FOG_NONE, fogColor, lbl_805A1D0C, lbl_805A1D10, lbl_805A1D14, lbl_805A1D10);
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetCurrentMtx(GX_PNMTX0);
    GXSetLineWidth(6, 5);
}

// TODO: clean up, give variables real names
namespace jj_800960C8 {
extern "C" void fn_80285A6C(void*, float*, void*, float, float, float, float);
extern "C" void fn_80283E28(int, void*);

struct Buffer {
    float args[4];
    float source[4];
    char output[0x30];
    float duplicate[4];
} __attribute__((aligned(16)));

// TODO: clean up, give variables real names
extern "C" void fn_800960C8(int arg0, void** arg1) {
    Buffer buffer;
    float* object = (float*)*arg1;
    float source0 = object[0x38];
    float source1 = object[0x39];
    float source2 = object[0x3A];
    float source3 = object[0x3B];

    float argValue0 = object[0x48];
    buffer.source[0] = source0;
    buffer.args[0] = argValue0;
    float argValue1 = object[0x49];
    buffer.source[1] = source1;
    buffer.args[1] = argValue1;
    float argValue2 = object[0x4A];
    buffer.source[2] = source2;
    buffer.args[2] = argValue2;
    float argValue3 = object[0x4B];
    buffer.source[3] = source3;
    buffer.args[3] = argValue3;

    buffer.duplicate[0] = buffer.source[0];
    buffer.duplicate[1] = buffer.source[1];
    buffer.duplicate[2] = buffer.source[2];
    buffer.duplicate[3] = buffer.source[3];

    fn_80285A6C(buffer.output, buffer.args, object,
                buffer.source[3], buffer.source[2],
                buffer.source[1], buffer.source[0]);
    fn_80283E28(arg0, buffer.output);
}
}

// TODO: clean up, give variables real names
namespace jj_8009616C {
#define GXSetFog GXSetFog_decl
#undef GXSetFog
extern "C" void GXSetFog(int, unsigned char*, float, float, float, float);

extern "C" {
void fn_801A12AC(unsigned int);
void fn_801EFB10();
void fn_801EFB44(int, int, int, int, int);
void fn_801F471C(int);
void fn_801F4748(int);
void fn_801EF280(int, int);
void fn_801F52E4(int);
extern unsigned char lbl_805A1D08, lbl_805A1D09, lbl_805A1D0A, lbl_805A1D0B;
extern float lbl_805A1D0C, lbl_805A1D10, lbl_805A1D14;

// TODO: clean up, give variables real names
void fn_8009616C() {
    fn_801A12AC(0x7ff);
    fn_801EFB10();
    fn_801EFB44(1, 9, 1, 4, 0);
    fn_801EFB44(1, 11, 1, 5, 0);
    fn_801F471C(1);
    fn_801F4748(0);
    GXSetBlendMode((GXBlendMode)1, (GXBlendFactor)4, (GXBlendFactor)5, (GXLogicOp)5);
    GXSetAlphaCompare((GXCompare)4, 0, (GXAlphaOp)0, (GXCompare)4, 0);
    GXSetZMode(1, (GXCompare)3, 1);
    GXSetZCompLoc(0);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)255, (GXTexMapID)255, (GXChannelID)4);
    GXSetTevOp((GXTevStageID)0, (GXTevMode)4);
    GXSetChanCtrl((GXChannelID)4, 0, (GXColorSrc)0, (GXColorSrc)1, (GXLightID)0, (GXDiffuseFn)0, (GXAttnFn)2);
    GXSetNumChans(1);
    GXSetCullMode((GXCullMode)0);
    unsigned char color[4];
    color[0] = lbl_805A1D08;
    color[1] = lbl_805A1D09;
    color[2] = lbl_805A1D0A;
    color[3] = lbl_805A1D0B;
    GXSetFog(0, color, lbl_805A1D0C, lbl_805A1D10, lbl_805A1D14, lbl_805A1D10);
    fn_801EF280(9, 1);
    fn_801EF280(11, 1);
    fn_801F52E4(0);
    GXSetLineWidth(6, 5);
}
}
}

