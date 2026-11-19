// DWC NP utility functions (not decompiled yet).

#include <revolution/OS/OS.h>
// TODO: clean up, give variables real names
/* jj_8034F020 */
extern int lbl_805A0FB8;

void fn_8034F020(int arg0) {
    lbl_805A0FB8 = arg0;
}

// TODO: clean up, give variables real names
/* jj_8034F684 */
extern int VFIsAvailable(void);
extern int DWC_AllocEx(int, int, int);
extern void VFInitEx(int, int);
extern int NWC24GetMyUserId(int *);
extern void DWC_Printf(int, const char *, ...);
extern void VFFinalize(void);
extern void DWC_Free(int, int, int);
extern int lbl_805A0FBC;
extern int lbl_805A0FC0;
extern int lbl_805A0FC4;
extern const char lbl_8048D2A0[];
extern const char lbl_8048D2C4[];

unsigned long long DWCi_Np_GetConsoleId(void) {
    int r30, r29, r28;
    if (lbl_805A0FBC == 0) {
        r30 = VFIsAvailable();
        r29 = DWC_AllocEx(3, 0x4000, 0x20);
        r28 = DWC_AllocEx(3, 0x8000, 0x20);
        lbl_805A0FBC = 1;
        if (r30 != 1)
            VFInitEx(r28, 0x8000);
        if (NWC24GetMyUserId(&lbl_805A0FC0) == 0)
            DWC_Printf(0x08000000, lbl_8048D2A0, lbl_805A0FC0, lbl_805A0FC4);
        else {
            DWC_Printf(0x08000000, lbl_8048D2C4);
            lbl_805A0FC4 = 0;
            lbl_805A0FC0 = 0;
        }
        if (r30 != 1)
            VFFinalize();
        DWC_Free(3, r29, 0);
        DWC_Free(3, r28, 0);
    }
    return ((unsigned long long)(unsigned int)lbl_805A0FC0 << 32) | (unsigned int)lbl_805A0FC4;
}

// TODO: clean up, give variables real names
/* jj_8034F79C */
extern int lbl_8059F1B8;

int* DWCi_Np_GetConsoleName(void) {
    return &lbl_8059F1B8;
}

// TODO: clean up, give variables real names
/* jj_8034F7A4 */

const char* DWCi_Np_GetAppGameName(void) {
    return OSGetAppGamename();
}

