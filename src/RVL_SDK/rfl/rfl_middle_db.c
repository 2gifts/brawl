#include <types.h>

u32 RFLGetMiddleDBBufferSize(u16 num) {
    return num << 6;
}

extern u8 lbl_8041AEC0[];
extern void RFLiConvertRaw2Info(void* raw, void* info);

void* fn_803D5B54(u32 index) {
    if (index >= 6) {
        return 0;
    }
    return lbl_8041AEC0 + index * 0x4A;
}

void RFLiGetDefaultData(void* info, int index) {
    RFLiConvertRaw2Info(lbl_8041AEC0 + index * 0x4A, info);
}

// TODO: clean up, give variables real names
/* jj_803D5B90 */
typedef struct {
 unsigned short a:3,b:3,c:4,d:6;
 unsigned short e:7,f:3,g:1,h:5;
 unsigned int i:6,j:3,k:4,l:5,m:4,n:5,o:5;
 unsigned int p:5,q:3,r:4,s:5,t:4,u:5,v:6;
 unsigned short w:4,x:4,y:5,z:3;
 unsigned short aa:5,ab:2,ac:4,ad:5;
 unsigned short ae:2,af:2,ag:3,ah:4,ai:5;
 unsigned short aj:4,ak:3,al:4,am:5;
 unsigned short an:1,ao:4,ap:5,aq:5,ar:1;
} Data;
// TODO: clean up, give variables real names
unsigned int fn_803D5B90(Data *p) {
 if (p->a > 7) return 0;
 if (p->b > 5) return 0;
 if (p->c > 11) return 0;
 if (p->e > 71) return 0;
 if (p->f > 7) return 0;
 if (p->g > 1) return 0;
 if (p->i > 47) return 0;
 if (p->j > 5) return 0;
 if (p->k > 7) return 0;
 if (p->l > 7) return 0;
 if (p->m > 12) return 0;
 if (p->n > 18) return 0;
 if (p->p > 23) return 0;
 if (p->q > 7) return 0;
 if (p->r > 8) return 0;
 if (p->s > 11) return 0;
 if (p->t > 12) return 0;
 if (p->u < 3) return 0;
 if (p->u > 18) return 0;
 if (p->w > 11) return 0;
 if (p->x > 8) return 0;
 if (p->y > 18) return 0;
 if (p->aa > 23) return 0;
 if (p->ab > 2) return 0;
 if (p->ac > 8) return 0;
 if (p->ad > 18) return 0;
 if (p->ae > 3) return 0;
 if (p->af > 3) return 0;
 if (p->ag > 7) return 0;
 if (p->ah > 8) return 0;
 if (p->ai > 16) return 0;
 if (p->aj > 8) return 0;
 if (p->ak > 5) return 0;
 if (p->al > 7) return 0;
 if (p->am > 20) return 0;
 if (p->an > 1) return 0;
 if (p->ao > 8) return 0;
 if (p->ap > 16) return 0;
 return p->aq <= 30;
}

// TODO: clean up, give variables real names
/* jj_803D60FC */

extern s32 RFLiPickupCharInfo(void* p);
extern void fn_803D6008(s32 a, void* p);

// TODO: clean up, give variables real names
s32 RFLGetAdditionalInfo(s32 arg0) {
    u8 sp8[0x50];
    s32 temp_r3;

    temp_r3 = RFLiPickupCharInfo(sp8);
    if (temp_r3 == 0) {
        fn_803D6008(arg0, sp8);
    }
    return temp_r3;
}

/* jj_803D5AC4 */
extern int RFLAvailable(void);
extern int RFLiCalculateCRC(int, int);
extern int RFLiAddMiddleDBUserData(int, int);

int RFLAddMiddleDBStoreData(int arg0, int arg1) {
    if (arg0 == 0) return 0xF;
    if (arg1 == 0) return 0xF;
    if (RFLAvailable() == 0) return 1;
    if (RFLiCalculateCRC(arg1, 0x4C) & 0xFFFF) return 7;
    return RFLiAddMiddleDBUserData(arg0, arg1);
}

/* jj_803D5924 */
extern void RFLiConvertHRaw2Info(void *a, void *b);
extern int RFLiIsValidID(void *p);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
int RFLiGetCharInfoMiddleDB(void *arg0, u8 *arg1, u32 arg2) {
    u8 *temp;

    if (arg1 == 0) {
        return 0;
    }
    if (arg2 >= *(u16 *)(arg1 + 0xC)) {
        return 0;
    }
    if (*(u16 *)(arg1 + 0xE) == 0) {
        return 0;
    }
    temp = *(u8 **)(arg1 + 0x8) + ((arg2 << 6) & 0x3FFFC0);
    if (RFLiIsValidID(temp + 0x18) == 0) {
        return 0;
    }
    RFLiConvertHRaw2Info(temp, arg0);
    return 1;
}

/* jj_803D5878 */
extern void* memset(void*, int, unsigned long);
typedef struct {
    int type;
    int x4;
    void* buf;
    unsigned short count;
    unsigned short xe;
    union {
        struct { unsigned char a, b, c, d; } u8;
        struct { unsigned short h; unsigned char c, d; } u16;
        struct { unsigned pad : 2; unsigned f : 15; unsigned rest : 15; } bf;
        unsigned w;
    } u;
    int x14;
} Obj;

void RFLInitMiddleDB(Obj* o, int type, void* buf, unsigned short count) {
    if (o == 0) return;
    if (buf == 0) return;
    o->type = type;
    o->x4 = 0;
    o->count = count;
    o->buf = buf;
    o->xe = 0;
    o->u.w = 0;
    o->x14 = 0;
    switch (type) {
    case 3:
        if (count > 100) return;
        o->u.u8.a = 2;
        o->u.u8.b = 3;
        o->u.u8.c = 3;
        break;
    case 1:
    case 2:
        if (count > 10000) return;
        o->u.bf.f = -1;
        break;
    case 0:
        o->u.u16.h = 0;
        o->u.u16.c = 2;
        break;
    }
    memset(o->buf, 0, o->count << 6);
}

