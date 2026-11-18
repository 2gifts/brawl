#include <gf/gf_copyefb.h>
#include <gf/gf_heap_manager.h>
#include <revolution/GX.h>
#include <revolution/OS.h>
#include <sr/sr_common.h>
#include <types.h>

namespace nw4r {
namespace g3d {
namespace G3DState {
const GXRenderModeObj* GetRenderModeObj();
}
} // namespace g3d
} // namespace nw4r

static gfCopyEFBMgr* lbl_805A0070;

extern "C" void fn_8002FDF4() {
    gfCopyEFBMgr* mgr = new (Heaps::SystemFW) gfCopyEFBMgr;
    if (mgr != NULL) {
        mgr->m_104 = false;
        mgr->m_108 = 200;
        mgr->m_copyEFBs[0].m_texBuffer = NULL;
        mgr->m_copyEFBs[0].m_isValid = false;
        mgr->m_copyEFBs[1].m_texBuffer = NULL;
        mgr->m_copyEFBs[1].m_isValid = false;
    }
    lbl_805A0070 = mgr;
}

gfCopyEFBMgr* gfCopyEFBMgr::getInstance() {
    return lbl_805A0070;
}

bool gfCopyEFBMgr::isValid(int index) {
    return m_copyEFBs[index].m_isValid;
}

GXTexObj* gfCopyEFBMgr::getCopyEFBTex(int index) {
    return &m_copyEFBs[index].m_gxTexObj;
}

void gfCopyEFBMgr::getEFB(int index) {
    gfCopyEFB* efb = &m_copyEFBs[index];
    if (efb->m_texBuffer != NULL) {
        GXSetTexCopySrc(0, 0, efb->m_width, efb->m_height);
        GXSetTexCopyDst(efb->m_width, efb->m_height, efb->m_format, FALSE);
        GXCopyTex(efb->m_texBuffer, FALSE);
        efb->m_isValid = true;
    }
}

void gfCopyEFBMgr::createCopyEFB(int index, HeapType heapType) {
    gfCopyEFB* efb = &m_copyEFBs[index];
    if (efb->m_texBuffer == NULL) {
        const GXRenderModeObj* mode = nw4r::g3d::G3DState::GetRenderModeObj();
        efb->m_width = mode->fbWidth;
        efb->m_height = mode->efbHeight;
        efb->m_format = GX_TF_RGB565;
        efb->m_texBufferSize = GXGetTexBufferSize(efb->m_width, efb->m_height, efb->m_format, FALSE, 0);
        efb->m_texBuffer = gfHeapManager::alloc(heapType, efb->m_texBufferSize);
        DCInvalidateRange(efb->m_texBuffer, efb->m_texBufferSize);
        GXInitTexObj(&efb->m_gxTexObj, efb->m_texBuffer, efb->m_width, efb->m_height, efb->m_format, GX_CLAMP, GX_CLAMP, FALSE);
        GXInitTexObjLOD(&efb->m_gxTexObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, FALSE, FALSE, GX_ANISO_1);
        efb->m_isValid = false;
    }
}
