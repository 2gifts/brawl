#include <types.h>
#include <mv/mv_movie_player.h>

extern "C" void fn_8007C168();
extern "C" void fn_8007C9DC();
extern "C" void fn_8007CB08();
extern "C" s32 fn_8007D1AC();
extern s32 lbl_805A0214;
extern s8 lbl_805A024E;

void mvMoviePlayer::setPause() {
    s32 result;

    if (lbl_805A0214 != 0) {
        result = fn_8007D1AC();
        if (result == 2) {
            fn_8007CB08();
        }
        if (result == 5) {
            fn_8007C9DC();
            fn_8007C168();
            lbl_805A0214 = 0;
        }
    }
    lbl_805A024E = 1;
}

#include <mv/mv_movie_player.h>

extern "C" void fn_8007C944();
extern "C" s32 fn_8007D1AC();
extern s32 lbl_805A0214;
extern s8 lbl_805A024E;

void mvMoviePlayer::resetPause() {
    if (lbl_805A0214 != 0) {
        if (fn_8007D1AC() == 4) {
            fn_8007C944();
        }
    }
    lbl_805A024E = 0;
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_8049DDB0[0x10] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_8049DDC0[0x10] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_8049DDD0[0x20] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_8049DDF0[0xd8] __attribute__((aligned(8))) = { 0 };
