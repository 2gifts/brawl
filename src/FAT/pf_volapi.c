#include <types.h>

// TODO: clean up, give variables real names
extern int lbl_805644C0[];

int fn_803EA188(void) {
    return lbl_805644C0[0x10];
}


extern char *fn_803EA100(s8);
extern int lbl_805644C0[];

s32 fn_803EAE34(s8 arg0, s32 *arg1) {
    char *p;
    u16 v;

    p = fn_803EA100(arg0);
    v = *(u16 *)(p + 0x1F8C);
    if (!(v & 8)) {
        lbl_805644C0[0x10] = 9;
        *(s32 *)(p + 0x1F80) = 9;
        return 9;
    }
    if (v & 2) {
        *arg1 = 1;
    } else {
        *arg1 = 0;
    }
    return 0;
}

