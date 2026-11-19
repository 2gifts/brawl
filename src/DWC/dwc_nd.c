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

// TODO: clean up, give variables real names
extern void fn_80352260(int, void *, void *);
extern void fn_80335390(void);
extern void fn_80335450(void);

// TODO: clean up, give variables real names
void fn_8035874C(int arg0, int arg1) {
    lbl_805A1030[0] = (void*)arg1;
    fn_80352260(arg0, fn_80335390, fn_80335450);
}

// TODO: clean up, give variables real names
/* jj_80356738 */
extern const char lbl_80415720[];

static inline int getblock(u8 *out, const char *in, int size) {
    int n = 0;
    int i;
    for (i = 0; i < (size > 3 ? 3 : size); i++) {
        if (*in) {
            *out++ = *in++;
            n++;
        } else {
            *out++ = 0;
        }
    }
    return n;
}

// TODO: clean up, give variables real names
int fn_80356738(s8 *out, const char *in, int size) {
    u8 buf[3];
    int len = 0;
    int n = getblock(buf, in, size);
    while (n > 0) {
        int a, b, c, d;
        if (size < 3) buf[2] = 0;
        if (size < 2) buf[1] = 0;
        a = buf[0] >> 2;
        b = ((buf[0] << 4) | (buf[1] >> 4)) & 0x3F;
        c = ((buf[1] << 2) | (buf[2] >> 6)) & 0x3F;
        d = buf[2] & 0x3F;
        out[0] = lbl_80415720[a];
        out[1] = lbl_80415720[b];
        out[2] = n > 1 ? lbl_80415720[c] : 42;
        out[3] = n > 2 ? lbl_80415720[d] : 42;
        size -= n;
        in += n;
        out += 4;
        len += 4;
        if (size == 0) break;
        n = getblock(buf, in, size);
    }
    *out = 0;
    return len;
}

/* jj_80358490 */
typedef struct { char a[0x22]; char b[0x64]; short end; char c[11], d[11], e[11]; char pad; int number; } Entry;
typedef struct { char pad[12]; Entry *entries; int count; } Context;
extern char lbl_8048E650[];
extern void *memset(void *, int, unsigned long);
extern int fn_803968E4(void *, const char *, char **);
extern int fn_803960EC(void *, char **, int *);
extern void fn_8034F028(void *, const void *, unsigned int);
extern unsigned int fn_803568D0(void *, const char *, int);
extern int fn_80392AB8(const char *, int);
// TODO: clean up, give variables real names
int fn_80358490(void *arg0, Context *ctx) {
 char *body;
 int ignored;
 char *value;
 double buffer[13];
 int size, i, pos, field, start, cr, n;
 int result = 0;
 unsigned int len;
 Entry *entry;
 memset(ctx->entries, 0, ctx->count * 0xb0);
 if (fn_803968E4(arg0, lbl_8048E650, &value) > 0 && value[0] == '0' && value[1] == 0) return 1;
 size = fn_803960EC(arg0, &body, &ignored);
 if (size > 0) {
  pos = 0;
  for (i = 0; i < ctx->count; i++) {
   if (pos >= size) { result = 1; break; }
   entry = &ctx->entries[i];
   start = pos;
   field = 0;
   cr = 0;
   result = 0;
   while (pos < size) {
    if (body[pos] == '\r') { cr = 1; }
    else if (cr) {
     if (body[pos] == '\n') {
      if (field == 5) {
       if (pos - 1 - start > 8) goto done;
       n = fn_80392AB8(body + start, pos - 1 - start);
       if (n < 0) goto done;
       entry->number = n;
      }
      pos++;
      result = field >= 5;
      break;
     }
     goto reset;
    } else {
     if (body[pos] == '\t') {
      switch (field) {
       case 0:
        if ((unsigned int)(pos-start) > 0x20) goto done;
        fn_8034F028(entry->a, body+start, pos-start); break;
       case 1:
        n = pos-start;
        if ((unsigned int)((n & ~3) - (n >> 2)) > 0x66) goto done;
        len = fn_803568D0(buffer, body+start, n);
        fn_8034F028(entry->b, buffer, len > 0x66 ? 0x66 : len);
        entry->end = 0; break;
       case 2:
        if ((unsigned int)(pos-start) > 10) goto done;
        fn_8034F028(entry->c, body+start, pos-start); break;
       case 3:
        if ((unsigned int)(pos-start) > 10) goto done;
        fn_8034F028(entry->d, body+start, pos-start); break;
       case 4:
        if ((unsigned int)(pos-start) > 10) goto done;
        fn_8034F028(entry->e, body+start, pos-start); break;
       case 5:
        if (pos-start > 8) goto done;
        n = fn_80392AB8(body+start, pos-start);
        if (n < 0) goto done;
        entry->number = n; break;
      }
      field++;
      start = pos+1;
     }
reset:
     cr = 0;
    }
    pos++;
   }
  }
 }
done:
 return result;
}

// TODO: clean up, give variables real names
/* jj_803560A4 */
extern int DWCi_NdSetAttr(const char *arg0, const char *arg1, const char *arg2);

int DWC_NdSetAttr(const char *arg0, const char *arg1, const char *arg2) {
    if (DWCi_IsError() != 0) {
        return 0;
    }
    return DWCi_NdSetAttr(arg0, arg1, arg2);
}

