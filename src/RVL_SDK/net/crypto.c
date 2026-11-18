#include <types.h>

// TODO: clean up, give variables real names
void fn_8038FD6C(void) {}

extern void fn_8038E744(void);

void fn_80390010(void) {
    fn_8038E744();
}

// TODO: clean up, give variables real names
extern int lbl_805A49C8;
extern void fn_8038FBB4(int, int, int, int, int*);

void fn_8038FD64(int a, int b, int c, int d) {
    fn_8038FBB4(a, b, c, d, &lbl_805A49C8);
}

// TODO: clean up, give variables real names
/* jj_8038F4C0 */
extern int lbl_8059FC38;
extern void *fn_80212588(int);
extern char lbl_80491398[];
extern char lbl_8059FC48;
void OSReport(const char *msg, ...);

void fn_8038F4C0(void) {
    char *base = lbl_80491398;
    if (lbl_8059FC38 >= 0) {
        void *r = fn_80212588(lbl_8059FC38);
        if (r == 0) {
            lbl_8059FC38 = -1;
            return;
        }
        OSReport(base + 0x10, base + 0x28, 0xA4, base + 0x58);
        OSReport(base + 0x34, r);
        OSReport(&lbl_8059FC48);
    }
}

/* jj_8038FF7C */
typedef struct {
    unsigned int x0;
    unsigned int x1;
    unsigned int x2;
    unsigned int x3;
} Blk;

extern void fn_8038F7C8(void *, void *, void *);

void fn_8038FF7C(Blk *a, Blk *b, const Blk *c)
{
    unsigned int t0, t1, t2, t3;

    t0 = a->x0;
    a->x0 = c->x0;
    t1 = a->x1;
    a->x1 = c->x1;
    t2 = a->x2;
    a->x2 = c->x2;
    t3 = a->x3;
    a->x3 = c->x3;
    fn_8038F7C8(a, b, (void *)c);
    b->x0 ^= t0;
    b->x1 ^= t1;
    b->x2 ^= t2;
    b->x3 ^= t3;
}

/* jj_8038FD70 */
extern void OSReport(const char* fmt, ...);
extern char lbl_804914A8[];
extern char lbl_804914D0[];

typedef void (*FuncType)(void*, void*, void*);

int fn_8038FD70(void* arg0, void* arg1, void* arg2, int arg3) {
    int result;
    FuncType func;
    FuncType* func_ptr;

    result = 0;
    func_ptr = *(FuncType**)((char*)arg0 + 0x104);
    func = *func_ptr;

    if (func == 0) {
        OSReport(lbl_804914A8);
    } else if (arg3 & 0xF) {
        OSReport(lbl_804914D0);
    } else {
        while (arg3 != 0) {
            func(arg0, arg1, arg2);
            arg2 = (void*)((char*)arg2 + 0x10);
            arg1 = (void*)((char*)arg1 + 0x10);
            arg3 -= 0x10;
        }
        result = 1;
    }
    return result;
}

/* jj_8038FE28 */
extern void OSReport(const char* fmt, ...);
extern char lbl_804914D0[];
extern char lbl_80491500[];

typedef struct Vt { int a; int (*f)(void*, int, int); } Vt;
typedef struct Obj { char pad[0x104]; Vt* vt; } Obj;

int fn_8038FE28(Obj* o, int a, int b, int n) {
    int r;
    int (*f)(void*, int, int) = o->vt->f;
    r = 0;
    if (f == 0) {
        OSReport(lbl_80491500);
    } else if (n & 0xF) {
        OSReport(lbl_804914D0);
    } else {
        while (n != 0) {
            f(o, a, b);
            b += 0x10;
            a += 0x10;
            n -= 0x10;
        }
        r = 1;
    }
    return r;
}

