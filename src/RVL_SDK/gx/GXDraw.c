// gx draw helpers: just the sphere (the cylinder from the stock sdk isn't in this build). it grabs the current
// vertex layout, swaps in a pos+nrm(+tex0) one, emits triangle strips per stack, then puts the layout back.
#include "revolution/gx.h"
#include "revolution/gx/GXRegs.h"

extern double sin(double);
extern double cos(double);
static inline f32 sinf(f32 x) { return (f32)sin(x); }
static inline f32 cosf(f32 x) { return (f32)cos(x); }

extern void GXGetVtxDesc(GXAttr attr, GXAttrType* type);
extern void GXGetVtxDescv(GXVtxDescList* list);
extern void GXGetVtxAttrFmtv(GXVtxFmt fmt, GXVtxAttrFmtList* list);
extern void GXSetVtxDescv(const GXVtxDescList* list);

// +1 for the terminator entry
static GXVtxDescList vcd[27];
static GXVtxAttrFmtList vat[27];

#define PI_F 3.1415927f

void GXDrawSphere(u32 stacks, u32 sectors) {
    GXAttrType tex0, tex0_copy;
    f32 radius;
    f32 stackStep, sectorStep;
    f32 stackAngle, stackAngleNext;
    f32 now_xy, next_xy;
    f32 now_z, next_z;
    f32 sectorAngle;
    f32 cosv, sinv;
    int i, j;

    radius = 1.0f;
    stackStep = PI_F / stacks;
    sectorStep = 2 * PI_F / sectors;

    // texcoord on or off decides if we emit st
    GXGetVtxDesc(GX_VA_TEX0, &tex0);

    GXGetVtxDescv(vcd);
    GXGetVtxAttrFmtv(GX_VTXFMT3, vat);

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT3, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT3, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);

    if (tex0 != GX_NONE) {
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetVtxAttrFmt(GX_VTXFMT3, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    }

    for (i = 0; i < (int)stacks; i++) {
        stackAngle = i * stackStep;
        stackAngleNext = stackAngle + stackStep;

        now_xy = radius * sinf(stackAngle);
        next_xy = radius * sinf(stackAngleNext);
        now_z = radius * cosf(stackAngle);
        next_z = radius * cosf(stackAngleNext);

        GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT3, (sectors + 1) * 2);
        {
            tex0_copy = tex0;

            for (j = 0; j <= (int)sectors; j++) {
                sectorAngle = j * sectorStep;
                cosv = cosf(sectorAngle);
                sinv = sinf(sectorAngle);

                // far ring vertex: position, normal, st
                GX_WRITE_F32(cosv * next_xy);
                GX_WRITE_F32(sinv * next_xy);
                GX_WRITE_F32(next_z);
                GX_WRITE_F32(cosv * next_xy / radius);
                GX_WRITE_F32(sinv * next_xy / radius);
                GX_WRITE_F32(next_z / radius);

                if (tex0_copy != GX_NONE) {
                    GX_WRITE_F32((f32)j / sectors);
                    GX_WRITE_F32((f32)(i + 1) / stacks);
                }

                // near ring vertex
                GX_WRITE_F32(cosv * now_xy);
                GX_WRITE_F32(sinv * now_xy);
                GX_WRITE_F32(now_z);
                GX_WRITE_F32(cosv * now_xy / radius);
                GX_WRITE_F32(sinv * now_xy / radius);
                GX_WRITE_F32(now_z / radius);

                if (tex0_copy != GX_NONE) {
                    GX_WRITE_F32((f32)j / sectors);
                    GX_WRITE_F32((f32)i / stacks);
                }
            }
        }
    }

    GXSetVtxDescv(vcd);
    GXSetVtxAttrFmtv(GX_VTXFMT3, vat);
}
