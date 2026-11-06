#include <types.h>
// HYPOTHESIS: layout of the Nd context from the offsets the code shows.
typedef struct NdContext {
    void (*callback)(int, int, int);
    void (*cleanupCallback)(void);
    char pad[0x51b8];
    char thread[1];
} NdContext;
typedef struct { char data[0x18]; } OSMutex;
// HYPOTHESIS: the first 0x18 bytes are an OSMutex.
typedef struct { OSMutex mutex; unsigned char rest[0x90]; } NdLockedData;
extern const char* lbl_8048E108[0xb2];
u32 fn_8020FF8C(void);
void DWCi_NdSetError(int, int);
int DWCi_NdCancelAsync(void);
int DWCi_NdGetProgress(int, int);
void OSLockMutex(OSMutex*);
void OSUnlockMutex(OSMutex*);
int OSJoinThread(void*, void*);
int DWCi_IsError();
void DWC_Printf(unsigned long, const char*, ...);

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
OSMutex lbl_805344B0 __attribute__((aligned(8)));
void* lbl_805344C8[4] __attribute__((aligned(8)));
NdLockedData lbl_805344D8 __attribute__((aligned(8)));
unsigned char lbl_80534580[0x18] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80534598[0x80] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
NdContext* lbl_805A0FE8;
int lbl_805A0FEC;
int lbl_805A0FF0[2];
int lbl_805A0FF8;
unsigned char lbl_805A0FFC[0x4] __attribute__((aligned(4))) = { 0 };
void (*lbl_805A1000)(void);
int lbl_805A1004;
unsigned char lbl_805A1008[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1010[0x8] __attribute__((aligned(4))) = { 0 };
void* lbl_805A1018;
void* lbl_805A101C;
unsigned char lbl_805A1020[0x4] __attribute__((aligned(4))) = { 0 };
void (*lbl_805A1024)(void*);
void* (*lbl_805A1028[2])(int, int);
void* lbl_805A1030[2];

// Function bodies below are taken from https://github.com/jj-link/brawl.

extern int DWC_AllocEx(int, int, int);

int DWCi_NdAlloc(int arg0, int arg1) {
    return DWC_AllocEx(10, arg0, arg1);
}

void DWC_Free(s32 a, void* b, s32 c);

void DWCi_NdFree(void* arg0) {
    DWC_Free(0xA, arg0, 0);
}

void DWCi_NdCallback(int a, int b, int c) {
    lbl_805A0FE8->callback(a, b, c);
}



void DWCi_NdCleanupCallback(void) {
    void (*cleanup)(void);
    OSJoinThread(lbl_805A0FE8->thread, 0);
    cleanup = lbl_805A0FE8->cleanupCallback;
    DWC_Free(10, lbl_805A0FE8, 0);
    lbl_805A0FE8 = 0;
    if (cleanup != 0) {
        cleanup();
    }
}



void NdCleanupCallback(void) {
    OSLockMutex(&lbl_805344B0);
    lbl_805A0FEC = 3;
    lbl_805A0FF0[0] = 4;
    OSUnlockMutex(&lbl_805344B0);
    DWC_Printf(1, "Cleanup Complete\n");
}



int DWC_NdCancelAsync(void) {
    if (DWCi_IsError()) {
        return 0;
    }
    OSLockMutex(&lbl_805344B0);
    if (lbl_805A0FEC != 2) {
        DWCi_NdSetError(2, 0);
        OSUnlockMutex(&lbl_805344B0);
        return 0;
    }
    OSUnlockMutex(&lbl_805344B0);
    if (DWCi_NdCancelAsync()) {
        return 1;
    }
    DWCi_NdSetError(2, 0);
    return 0;
}



int DWC_NdGetProgress(int a, int b) {
    if (DWCi_IsError()) {
        return 0;
    }
    OSLockMutex(&lbl_805344B0);
    if (lbl_805A0FEC != 2) {
        OSUnlockMutex(&lbl_805344B0);
        return 0;
    }
    OSUnlockMutex(&lbl_805344B0);
    return DWCi_NdGetProgress(a, b);
}



const char* DWCi_SCUTILGetISOCountryCodeA2(void) {
    u32 addressId = fn_8020FF8C();
    u32 country;
    if (addressId == -1) {
        return 0;
    }
    country = addressId >> 24;
    if (country >= 0xb2) {
        return 0;
    }
    return lbl_8048E108[country];
}



void nhttpCleanupCallback(void) {
    int i;
    void** slot;
    for (i = 0, slot = lbl_805344C8; i < 3; i++, slot++) {
        if (*slot != 0) {
            lbl_805A1024(*slot);
            *slot = 0;
        }
    }
    lbl_805A1024(lbl_805A1018);
    lbl_805A1024(lbl_805A101C);
    lbl_805A1000();
}


int DWC_SvlInit(void) {
    lbl_805A1030[0] = 0;
    return 1;
}



void DWC_SvlCleanup(void) {
    lbl_805A1030[0] = 0;
}

// HYPOTHESIS: layout of the Nd HTTP parameter block from the offsets the code shows.
typedef struct NdHttpParam {
    int type;
    int unk4;
    int unk8;
    void* unkC;
    int unk10;
    void* unk14;
    void* unk18;
    void* unk1C;
} NdHttpParam;

void freeHttpParam(NdHttpParam* param) {
    if (param != 0) {
        switch (param->type) {
        case 0:
        case 1:
            break;
        case 2:
            if (param->unk1C != 0) {
                lbl_805A1024(param->unk1C);
                param->unk1C = 0;
            }
            if (param->unk18 != 0) {
                lbl_805A1024(param->unk18);
                param->unk18 = 0;
            }
            lbl_805A1024(param->unk14);
            param->unk14 = 0;
            break;
        case 3:
            lbl_805A1024(param->unkC);
            param->unkC = 0;
            break;
        }
        lbl_805A1024(param);
    }
}

void* memset(void*, int, unsigned long);
int setupHttp(NdHttpParam**, int*, const char*, void*, int);
int NHTTPStartConnection(int);
int DWCi_NdGetFileListAsync(void*, int, int);
int DWCi_NdGetFileAsync(void*, int, int);

int DWCi_NdGetFileListNumAsync(int arg) {
    NdHttpParam* param;
    int request;
    int connection;
    int started;
    if (!setupHttp(&param, &request, "Y291bnQ*", lbl_80534598, 0x80)) {
        return 0;
    }
    param->type = 1;
    param->unkC = (void*)arg;
    connection = request;
    OSLockMutex(&lbl_805344D8.mutex);
    if (NHTTPStartConnection(connection)) {
        DWCi_NdSetError(3, 0);
        OSUnlockMutex(&lbl_805344D8.mutex);
        started = 0;
    } else {
        lbl_805A1004 = connection;
        OSUnlockMutex(&lbl_805344D8.mutex);
        started = 1;
    }
    if (!started) {
        return 0;
    }
    return 1;
}

int DWC_NdGetFileListNumAsync(int arg) {
    if (DWCi_IsError()) {
        return 0;
    }
    OSLockMutex(&lbl_805344B0);
    if (lbl_805A0FEC != 1) {
        DWCi_NdSetError(2, 0);
        OSUnlockMutex(&lbl_805344B0);
        return 0;
    }
    lbl_805A0FEC = 2;
    OSUnlockMutex(&lbl_805344B0);
    if (DWCi_NdGetFileListNumAsync(arg)) {
        return 1;
    }
    OSLockMutex(&lbl_805344B0);
    lbl_805A0FEC = 1;
    OSUnlockMutex(&lbl_805344B0);
    return 0;
}

int DWC_NdGetFileListAsync(void* list, int count, int arg) {
    if (DWCi_IsError()) {
        return 0;
    }
    OSLockMutex(&lbl_805344B0);
    if (lbl_805A0FEC != 1) {
        DWCi_NdSetError(2, 0);
        OSUnlockMutex(&lbl_805344B0);
        return 0;
    }
    lbl_805A0FEC = 2;
    OSUnlockMutex(&lbl_805344B0);
    memset(list, 0, arg * 0xb0);
    if (DWCi_NdGetFileListAsync(list, count, arg)) {
        return 1;
    }
    OSLockMutex(&lbl_805344B0);
    lbl_805A0FEC = 1;
    OSUnlockMutex(&lbl_805344B0);
    return 0;
}

int DWC_NdGetFileAsync(void* buf, int size, int arg) {
    if (DWCi_IsError()) {
        return 0;
    }
    OSLockMutex(&lbl_805344B0);
    if (lbl_805A0FEC != 1) {
        DWCi_NdSetError(2, 0);
        OSUnlockMutex(&lbl_805344B0);
        return 0;
    }
    lbl_805A0FEC = 2;
    OSUnlockMutex(&lbl_805344B0);
    if (DWCi_NdGetFileAsync(buf, size, arg)) {
        return 1;
    }
    OSLockMutex(&lbl_805344B0);
    lbl_805A0FEC = 1;
    OSUnlockMutex(&lbl_805344B0);
    return 0;
}

int setAttr(int, const char*);

int DWCi_NdSetAttr(const char* attr0, const char* attr1, const char* attr2) {
    int i;
    void** slot;
    OSLockMutex(&lbl_805344D8.mutex);
    for (i = 0, slot = lbl_805344C8; i < 3; i++, slot++) {
        if (*slot != 0) {
            lbl_805A1024(*slot);
            *slot = 0;
        }
    }
    if (setAttr(0, attr0) && setAttr(1, attr1) && setAttr(2, attr2)) {
        OSUnlockMutex(&lbl_805344D8.mutex);
        return 1;
    }
    for (i = 0, slot = lbl_805344C8; i < 3; i++, slot++) {
        if (*slot != 0) {
            lbl_805A1024(*slot);
            *slot = 0;
        }
    }
    OSUnlockMutex(&lbl_805344D8.mutex);
    return 0;
}
