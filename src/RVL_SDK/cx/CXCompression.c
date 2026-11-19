#include <types.h>

// TODO: clean up, give variables real names
/* jj_80205474 */
typedef unsigned short u16;
typedef unsigned char u8;
typedef short s16;
typedef struct {
    u16 a;
    u16 b;
    s16* c;
    s16* d;
    s16* e;
} Q;

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void fn_80205474(Q* p, u8* q) {
    u16 b;
    u16 a;
    u8 x;
    u16 v;
    s16* c;
    s16* t;
    s16* e;
    int i;
    s16 r;
    b = p->b;
    x = q[0];
    c = p->d;
    t = p->c;
    e = p->e;
    a = p->a;
    if (b == 0x1000) {
        i = q[-0x1000] * 2;
        r = t[*(s16*)((char*)c + i)];
        *(s16*)((char*)c + i) = r;
        if (r == -1) *(s16*)((char*)e + i) = -1;
        v = a;
    } else {
        v = b;
    }
    i = x * 2;
    r = *(s16*)((char*)e + i);
    if (r == -1) *(s16*)((char*)c + i) = v;
    else t[r] = v;
    *(s16*)((char*)e + i) = v;
    t[v] = -1;
    if (b == 0x1000) {
        p->a = (a + 1) % 0x1000;
    } else {
        p->b = p->b + 1;
    }
}

