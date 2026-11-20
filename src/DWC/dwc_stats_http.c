#include <string.h>
#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0FE0[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0FE4[0x4] __attribute__((aligned(4))) = { 0 };

// TODO: clean up, give variables real names
void fn_80354760(void) {}

// TODO: clean up, give variables real names
extern char lbl_8048DC08[];

int fn_80355454(int *arg0) {
    char *p = lbl_8048DC08;
    *arg0 = *(int *)(p + 0x6C);
    return *(int *)(p + 0x68);
}

extern char lbl_8048DC08[];
extern void DWC_CancelGHTTPRequest(int);

// TODO: clean up, give variables real names
void fn_80355388(void) {
    int val = *(int *)(lbl_8048DC08 + 8);
    if (val >= 0) {
        DWC_CancelGHTTPRequest(val);
    }
    *(int *)lbl_8048DC08 = 0;
}

// TODO: clean up, give variables real names
/* jj_8035423C */

extern char lbl_8048DC08[];

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
s32 fn_8035423C(void* arg0, void* arg1, s32 arg2, s32 arg3, u32 arg4, s32 arg5, s32 arg6) {
    s32 i;
    s32 count;

    *(u32*)(lbl_8048DC08 + 0xC) = 0;
    if (strlen((char*)arg0) >= 0x20) {
        return 0;
    }
    if (strlen((char*)arg1) != 0x14) {
        return 0;
    }
    if ((arg2 - 5) & 7) {
        return 0;
    }
    if (!(arg3 & 1)) {
        return 0;
    }
    count = 0;
    for (i = 0; i < 32; i++) {
        if (((arg4 >> i) & 1) == 1) {
            count++;
        }
    }
    if (count != 1) {
        return 0;
    }
    memcpy(lbl_8048DC08 + 0x10, arg0, 0x20);
    memcpy(lbl_8048DC08 + 0x30, arg1, 0x14);
    *(s32*)(lbl_8048DC08 + 0x44) = arg2;
    *(s32*)(lbl_8048DC08 + 0x48) = arg3;
    *(u32*)(lbl_8048DC08 + 0x4C) = arg4;
    *(s32*)(lbl_8048DC08 + 0x50) = arg5;
    *(u32*)(lbl_8048DC08 + 0xC) = 1;
    return 1;
}

// TODO: clean up, give variables real names
/* jj_803553C8 */
extern void DWC_Free(int a, int b, int c);
extern void DWCi_GsFree(void *p);
extern void DWC_ShutdownGHTTP(void);
extern char lbl_8048DC08[];

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void DWCi_EncSessionShutdown(void) {
    int a;
    int c;
    a = *(int *)(lbl_8048DC08 + 0x54);
    if (a) {
        DWC_Free(8, a, 0);
        *(int *)(lbl_8048DC08 + 0x54) = 0;
    }
    c = *(int *)(lbl_8048DC08 + 0x60);
    if (c) {
        DWCi_GsFree((void *)c);
        *(int *)(lbl_8048DC08 + 0x64) = 0;
        *(int *)(lbl_8048DC08 + 0x60) = 0;
    }
    DWC_ShutdownGHTTP();
    *(int *)(lbl_8048DC08 + 0) = 2;
    *(int *)(lbl_8048DC08 + 0x4) = 0;
}

