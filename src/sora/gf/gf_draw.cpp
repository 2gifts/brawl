#include <types.h>

// TODO: clean up, give variables real names
namespace jj_8001AA88 {
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

extern "C" void fn_80192B00(const void *, u32 *, u16 *, u16 *, u32 *, float *, float *, u8 *);
extern "C" void fn_80192A44(const void *, u32 *, u16 *, u16 *, u32 *, float *, float *, u8 *);
extern "C" void fn_801F2AFC(void *, u32, u16, u16, u32, u32, u32, u8, u32);
extern "C" void fn_801F28F0(void *, u32, u16, u16, u32, u32, u32, u8);
extern "C" void fn_801F2B44(void *, s32, s32, s32, s32, s32, float, float, float);
extern "C" float lbl_805A1460;

struct Resource {
    u8 pad00[0x10];
    s32 relative;
    u8 pad14[4];
    u32 value;
    u16 size;
};

struct ParseLocals {
    u8 special;
    u8 pad11;
    u16 height;
    u16 width;
    u16 pad16;
    float y;
    float x;
    u32 formatB;
    u32 formatA;
    u32 file;
};

extern "C" void fn_8001AA88(void *arg0, const Resource **source) {
    const Resource *resource = *source;
    ParseLocals locals;

    if ((resource->value & 1) != 0) {
        fn_80192B00(
            source,
            &locals.file,
            &locals.width,
            &locals.height,
            &locals.formatA,
            &locals.x,
            &locals.y,
            &locals.special
        );
        s32 unused = 0;
        fn_801F2AFC(
            arg0,
            locals.file,
            locals.width,
            locals.height,
            locals.formatA,
            1,
            1,
            locals.special,
            unused
        );
    } else {
        fn_80192A44(
            source,
            &locals.file,
            &locals.width,
            &locals.height,
            &locals.formatB,
            &locals.x,
            &locals.y,
            &locals.special
        );
        fn_801F28F0(
            arg0,
            locals.file,
            locals.width,
            locals.height,
            locals.formatB,
            1,
            1,
            locals.special
        );
    }
    if (locals.special) {
        fn_801F2B44(
            arg0, 3, 1, 0, 0, 0,
            locals.x, locals.y, lbl_805A1460
        );
    } else {
        fn_801F2B44(
            arg0, 1, 1, 0, 0, 0,
            locals.x, locals.y, lbl_805A1460
        );
    }
}
}

