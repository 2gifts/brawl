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

// TODO: clean up, give variables real names
/* jj_803D01E4 */
extern s32 RFLiPickupCharInfo(void* p);
extern void RFLiInitCharModel(void* a0, void* p, void* a4, void* a5, void* a6);

s32 RFLInitCharModel(void* a0, void* a1, void* a2, void* a3, void* a4, void* a5, void* a6) {
    char buf[72];
    s32 ret = RFLiPickupCharInfo(buf);
    if (ret == 0) {
        RFLiInitCharModel(a0, buf, a4, a5, a6);
    }
    return ret;
}

/* jj_803D0108 */
#pragma optimization_level 1
int RFLiGetMaskBufSize(int arg0);

int RFLGetModelBufferSize(int arg0, unsigned int arg1) {
    int count;
    unsigned int one;
    int sh;
    int tmp;
    int base;
    int y;

    one = 1;
    count = 0;
    if (arg1 & (one << count)) {
        count = 1;
    }
    sh = 1;
    if (arg1 & (one << sh)) {
        count += 1;
    }
    sh = 2;
    if (arg1 & (one << sh)) {
        count += 1;
    }
    sh = 3;
    if (arg1 & (one << sh)) {
        count += 1;
    }
    sh = 4;
    if (arg1 & (one << sh)) {
        count += 1;
    }
    sh = 5;
    if (arg1 & (one << sh)) {
        count += 1;
    }
    sh = 6;
    if (arg1 & (one << sh)) {
        count += 1;
    }
    tmp = RFLiGetMaskBufSize(arg0);
    tmp = tmp * count;
    base = count << 5;
    base = (base + 0x1F) & ~0x1F;
    y = tmp + 0x1F;
    tmp = base + 0x10000;
    tmp = tmp + (y & ~0x1F);
    return tmp - 0x7DA0;
}

/* jj_803D063C */
typedef struct {
    unsigned char r, g, b, a;
} Color4;

typedef struct {
    unsigned char unk0;
    int unk4;
    int unk8;
    int unkC;
    Color4 unk10;
    unsigned char unk14;
} Arg;

void GXSetAlphaCompare(int, int, int, int, int);
void GXSetBlendMode(int, int, int, int);
void GXSetZMode(int, int, int);
void GXSetZCompLoc(unsigned char);
void GXSetColorUpdate(int);
void GXSetAlphaUpdate(int);
void GXSetDither(int);
void GXSetDstAlpha(int, int);
void GXSetTevDirect(int);
void GXSetTevSwapMode(int, int, int);
void GXSetTevOrder(int, int, int, int);
void GXSetTevColorIn(int, int, int, int, int);
void GXSetTevColorOp(int, int, int, int, int, int);
void GXSetTevAlphaIn(int, int, int, int, int);
void GXSetTevAlphaOp(int, int, int, int, int, int);
void RFLLoadMaterialSetting(void*);
void GXClearVtxDesc(void);
void GXSetVtxDesc(int, int);
void GXSetVtxAttrFmt(int, int, int, int, int);
void GXSetNumTexGens(int);
void GXSetNumChans(int);
void GXSetChanCtrl(int, int, int, int, int, int, int);
void GXSetChanAmbColor(int, Color4*);
void GXSetChanMatColor(int, Color4*);

#define lbl_8041AD18 ((char*)&lbl_8041AD18)
extern char lbl_8041AD3C[];
extern unsigned char lbl_805A49F0;
extern unsigned char lbl_805A49F1;
extern unsigned char lbl_805A49F2;
extern unsigned char lbl_805A49F3;

void RFLLoadDrawSetting(Arg arg) {
    Color4 c0;
    Color4 c1;

    GXSetAlphaCompare(4, 0, 1, 0, 0);
    GXSetBlendMode(1, 4, 5, 3);
    GXSetZMode(1, 3, 1);
    GXSetZCompLoc(arg.unk14);
    GXSetColorUpdate(1);
    GXSetAlphaUpdate(1);
    GXSetDither(0);
    GXSetDstAlpha(0, 0);
    if (arg.unk0) {
        GXSetTevDirect(1);
        GXSetTevSwapMode(1, 0, 0);
        GXSetTevOrder(1, 0xFF, 0xFF, 4);
        GXSetTevColorIn(1, 0xF, 0, 0xA, 0xF);
        GXSetTevColorOp(1, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn(1, 7, 7, 7, 0);
        GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
        RFLLoadMaterialSetting(lbl_8041AD18);
        GXClearVtxDesc();
        GXSetVtxDesc(9, 2);
        GXSetVtxDesc(0xA, 2);
        GXSetVtxDesc(0xD, 2);
        GXSetVtxAttrFmt(0, 9, 1, 3, 8);
        GXSetVtxAttrFmt(0, 0xA, 0, 3, 0xE);
        GXSetVtxAttrFmt(0, 0xD, 1, 3, 0xD);
        GXSetNumTexGens(1);
        GXSetNumChans(1);
        GXSetChanCtrl(0, 1, 0, 0, arg.unk4, arg.unk8, arg.unkC);
        GXSetChanCtrl(2, 0, 0, 0, 0, 0, 2);
        c0 = arg.unk10;
        GXSetChanAmbColor(0, &c0);
        c1.r = lbl_805A49F0;
        c1.g = lbl_805A49F1;
        c1.b = lbl_805A49F2;
        c1.a = lbl_805A49F3;
        GXSetChanMatColor(0, &c1);
    } else {
        RFLLoadMaterialSetting(lbl_8041AD3C);
        GXClearVtxDesc();
        GXSetVtxDesc(9, 2);
        GXSetVtxDesc(0xA, 2);
        GXSetVtxDesc(0xD, 2);
        GXSetVtxAttrFmt(0, 9, 1, 3, 8);
        GXSetVtxAttrFmt(0, 0xA, 0, 3, 0xE);
        GXSetVtxAttrFmt(0, 0xD, 1, 3, 0xD);
        GXSetNumTexGens(1);
        GXSetNumChans(0);
    }
}
#undef lbl_8041AD18

