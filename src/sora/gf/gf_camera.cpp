#include <gf/gf_camera.h>
#include <mt/mt_matrix.h>
#include <revolution/GX.h>
#include <revolution/MTX.h>
#include <types.h>

struct gfCameraLayout {
    char unk0[0xD8];
    f32 unkD8;
    char unkDC[8];
    f32 unkE4;
    char unkE8[0x2C];
    f32 unk114;
    f32 unk118;
    f32 unk11C;
    f32 unk120;
    f32 unk124;
    f32 unk128;
    u16 unk12C;
    u16 unk12E;
    u16 unk130;
    u16 unk132;
};

extern f32 lbl_805A1434;

void gfCamera::setGX() {
    gfCameraLayout* layout = (gfCameraLayout*)this;
    Mtx44 proj;

    GXSetScissor(layout->unk12C, layout->unk12E, layout->unk130, layout->unk132);
    GXSetViewport(layout->unk114, layout->unk118, layout->unk11C, layout->unk120, layout->unk124, layout->unk128);
    GXSetScissorBoxOffset(0, 0);
    if (m_projectionType == Projection_Orthogonal) {
        C_MTXOrtho(proj, m_orthoParam.m_top, m_orthoParam.m_bottom, m_orthoParam.m_left, m_orthoParam.m_right, m_near, m_far);
        GXSetProjection(proj, GX_ORTHOGRAPHIC);
    } else {
        C_MTXPerspective(proj, lbl_805A1434 * layout->unkD8, layout->unkE4, m_near, m_far);
        GXSetProjection(proj, GX_PERSPECTIVE);
    }
    GXLoadPosMtxImm(*(Matrix*)this, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
}
