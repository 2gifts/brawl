#include <mv/mv_THPPlayer.h>
#include <mv/mv_movie_player.h>
#include <revolution/OS/OSInterrupt.h>
#include <types.h>

extern "C" void fn_8007C9DC();
extern s32 lbl_805A0214;
extern s8 lbl_805A024E;

void mvMoviePlayer::setPause() {
    s32 result;

    if (lbl_805A0214 != 0) {
        result = THPPlayerGetState();
        if (result == 2) {
            THPPlayerPause();
        }
        if (result == 5) {
            fn_8007C9DC();
            THPPlayerClose();
            lbl_805A0214 = 0;
        }
    }
    lbl_805A024E = 1;
}

#include <mv/mv_movie_player.h>

extern s32 lbl_805A0214;
extern s8 lbl_805A024E;

void mvMoviePlayer::resetPause() {
    if (lbl_805A0214 != 0) {
        if (THPPlayerGetState() == 4) {
            THPPlayerPlay();
        }
    }
    lbl_805A024E = 0;
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_8049DDB0[0x10] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_8049DDC0[0x10] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_8049DDD0[0x20] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_8049DDF0[0xd8] __attribute__((aligned(8))) = { 0 };

extern "C" void MIXUpdateSettings();
extern void (*lbl_805A0230)();

void CallbackTHPAudioFrame() {
    if (lbl_805A0230) lbl_805A0230();
    BOOL interrupts = OSEnableInterrupts();
    MIXUpdateSettings();
    THPPlayerStreamUpdate();
    OSRestoreInterrupts(interrupts);
}
