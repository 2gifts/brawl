#include <types.h>
#include <mt/mt_vector.h>

struct ecMgr {
    void endEffect(u32 arg0);
};

extern ecMgr* g_ecMgr;
extern f32 lbl_805A1998;
extern "C" void fn_80065C98(struct ecTrace* p1);

// HYPOTHESIS: an element of the trace's 0x18-byte per-point array (unk0). fn_80065C98 allocates it with new[] and an
// empty constructor; it has no destructor, so delete[] frees the block behind the array header without a destroy call.
struct ecTracePoint {
    u8 _0[0x18];
    ecTracePoint() {}
};

struct ecTrace {
    ecTracePoint* unk0;
    Vec3f* unk4; // HYPOTHESIS: the 0xC-byte elements of the second per-point array (empty constructor, no destructor)
    s16 unk8;
    u8 unkA[0x4];
    u8 unkE;
    u8 unkF;
    u8 unk10[0x1C];
    u32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    s32 unk40;
    u32 unk44;
    s16 unk48;
    s16 unk4A;
    u8 unk4C[0x24];
    s8 unk70;
    u8 unk71[0x7];
    f32 unk78;
    u32 unk7C;
    u32 unk80;

    ecTrace(u32 p1, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6);
    ~ecTrace();
    void end(u32 p1, u32 p2);
};

void ecTrace::end(u32 p1, u32 p2) {
    f32 ratio;

    if (unkF == 0) {
        if (p2 == 0) {
            unkF = 1;
            if (p1 == 0) {
                ratio = lbl_805A1998;
            } else {
                ratio = lbl_805A1998 / (f32) p1;
            }
            unk78 = ratio;
            return;
        }
        unk7C = p2;
        unk80 = p1;
        return;
    }
    if (unkF == 1) {
        if (p1 == 0) {
            ratio = lbl_805A1998;
        } else {
            ratio = lbl_805A1998 / (f32) p1;
        }
        unk78 = ratio;
    }
}

ecTrace::~ecTrace() {
    if (unk0 != NULL) {
        delete[] unk0;
        unk0 = NULL;
    }
    if (unk4 != NULL) {
        delete[] unk4;
        unk4 = NULL;
    }
    g_ecMgr->endEffect(unk2C);
}

ecTrace::ecTrace(u32 p1, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6) {
    unk30 = -1;
    unk34 = -1;
    unk38 = -1;
    unk3C = -1;
    unk40 = 0;
    unk8 = p1 + 1;
    unk70 = (s8) p5;
    fn_80065C98(this);
    unkE |= 1;
    if (p6 != 0) {
        unkE |= 2;
    }
    unk44 = p2;
    unk48 = (s16) p3;
    unk4A = (s16) p4;
}

// TODO: clean up, give variables real names
extern "C" void fn_80065BF8(void) {}

extern "C" void fn_80065E60(void) {}

// TODO: clean up, give variables real names
namespace jj_80067B24 {
struct Vec3 {
    f32 x;
    f32 y;
    f32 z;
};

struct Seg {
    Vec3 a;
    Vec3 b;
};

extern "C" void fn_800685A4(Seg* out, Seg* a, Seg* b, f32* tol);

extern "C" void fn_80067B24(Vec3* arg0, Seg* arg1, Seg* arg2, f32* arg3, s32 arg4) {
    Seg buf[15];

    if (arg4 != 0) {
        s32 idx = -1;
        switch (arg4) {
        case 0:
        case 2:
        case 4:
        case 5:
        case 6:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            break;
        case 1:
            fn_800685A4(&buf[0], arg1, arg2, arg3);
            idx = 0;
            break;
        case 3:
            fn_800685A4(&buf[1], arg1, arg2, arg3);
            fn_800685A4(&buf[0], arg1, &buf[1], 0);
            fn_800685A4(&buf[2], &buf[1], arg2, 0);
            idx = 2;
            break;
        case 7:
            fn_800685A4(&buf[3], arg1, arg2, arg3);
            fn_800685A4(&buf[1], arg1, &buf[3], 0);
            fn_800685A4(&buf[0], arg1, &buf[1], 0);
            fn_800685A4(&buf[2], &buf[1], &buf[3], 0);
            fn_800685A4(&buf[5], &buf[3], arg2, 0);
            fn_800685A4(&buf[4], &buf[3], &buf[5], 0);
            fn_800685A4(&buf[6], &buf[5], arg2, 0);
            idx = 6;
            break;
        case 15:
            fn_800685A4(&buf[7], arg1, arg2, arg3);
            fn_800685A4(&buf[3], arg1, &buf[7], 0);
            fn_800685A4(&buf[11], &buf[7], arg2, 0);
            fn_800685A4(&buf[1], arg1, &buf[3], 0);
            fn_800685A4(&buf[0], arg1, &buf[1], 0);
            fn_800685A4(&buf[2], &buf[1], &buf[3], 0);
            fn_800685A4(&buf[5], &buf[3], &buf[7], 0);
            fn_800685A4(&buf[4], &buf[3], &buf[5], 0);
            fn_800685A4(&buf[6], &buf[5], &buf[7], 0);
            fn_800685A4(&buf[13], &buf[11], arg2, 0);
            fn_800685A4(&buf[14], &buf[13], arg2, 0);
            fn_800685A4(&buf[12], &buf[11], &buf[13], 0);
            fn_800685A4(&buf[9], &buf[7], &buf[11], 0);
            fn_800685A4(&buf[8], &buf[7], &buf[9], 0);
            fn_800685A4(&buf[10], &buf[9], &buf[11], 0);
            idx = 14;
            break;
        }

        {
            Seg* src = &buf[idx];
            Vec3* dst = &arg0[idx];
            for (u32 i = idx; (s32)i >= 0; i--) {
                dst->x = src->a.x;
                dst->y = src->a.y;
                dst->z = src->a.z;
                dst--;
                src--;
            }
        }
    }
}
}

