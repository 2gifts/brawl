#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_804F9FC0[0x14a0] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_804FB460[0x40] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0CA0[0x4] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_805A0CA4[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0CA8[0x8] __attribute__((aligned(4))) = { 0 };
typedef struct { u32 x, y; } Pair;
Pair lbl_805A0CB0 = { 0, 0 };
unsigned char lbl_805A0CB8[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0CBC[0x4] __attribute__((aligned(4))) = { 0 };

// TODO: clean up, give variables real names
extern int lbl_8059ED5C;
extern int lbl_8059ED60;

void fn_8022555C(char a, char b) {
    lbl_8059ED5C = a;
    lbl_8059ED60 = b;
}

extern unsigned char lbl_804F9FC0[];

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void fn_80225570(int idx, float a, float b) {
    unsigned char* p = lbl_804F9FC0 + idx * 0x528;
    *(float*)(p + 0x9c) = a;
    *(float*)(p + 0xa0) = b;
}

// TODO: clean up, give variables real names
void fn_80228138(int arg0) {
    int offset = arg0 * 0x528;
    ((unsigned char *)lbl_804F9FC0 + offset)[0x520] = 0;
}

// TODO: clean up, give variables real names
void fn_80228154(int arg0) {
    int offset = arg0 * 0x528;
    ((unsigned char *)lbl_804F9FC0 + offset)[0x520] = 1;
}

// TODO: clean up, give variables real names
/* jj_80228448 */
extern unsigned char lbl_804F9FC0[];

void KPADiControlDpdCallback(int arg0, int arg1) {
    unsigned char *p;
    void (*fn)(int, int);

    p = lbl_804F9FC0 + arg0 * 0x528;
    if (arg1 == 0) {
        if (*(void (**)(int, int))(p + 0x4D8) != 0 && p[0x523] == 0) {
            p[0x523] = 1;
            fn = *(void (**)(int, int))(p + 0x4D8);
            fn(arg0, 1);
            p[0x522] = 0;
        }
    }
    p[0x51F] = 0;
}

/* jj_80225A88 */
extern double sqrt(float);
extern const float lbl_805A39E8;
extern const float lbl_805A39F8;
extern const float lbl_805A3A04;
extern float lbl_8059ED2C;
typedef struct { char pad0[0x54]; float x, y; char pad1[0x448]; float a, b, c; } Object;
// TODO: clean up, give variables real names
void calc_acc_vertical(Object *p) {
    float nx, oy, ox, t, t2;
    float sum = p->a * p->a + p->b * p->b;
    float x = (float)sqrt(sum);
    float y = -p->c;
    float len = (float)sqrt(sum + y * y);
    if (lbl_805A39E8 == len || len >= lbl_805A3A04) return;
    nx = x / len;
    y /= len;
    if (len > lbl_805A39F8) len = lbl_805A3A04 - len;
    ox = p->x;
    t = len * lbl_8059ED2C;
    oy = p->y;
    len *= t;
    nx -= ox;
    t2 = len;
    t2 *= nx;
    x = ox + t2;
    y = oy + len * (y - oy);
    len = (float)sqrt(x * x + y * y);
    if (lbl_805A39E8 != len) {
        p->x = x / len;
        p->y = y / len;
    }
}

/* jj_80227164 */
extern double sqrt(float);
extern const double lbl_805A3A08;
extern const float lbl_805A39E8;

// TODO: clean up, give variables real names
void clamp_stick_circle(float* out, int a, int b, int c, int d) {
    float fa = (float)a;
    float fb = (float)b;
    float fc = (float)c;
    float fd = (float)d;
    float len = (float)sqrt(fa * fa + fb * fb);
    if (len <= fc) {
        out[1] = lbl_805A39E8;
        out[0] = lbl_805A39E8;
    } else if (len >= fd) {
        out[0] = fa / len;
        out[1] = fb / len;
    } else {
        float t = (len - fc) / (fd - fc) / len;
        out[0] = fa * t;
        out[1] = fb * t;
    }
}

/* jj_802267B0 */
extern f32 lbl_8059ED58;

typedef struct Point {
    f32 x;
    f32 y;
    s8 flag8;
    s8 flag9;
    u8 pad[2];
} Point;

typedef struct Obj {
    u8 pad00[0x5E];
    s8 f5E;
    u8 pad5F[0xB0 - 0x5F];
    f32 fB0;
    f32 fB4;
    u8 padB8[0xC4 - 0xB8];
    Point pts[6];
    u8 pad10C[0x490 - 0x10C];
    f32 f490;
    f32 f494;
    f32 f498;
    u8 pad49C[0x4B8 - 0x49C];
    f32 f4B8;
    f32 f4BC;
} Obj;

// TODO: clean up, give variables real names
s32 select_1obj_continue(Obj* obj) {
    Point* outer;
    Point* inner;
    Point* bestA;
    Point* bestB;
    f32 best;
    f32 dx;
    f32 dy;
    f32 d2;
    f32 vx;
    f32 vy;
    f32 fx;
    f32 fy;
    f32 ty;

    best = lbl_8059ED58 * lbl_8059ED58;
    outer = &obj->pts[4];
    do {
        if (outer->flag8 == 0 && outer->flag9 == 0) {
            inner = &obj->pts[0];
            do {
                if (inner->flag8 == 0) {
                    dx = outer->x - inner->x;
                    dy = outer->y - inner->y;
                    d2 = dx * dx + dy * dy;
                    if (d2 < best) {
                        best = d2;
                        bestA = outer;
                        bestB = inner;
                    }
                }
                inner++;
            } while (inner < &obj->pts[4]);
        }
        outer++;
    } while (outer < &obj->pts[6]);

    if (best == lbl_8059ED58 * lbl_8059ED58) {
        return 0;
    }

    *bestA = *bestB;

    vx = obj->fB0 * obj->f4B8 + obj->fB4 * obj->f4BC;
    vy = obj->fB4 * obj->f4B8 - obj->fB0 * obj->f4BC;
    fx = obj->f490 * vx;
    obj->f494 = vx;
    fy = obj->f490 * vy;
    obj->f498 = vy;

    if (bestA == &obj->pts[4]) {
        obj->pts[5].x = bestA->x + fx;
        ty = bestA->y + fy;
        obj->pts[5].flag8 = 0;
        obj->pts[5].flag9 = -1;
        obj->pts[5].y = ty;
    } else {
        obj->pts[4].x = bestA->x - fx;
        ty = bestA->y - fy;
        obj->pts[4].flag8 = 0;
        obj->pts[4].flag9 = -1;
        obj->pts[4].y = ty;
    }

    if ((s8) obj->f5E < 0) {
        return -1;
    }
    return 1;
}

/* jj_80225760 */
#define Obj Obj_jj80225760

typedef struct Obj {
    u32 w0;
    u32 w4;
    u32 w8;
    u8 pad0C[0x60 - 0x0C];
    u32 w60;
    u32 w64;
    u32 w68;
    u8 pad6C[0x4CC - 0x6C];
    u16 h4cc;
    u16 h4ce;
    u16 h4d0;
    u16 h4d2;
    u16 h4d4;
    u16 h4d6;
} Obj;

void calc_button_repeat(Obj *o, u32 arg1, s32 arg2) {
    if (o->w4 != 0 || o->w8 != 0) {
        o->h4cc = 0;
        o->h4ce = o->h4d0;
        if (o->w4 != 0 && o->h4d2 != 0) {
            o->w0 |= 0x80000000;
        }
    } else if (o->w0 != 0) {
        u16 a;
        u16 r8;
        u32 b;
        a = o->h4cc + arg2;
        o->h4cc = a;
        if (a >= 0x9C40) {
            o->h4cc = a - 0x9C40;
        }
        r8 = o->h4cc;
        if (r8 >= o->h4ce) {
            o->w0 |= 0x80000000;
            b = o->h4ce + o->h4d2;
            o->h4ce = b;
            if (r8 >= 0x4E20) {
                o->h4cc = r8 - 0x4E20;
                o->h4ce = (u16)b - 0x4E20;
            }
        }
    }

    if (arg1 != 2) {
        return;
    }

    if (o->w64 != 0 || o->w68 != 0) {
        o->h4d4 = 0;
        o->h4d6 = o->h4d0;
        if (o->w64 != 0 && o->h4d2 != 0) {
            o->w60 |= 0x80000000;
        }
    } else if (o->w60 != 0) {
        u16 a;
        u16 r6;
        u32 b;
        a = o->h4d4 + arg2;
        o->h4d4 = a;
        if (a >= 0x9C40) {
            o->h4d4 = a - 0x9C40;
        }
        r6 = o->h4d4;
        if (r6 >= o->h4d6) {
            o->w60 |= 0x80000000;
            b = o->h4d6 + o->h4d2;
            o->h4d6 = b;
            if (r6 >= 0x4E20) {
                o->h4d4 = r6 - 0x4E20;
                o->h4d6 = (u16)b - 0x4E20;
            }
        }
    }
}
#undef Obj

/* jj_802265FC */
#define Obj Obj_jj802265FC
#define Point Point_jj802265FC
typedef struct Vec2 {
    f32 x;
    f32 y;
} Vec2;

typedef struct Point {
    f32 x;
    f32 y;
    s8 flag8;
    s8 flag9;
    u8 pad[2];
} Point;

typedef struct Obj {
    u8 pad00[0xB0];
    f32 fB0;
    f32 fB4;
    u8 padB8[0xC4 - 0xB8];
    Point pts[6];
    u8 pad10C[0x4A0 - 0x10C];
    f32 f4A0;
    u8 pad4A4[0x4B8 - 0x4A4];
    f32 f4B8;
    f32 f4BC;
    u8 pad4C0[0x4F4 - 0x4C0];
    f32 f4F4;
    f32 f4F8;
    f32 f4FC;
    f32 f500;
} Obj;

s32 select_1obj_first(Obj* obj) {
    Point* p;
    f32 s[4];
    f32 offX;
    f32 offY;

    p = &obj->pts[0];

    offX = obj->fB0 * obj->f4B8 + obj->fB4 * obj->f4BC;
    offY = obj->fB4 * obj->f4B8 - obj->fB0 * obj->f4BC;
    offX *= obj->f4A0;
    offY *= obj->f4A0;

    do {
        if (p->flag8 == 0) {
            s[2] = p->x - offX;
            s[3] = p->y - offY;
            s[0] = p->x + offX;
            s[1] = p->y + offY;
            if (s[2] <= obj->f4F4 || s[2] >= obj->f4FC || s[3] <= obj->f4F8 || s[3] >= obj->f500) {
                if (s[0] > obj->f4F4 && s[0] < obj->f4FC && s[1] > obj->f4F8 && s[1] < obj->f500) {
                    obj->pts[5] = *p;
                    *(Vec2*)&obj->pts[4].x = *(Vec2*)&s[2];
                    obj->pts[4].flag8 = 0;
                    obj->pts[4].flag9 = -1;
                    return -1;
                }
            } else {
                if (s[0] <= obj->f4F4 || s[0] >= obj->f4FC || s[1] <= obj->f4F8 || s[1] >= obj->f500) {
                    obj->pts[4] = *p;
                    *(Vec2*)&obj->pts[5].x = *(Vec2*)&s[0];
                    obj->pts[5].flag8 = 0;
                    obj->pts[5].flag9 = -1;
                    return -1;
                }
            }
        }
        p++;
    } while (p < &obj->pts[4]);
    return 0;
}
#undef Point
#undef Obj

/* jj_802261EC */
#define Vec2 Vec2_jj802261EC
#pragma fp_contract off
extern float lbl_8059ED40;
extern const float lbl_8059ED48;
extern const float lbl_805A39E8;
extern const float lbl_805A39F8;
typedef struct 
{
  float x;
  float y;
  signed char flag;
  char pad[3];
} Elem;
typedef struct 
{
  float x;
  float y;
} Vec2;
inline static float dot2(const Vec2 *b, const Vec2 *a)
{
  return (a->x * b->x) + (a->y * b->y);
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
int select_2obj_first(void *base)
{
  float scale;
  float best = lbl_8059ED48;
  char *new_var;
  float threshold;
  Elem *new_var4[2];
  char *new_var2;
  Elem *outer = (Elem *) (((char *) base) + 0xC4);
  Elem *inner;
  Elem *best_first;
  Elem *best_second;
  char *new_var3;
  threshold = lbl_805A39E8;
  scale = lbl_805A39F8;
  new_var4[0] = (Elem *) (((char *) base) + 0xE8);
  do
  {
    if (outer->flag == 0)
    {
      new_var = (char *) base;
      inner = outer + 1;
      new_var4[1] = (Elem *) (new_var + 0xE8);
      do
      {
        if (inner->flag == 0)
        {
          float dy;
          float dx = inner->x - outer->x;
          dy = inner->y - outer->y;
          {
            float inverse = scale / ((float) sqrt((dx * dx) + (dy * dy)));
            float speed;
            Vec2 projection;
            dx *= inverse;
            new_var = (char *) base;
            dy *= inverse;
            new_var2 = new_var + 0xB4;
            new_var3 = new_var + 0xB4;
            projection.x = ((*((float *) (((char *) base) + 0xB0))) * dx) + ((*((float *) new_var3)) * dy);
            projection.y = ((*((float *) new_var2)) * dx) - ((*((float *) (new_var + 0xB0))) * dy);
            speed = (*((float *) (((char *) base) + 0x510))) * inverse;
            if ((!(speed <= (*((float *) (new_var + 0x514))))) && (!(speed >= lbl_8059ED40)))
            {
              float value = dot2(&projection, (Vec2 *) (new_var + 0x4B8));
              if (value < threshold)
              {
                float magnitude = -value;
                if (magnitude > best)
                {
                  best = magnitude;
                  best_first = inner;
                  best_second = outer;
                }
              }
              else
                if (value > best)
              {
                best = value;
                best_first = outer;
                best_second = inner;
              }
            }
          }
        }
        inner++;
      }
      while (inner <= new_var4[1]);
    }
    outer++;
  }
  while (outer < new_var4[0]);
  if (best == lbl_8059ED48)
  {
    return 0;
  }
  *((Elem *) (((char *) base) + 0xF4)) = *best_first;
  *((Elem *) (new_var + 0x100)) = *best_second;
  return 2;
}
#undef Vec2

/* jj_8022728C */
#define lbl_805A39E8 (*(float*)&lbl_805A39E8)
#define lbl_805A39F8 (*(float*)&lbl_805A39F8)

// TODO: clean up, give variables real names
void clamp_stick_cross(float *out, int x, int y, int min, int max)
{
    float a, b, len;
    if (x < 0) {
        int v = -x;
        if (v <= min)
            out[0] = lbl_805A39E8;
        else if (v >= max)
            out[0] = lbl_805A39F8;
        else
            out[0] = (float)-(x + min) / (float)(max - min);
        out[0] = -out[0];
    } else if (x <= min)
        out[0] = lbl_805A39E8;
    else if (x >= max)
        out[0] = lbl_805A39F8;
    else
        out[0] = (float)(x - min) / (float)(max - min);
    if (y < 0) {
        int v = -y;
        if (v <= min)
            out[1] = lbl_805A39E8;
        else if (v >= max)
            out[1] = lbl_805A39F8;
        else
            out[1] = (float)-(y + min) / (float)(max - min);
        out[1] = -out[1];
    } else if (y <= min)
        out[1] = lbl_805A39E8;
    else if (y >= max)
        out[1] = lbl_805A39F8;
    else
        out[1] = (float)(y - min) / (float)(max - min);
    a = out[0];
    b = out[1];
    if (a * a + b * b > lbl_805A39F8) {
        len = (float)sqrt(a * a + b * b);
        out[0] = out[0] / len;
        out[1] = out[1] / len;
    }
}
#undef lbl_805A39E8
#undef lbl_805A39F8

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
typedef struct { u32 x, y, z; } Triple;
extern f32 lbl_8059ED30;
extern u16 lbl_8059ED38;
extern f32 lbl_8059ED3C, lbl_8059ED44;
extern const f32 lbl_805A39E8, lbl_805A39F4, lbl_805A39F8, lbl_805A39FC, lbl_805A3A00;
#pragma opt_unroll_loops off
void reset_kpad(void *arg0) {
    u8 *p = arg0;
    f32 temp_f1 = *(f32 *)(p + 0xA4);
    f32 z = lbl_805A39E8;
    f32 q = lbl_805A3A00;
    f32 n = lbl_805A39FC;
    f32 v = lbl_805A39F8;
    f32 m = lbl_805A39F4;
    f32 temp_f0;
    f32 cb;
    u8 *var_r29, *var_r5;
    *(u8 *)(p + 0x51D) = 0;
    *(f32 *)(p + 0x4F4) = m + lbl_8059ED3C;
    *(f32 *)(p + 0x4FC) = v - lbl_8059ED3C;
    *(f32 *)(p + 0x4F8) = n + lbl_8059ED3C;
    *(f32 *)(p + 0x500) = q - lbl_8059ED3C;
    *(f32 *)(p + 0x504) = v / lbl_8059ED44;
    *(f32 *)(p + 0x508) = m / lbl_8059ED44;
    *(f32 *)(p + 0x50C) = lbl_8059ED30 * lbl_8059ED30;
    *(f32 *)(p + 0x514) = *(f32 *)lbl_805A0CBC;
    cb = *(f32 *)lbl_805A0CB8;
    temp_f0 = cb / temp_f1;
    *(f32 *)(p + 0x510) = cb;
    *(s32 *)(p + 8) = 0;
    *(s32 *)(p + 4) = 0;
    *(s32 *)(p + 0) = 0;
    *(u16 *)(p + 0x4CC) = 0;
    *(u16 *)(p + 0x4CE) = *(u16 *)(p + 0x4D0);
    *(u8 *)(p + 0x5E) = 0;
    *(u8 *)(p + 0x4CA) = 0;
    *(Pair *)(p + 0x28) = lbl_805A0CB0;
    *(f32 *)(p + 0x4B8) = v;
    *(f32 *)(p + 0x4BC) = z;
    *(Pair *)(p + 0x20) = *(Pair *)(p + 0x28);
    *(f32 *)(p + 0x30) = z;
    *(f32 *)(p + 0x4B0) = v;
    *(f32 *)(p + 0x34) = v;
    *(f32 *)(p + 0x4B4) = z;
    *(f32 *)(p + 0x38) = z;
    *(Pair *)(p + 0x3C) = lbl_805A0CB0;
    *(f32 *)(p + 0x14) = z;
    *(f32 *)(p + 0xC) = z;
    *(f32 *)(p + 0x10) = m;
    *(f32 *)(p + 0x44) = z;
    *(f32 *)(p + 0x54) = v;
    *(f32 *)(p + 0x58) = z;
    *(f32 *)(p + 0x48) = temp_f1;
    *(f32 *)(p + 0x50) = z;
    *(f32 *)(p + 0x4C) = z;
    *(f32 *)(p + 0x49C) = temp_f1;
    *(f32 *)(p + 0x4A0) = temp_f0;
    *(f32 *)(p + 0x490) = temp_f0;
    *(s32 *)(p + 0x494) = *(s32 *)(p + 0xB0);
    *(s32 *)(p + 0x498) = *(s32 *)(p + 0xB4);
    *(f32 *)(p + 0x18) = v;
    *(f32 *)(p + 0x1C) = z;
    *(Triple *)(p + 0x4A4) = *(Triple *)(p + 0xC);
    *(Pair *)(p + 0x4C0) = *(Pair *)(p + 0x4B8);
    *(u16 *)(p + 0x4C8) = lbl_8059ED38;
    *(u16 *)(p + 0x10C) = 0;
    var_r29 = p + 0xE8;
    do {
        *(s8 *)(var_r29 + 8) = -1;
        var_r29 -= 0xC;
    } while (var_r29 >= p + 0xC4);
    var_r5 = p + 0x100;
    do {
        *(s8 *)(var_r5 + 8) = -1;
        var_r5 -= 0xC;
    } while (var_r5 >= p + 0xF4);
    *(u8 *)(p + 0x10F) = 0;
    *(u8 *)(p + 0x51E) = 1;
}
