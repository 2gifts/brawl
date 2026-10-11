#include "revolution/gx.h"
#include "revolution/os.h"
#include "revolution/tpl.h"

#define TPL_VERSION_NUMBER 0x0020AF30

// turns the file-relative offsets in a texture palette into real pointers.
// the unpacked flags make it safe to call twice on the same palette.
void TPLBind(TPLPalettePtr pal) {
    u16 i;

    if (pal->versionNumber != TPL_VERSION_NUMBER) {
        OSPanic("TPL.c", 25, "invalid version number for texture palette");
    }

    pal->descriptorArray = (TPLDescriptorPtr)((u32)pal->descriptorArray + (u32)pal);

    for (i = 0; i < pal->numDescriptors; i++) {
        if (pal->descriptorArray[i].textureHeader) {
            pal->descriptorArray[i].textureHeader =
                (TPLHeaderPtr)((u32)pal + (u32)pal->descriptorArray[i].textureHeader);

            if (!pal->descriptorArray[i].textureHeader->unpacked) {
                pal->descriptorArray[i].textureHeader->data =
                    (char *)((u32)pal + (u32)pal->descriptorArray[i].textureHeader->data);
                pal->descriptorArray[i].textureHeader->unpacked = 1;
            }
        }

        if (pal->descriptorArray[i].CLUTHeader) {
            pal->descriptorArray[i].CLUTHeader =
                (TPLClutHeaderPtr)((u32)pal + (u32)pal->descriptorArray[i].CLUTHeader);

            if (!pal->descriptorArray[i].CLUTHeader->unpacked) {
                pal->descriptorArray[i].CLUTHeader->data =
                    (char *)((u32)pal + (u32)pal->descriptorArray[i].CLUTHeader->data);
                pal->descriptorArray[i].CLUTHeader->unpacked = 1;
            }
        }
    }
}

TPLDescriptorPtr TPLGet(TPLPalettePtr pal, u32 id) {
    return &pal->descriptorArray[id % pal->numDescriptors];
}

// HYPOTHESIS: name follows the stock SDK API (TPLGetGXTexObjFromPalette); the body is exactly
// "GXInitTexObj + GXInitTexObjLOD from one descriptor", no other evidence in-repo.
void TPLGetGXTexObjFromPalette(TPLPalettePtr pal, GXTexObj *to, u32 id) {
    TPLDescriptorPtr desc = pal->descriptorArray;
    u32 idx = id % pal->numDescriptors;
#define TPL_HDR (desc[idx].textureHeader)
    GXInitTexObj(to, TPL_HDR->data, TPL_HDR->width, TPL_HDR->height, (GXTexFmt)TPL_HDR->format,
                 TPL_HDR->wrapS, TPL_HDR->wrapT, TPL_HDR->minLOD != TPL_HDR->maxLOD ? GX_TRUE : GX_FALSE);
    GXInitTexObjLOD(to, TPL_HDR->minFilter, TPL_HDR->magFilter, TPL_HDR->minLOD, TPL_HDR->maxLOD,
                    TPL_HDR->LODBias, GX_FALSE, TPL_HDR->edgeLODEnable, GX_ANISO_1);
#undef TPL_HDR
}
