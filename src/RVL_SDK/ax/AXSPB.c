#include "revolution.h"

extern u8 lbl_804E89E0[];
extern u32 lbl_805A0AD8;
extern u32 lbl_805A0ADC;
extern u32 lbl_805A0AE0;
extern u32 lbl_805A0AE4;
extern u32 lbl_805A0AE8;
extern u32 lbl_805A0AEC;
extern u32 lbl_805A0AF0;
extern u32 lbl_805A0AF4;
extern u32 lbl_805A0AF8;
extern u32 lbl_805A0AFC;
extern u32 lbl_805A0B00;
extern u32 lbl_805A0B04;
extern u32 lbl_805A0B08;
extern u32 lbl_805A0B0C;
extern u32 lbl_805A0B10;
extern u32 lbl_805A0B14;
extern u32 lbl_805A0B18;
extern u32 lbl_805A0B1C;
extern u32 lbl_805A0B20;
extern u32 lbl_805A0B24;
extern u32 lbl_805A0B38;
extern u32 lbl_805A0B28;

void* __AXGetStudio(void) {
    return lbl_804E89E0;
}

void __AXSPBInit(void) {
    lbl_805A0AD8 = 0;
    lbl_805A0ADC = 0;
    lbl_805A0AE0 = 0;
    lbl_805A0AE4 = 0;
    lbl_805A0AE8 = 0;
    lbl_805A0AEC = 0;
    lbl_805A0AF0 = 0;
    lbl_805A0AF4 = 0;
    lbl_805A0AF8 = 0;
    lbl_805A0AFC = 0;
    lbl_805A0B00 = 0;
    lbl_805A0B04 = 0;
    lbl_805A0B08 = 0;
    lbl_805A0B0C = 0;
    lbl_805A0B10 = 0;
    lbl_805A0B14 = 0;
    lbl_805A0B18 = 0;
    lbl_805A0B1C = 0;
    lbl_805A0B20 = 0;
    lbl_805A0B24 = 0;
}

void __AXSPBQuit(void) {}

u32 __AXGetNumVoices(void) {
    return lbl_805A0B38;
}

u32 __AXGetPBs(void) {
    return lbl_805A0B28;
}

// TODO: clean up, give variables real names
/* jj_802012DC */
void __AXDepopFadeRmt(int *arg0, int *arg1, short *arg2) {
    int q;

    q = *arg0 / 18;
    if (q != 0) {
        if (q > 20) {
            q = 20;
        }
        if (q < -20) {
            q = -20;
        }
        *arg1 = *arg0;
        *arg0 -= q * 18;
        *arg2 = -q;
        return;
    }
    *arg0 = 0;
    *arg1 = 0;
    *arg2 = 0;
}

