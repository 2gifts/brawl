#define MV_MOVIE_PLAYER_INTERNAL
#include <mv/mv_THPPlayer.h>
#include <mv/mv_THPVideoDecode.h>
#include <mv/mv_movie_player.h>
#include <revolution/DVD/dvdfs.h>
#include <revolution/OS/OSInterrupt.h>
#include <revolution/OS/OSTime.h>
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
#include <revolution/DVD/dvdfs.h>

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

// TODO: clean up, give variables real names
namespace jj_8007FDF4 {
extern "C" {

extern unsigned char lbl_8040ABC0[0x18];

unsigned char *fn_8007FDF4(void) {
    return lbl_8040ABC0;
}

}
}

namespace jj_8007F3AC {
extern "C" {
void __dt__6gfTaskFv(void*, int);
void __dl__FPv(void*);
}

extern "C" void* fn_8007F3AC(void* p, int n) {
    if (p) {
        __dt__6gfTaskFv(p, 0);
        if (n > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

extern u8 lbl_805A024A;
extern u8 lbl_805A024C[2];
extern s32 lbl_805A0238;
extern void* lbl_805A022C;
extern s32 lbl_805A0250;
extern s32 lbl_805A0254;
extern s32 lbl_805A0258;

u8 mvMoviePlayer::isOpened() {
    return lbl_805A0214 ? lbl_805A024A : 0;
}

void mvMoviePlayer::loadLastFrameInfo() {
    if (lbl_805A0214 != 0) {
        DVDOpen((const char*)lbl_8049DDD0, (DVDFileInfo*)lbl_8049DDF0);
        DVDReadAsyncPrio((DVDFileInfo*)lbl_8049DDF0, lbl_805A022C, 0x20, g_thpObject.unk64, 0, 2);
        lbl_805A024C[0] = 1;
    }
}

bool mvMoviePlayer::isLoadedLastFrameInfo() {
    if (lbl_805A024C[0] != 0 && DVDGetCommandBlockStatus((DVDCommandBlock*)lbl_8049DDF0) == 0) {
        return true;
    }
    return false;
}

void mvMoviePlayer::closeLastFrameInfo() {
    if (lbl_805A024C[0] != 0) {
        DVDClose((DVDFileInfo*)lbl_8049DDF0);
        lbl_805A024C[0] = 0;
    }
}

void mvMoviePlayer::getVideoSize(int* width, int* height) {
    *width = ((int*)lbl_8049DDB0)[0];
    *height = ((int*)lbl_8049DDB0)[1];
}

int mvMoviePlayer::getVideoState() {
    return THPPlayerGetState();
}

int mvMoviePlayer::getTotalFrame() {
    return lbl_805A0238;
}

void mvMoviePlayer::play() {
    if (lbl_805A0214) {
        THPPlayerPlay();
    }
}

void mvMoviePlayer::setScnMdl(nw4r::g3d::ScnMdl* model, int unk) {
    lbl_805A0250 = (s32)model;
    lbl_805A0254 = unk;
    lbl_805A0258 = 0;
}

void mvMoviePlayer::loadLastFrame() {
    if (lbl_805A0214 != 0) {
        u8* work = lbl_8049DDF0;
        int size = ((int*)lbl_805A022C)[1];
        DVDOpen((const char*)lbl_8049DDD0, (DVDFileInfo*)(work + 0x3C));
        DVDReadAsyncPrio((DVDFileInfo*)(work + 0x3C), lbl_805A022C, size, g_thpObject.unk68, 0, 2);
        lbl_805A024C[1] = 1;
    }
}

bool mvMoviePlayer::isLoadedLastFrame() {
    if (lbl_805A024C[1] != 0) {
        if (DVDGetCommandBlockStatus((DVDCommandBlock*)(lbl_8049DDF0 + 0x3C)) == 0) {
            return true;
        }
    }
    return false;
}

void mvMoviePlayer::closeLastFrame() {
    if (lbl_805A024C[1] != 0) {
        DVDClose((DVDFileInfo*)(lbl_8049DDF0 + 0x3C));
        lbl_805A024C[1] = 0;
    }
}

s8 mvMoviePlayer::isPause() const {
    return lbl_805A024E;
}

void mvMoviePlayer::setLastFrame() {
    if (lbl_805A0214 != 0) {
        THPPlayerPause();
        THPVideoDecodeOneShot((u32*)lbl_805A022C);
    }
}

// HYPOTHESIS: the timing record of the player.
struct MovieTiming {
    u32 unk0;
    u32 unk4;
    u32 unk8;
    u32 unkC;
};

extern "C" void fn_8007C61C(u32, u32, u32);

void mvMoviePlayer::prepare(int unk) {
    if (lbl_805A0214 != 0) {
        MovieTiming* timing = (MovieTiming*)lbl_8049DDC0;
        u32 divisor = timing->unkC;
        u32 remainder;
        if (divisor != 1) {
            remainder = OSGetTick() % timing->unkC;
        } else {
            remainder = 0;
        }
        fn_8007C61C(0, unk, remainder);
    }
}
