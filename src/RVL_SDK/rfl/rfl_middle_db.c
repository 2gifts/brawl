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

