#include <types.h>

extern u32 lbl_8041ADD0[];
extern u32 lbl_8041AD60[];
extern u8 lbl_8041AD18[];
extern void fn_803D09F8(void* model, void* setting);
extern void fn_803D0F04(void* model, void* setting);

u32 RFLGetFavoriteColor(int index) {
    return lbl_8041ADD0[index];
}

u32 fn_803D0624(const u16* data) {
    return lbl_8041AD60[(*data >> 10) & 7];
}

void RFLDrawOpa(void* model) {
    fn_803D09F8(model, lbl_8041AD18);
}

void RFLDrawXlu(void* model) {
    fn_803D0F04(model, lbl_8041AD18);
}

extern void PSMTXCopy(const f32 src[3][4], f32 dst[3][4]);
extern void PSMTXInvXpose(const f32 src[3][4], f32 dst[3][4]);

void RFLSetMtx(f32 dst[3][4], f32 src[3][4]) {
    f32(*inv)[4] = (f32(*)[4])((u8*)dst + 0x30);
    PSMTXCopy(src, dst);
    PSMTXInvXpose(src, inv);
}
