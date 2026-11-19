#include <revolution/GX.h>
#include <types.h>

int RFLiGetMaxMaskRsl(int size) {
    switch (size) {
    case 0x40:
    case 0x60:
        return 0x40;
    case 0x80:
    case 0xE0:
        return 0x80;
    case 0x100:
    case 0x1E0:
        return 0x100;
    default:
        return 0x40;
    }
}

u32 RFLiGetMaskBufSize(u32 flags) {
    u32 size = 0;
    if (flags & 0x20) {
        size = 0x800;
    }
    if (flags & 0x40) {
        size += 0x2000;
    }
    if (flags & 0x80) {
        size += 0x8000;
    }
    if (flags & 0x100) {
        size += 0x20000;
    }
    return size;
}

// TODO: clean up, give variables real names
/* jj_803D3D3C */

typedef struct {
    unsigned int unk0, flags4, flags8;
    unsigned short unkC, flagsE, flags10;
} Settings;

typedef struct {
    float x, y, z, w, h;
    int mode;
    unsigned int padding[2];
    GXTexObj texture;
} DrawItem;

extern const GXColor lbl_8041AEA0[];
extern const GXColor lbl_8041AE68[];
extern const unsigned char lbl_805A4A30, lbl_805A4A31, lbl_805A4A32, lbl_805A4A33;

void fn_801F3194(void);
void fn_801F54B0(float *);
void fn_803D3164(void);
void fn_801F3AFC(int);
void fn_801F471C(int);
void fn_801F3F20(int, int, int);
void fn_801F3C30(int, int, int, int, int);
void fn_801F3C70(int, int, int, int, int);
void fn_801F3CB0(int, int, int, int, int, int);
void fn_801F3D08(int, int, int, int, int, int);
void fn_801F3D60(int, const void *);
void fn_801F2FB0(void *, int);
void fn_803D45A8(float, float, float, float, float, int);
void fn_803D333C(unsigned int);
void fn_803D3584(unsigned int, unsigned int);
void fn_801F1E60(void *, int);
void fn_801F0AF0(void);
void fn_801F0A30(void);
void fn_801D76BC(void *, int);
void fn_801F5484(float, float, float, float, float, float);

#define SETUP() \
    GXSetNumTevStages(1); \
    fn_801F3F20(0, 0, 0); \
    GXSetTevOrder(0, 0, 0, 4); \
    fn_801F3C30(0, 15, 15, 15, 2); \
    fn_801F3C70(0, 7, 7, 7, 4); \
    fn_801F3CB0(0, 0, 0, 0, 1, 0); \
    fn_801F3D08(0, 0, 0, 0, 1, 0)

#define DRAW(i) \
    fn_801F2FB0(&items[i].texture, 0); \
    fn_803D45A8(items[i].x, items[i].y, items[i].z, \
                items[i].w, items[i].h, items[i].mode)

void fn_803D3D3C(void *arg0, Settings *settings, DrawItem *items, int size)
{
    float viewport[6];
    GXColor c0, c1;
    unsigned int c2;
    GXColor c3, c4;
    unsigned int c5;
    GXColor base;
    unsigned int index0, index1, index2, index3;

    fn_801F3194();
    fn_801F54B0(viewport);
    fn_803D3164();
    fn_801F3AFC(0);
    fn_801F3AFC(1);
    fn_801F3AFC(2);
    GXSetBlendMode(1, 7, 6, 15);
    fn_801F471C(1);
    index0 = (settings->flags10 >> 9) & 7;
    SETUP();
    c0 = lbl_8041AEA0[index0];
    fn_801F3D60(1, &c0);
    DRAW(5);
    DRAW(6);
    fn_803D333C((settings->flagsE >> 9) & 3);
    DRAW(4);
    index1 = (settings->flags8 >> 24) & 7;
    SETUP();
    c1 = lbl_8041AE68[index1];
    fn_801F3D60(1, &c1);
    DRAW(2);
    DRAW(3);
    fn_803D3584((settings->flags4 >> 23) & 7, settings->flags4 >> 26);
    DRAW(0);
    DRAW(1);
    SETUP();
    base.r = lbl_805A4A30;
    base.g = lbl_805A4A31;
    base.b = lbl_805A4A32;
    base.a = lbl_805A4A33;
    c2 = *(unsigned int *)&base;
    fn_801F3D60(1, &c2);
    DRAW(7);
    fn_801F471C(0);
    fn_801F1E60(arg0, 1);
    fn_801F0AF0();
    GXSetBlendMode(1, 7, 1, 15);
    index2 = (settings->flags10 >> 9) & 7;
    SETUP();
    c3 = lbl_8041AEA0[index2];
    fn_801F3D60(1, &c3);
    DRAW(5);
    DRAW(6);
    fn_803D333C((settings->flagsE >> 9) & 3);
    DRAW(4);
    index3 = (settings->flags8 >> 24) & 7;
    SETUP();
    c4 = lbl_8041AE68[index3];
    fn_801F3D60(1, &c4);
    DRAW(2);
    DRAW(3);
    fn_803D3584((settings->flags4 >> 23) & 7, settings->flags4 >> 26);
    DRAW(0);
    DRAW(1);
    SETUP();
    c5 = *(unsigned int *)&base;
    fn_801F3D60(1, &c5);
    DRAW(7);
    fn_801F471C(1);
    fn_801F0A30();
    fn_801D76BC(arg0, size * size * 2);
    fn_801F1E60(arg0, 1);
    fn_801F0AF0();
    fn_801F5484(viewport[0], viewport[1], viewport[2], viewport[3], viewport[4], viewport[5]);
}

