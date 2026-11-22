#define MV_PLAYER_OPENED_T u8
#define MV_PLAYER_PAUSE_T s8
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

extern "C" unsigned char lbl_8040ABC0[0x18];

// TODO: clean up, give variables real names
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

// TODO: clean up, give variables real names
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

// TODO: clean up, give variables real names
u8 mvMoviePlayer::isOpened() {
    return lbl_805A0214 ? lbl_805A024A : 0;
}

// TODO: clean up, give variables real names
void mvMoviePlayer::loadLastFrameInfo() {
    if (lbl_805A0214 != 0) {
        DVDOpen((const char*)lbl_8049DDD0, (DVDFileInfo*)lbl_8049DDF0);
        DVDReadAsyncPrio((DVDFileInfo*)lbl_8049DDF0, lbl_805A022C, 0x20, g_thpObject.unk64, 0, 2);
        lbl_805A024C[0] = 1;
    }
}

// TODO: clean up, give variables real names
bool mvMoviePlayer::isLoadedLastFrameInfo() {
    if (lbl_805A024C[0] != 0 && DVDGetCommandBlockStatus((DVDCommandBlock*)lbl_8049DDF0) == 0) {
        return true;
    }
    return false;
}

// TODO: clean up, give variables real names
void mvMoviePlayer::closeLastFrameInfo() {
    if (lbl_805A024C[0] != 0) {
        DVDClose((DVDFileInfo*)lbl_8049DDF0);
        lbl_805A024C[0] = 0;
    }
}

// TODO: clean up, give variables real names
void mvMoviePlayer::getVideoSize(int* width, int* height) {
    *width = ((int*)lbl_8049DDB0)[0];
    *height = ((int*)lbl_8049DDB0)[1];
}

int mvMoviePlayer::getVideoState() {
    return THPPlayerGetState();
}

// TODO: clean up, give variables real names
int mvMoviePlayer::getTotalFrame() {
    return lbl_805A0238;
}

// TODO: clean up, give variables real names
void mvMoviePlayer::play() {
    if (lbl_805A0214) {
        THPPlayerPlay();
    }
}

// TODO: clean up, give variables real names
void mvMoviePlayer::setScnMdl(nw4r::g3d::ScnMdl* model, int unk) {
    lbl_805A0250 = (s32)model;
    lbl_805A0254 = unk;
    lbl_805A0258 = 0;
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void mvMoviePlayer::loadLastFrame() {
    if (lbl_805A0214 != 0) {
        u8* work = lbl_8049DDF0;
        int size = ((int*)lbl_805A022C)[1];
        DVDOpen((const char*)lbl_8049DDD0, (DVDFileInfo*)(work + 0x3C));
        DVDReadAsyncPrio((DVDFileInfo*)(work + 0x3C), lbl_805A022C, size, g_thpObject.unk68, 0, 2);
        lbl_805A024C[1] = 1;
    }
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
bool mvMoviePlayer::isLoadedLastFrame() {
    if (lbl_805A024C[1] != 0) {
        if (DVDGetCommandBlockStatus((DVDCommandBlock*)(lbl_8049DDF0 + 0x3C)) == 0) {
            return true;
        }
    }
    return false;
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void mvMoviePlayer::closeLastFrame() {
    if (lbl_805A024C[1] != 0) {
        DVDClose((DVDFileInfo*)(lbl_8049DDF0 + 0x3C));
        lbl_805A024C[1] = 0;
    }
}

// TODO: clean up, give variables real names
s8 mvMoviePlayer::isPause() const {
    return lbl_805A024E;
}

// TODO: clean up, give variables real names
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

// TODO: clean up, give variables real names
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

// TODO: clean up, give variables real names
namespace jj_8007E99C {
extern "C" {

void MIXInit(void);
int AXRegisterCallback(int);
void AXRegisterAuxACallback(int, int);
int GetSizeTHPReadWork__Fv(void);
void* alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(int, unsigned long);
void SetTHPReadWork__FP11THPReadWork(void*);
int fn_8007E3A4(void);
void SetTHPVideoDecWork__FP15THPVideoDecWork(void*);
int GetSizeTHPAudioDecWork__Fv(void);
void SetTHPAudioDecWork__FP15THPAudioDecWork(void*);
int GetSizeTHPPlayerWork__Fv(void);
void SetTHPPlayerWork__FPv(void*);
void Disable__Q34nw4r2ut2LCFv(void);
void THPPlayerInit__Fv(void);
void CallbackTHPAudioFrame__Fv(void);

extern "C" int lbl_805A0210;
extern "C" int lbl_805A025C;
extern "C" int lbl_805A0230;
extern "C" void* lbl_805A021C;
extern "C" void* lbl_805A0220;
extern "C" void* lbl_805A0224;
extern "C" void* lbl_805A0228;
extern "C" unsigned char lbl_805A024C[2];
extern "C" unsigned char lbl_805A024E;
extern "C" unsigned char lbl_805A024A;

int __ct__13mvMoviePlayerFv(int arg0) {
    int tmp = lbl_805A0210;
    lbl_805A025C = arg0;
    lbl_805A0210 = tmp + 1;
    MIXInit();
    lbl_805A0230 = AXRegisterCallback((int)CallbackTHPAudioFrame__Fv);
    AXRegisterAuxACallback(0, 0);
    lbl_805A021C = alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(0x2a, GetSizeTHPReadWork__Fv());
    SetTHPReadWork__FP11THPReadWork(lbl_805A021C);
    lbl_805A0220 = alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(0x2a, fn_8007E3A4());
    SetTHPVideoDecWork__FP15THPVideoDecWork(lbl_805A0220);
    lbl_805A0224 = alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(0x2a, GetSizeTHPAudioDecWork__Fv());
    SetTHPAudioDecWork__FP15THPAudioDecWork(lbl_805A0224);
    lbl_805A0228 = alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(0x2a, GetSizeTHPPlayerWork__Fv());
    SetTHPPlayerWork__FPv(lbl_805A0228);
    Disable__Q34nw4r2ut2LCFv();
    THPPlayerInit__Fv();
    lbl_805A024C[1] = 0;
    lbl_805A024C[0] = 0;
    lbl_805A024E = 0;
    lbl_805A024A = 0;
    return arg0;
}

}
}

// TODO: clean up, give variables real names
namespace jj_8007EA78 {
struct S { char d[0x3c]; };
extern "C" {
void exit__6gfTaskFv(void*);
void fn_8007C9DC();
void THPPlayerClose__Fv();
void FreeTHPReadWork__Fv();
void fn_8007E3C8();
void FreeTHPAudioDecWork__Fv();
void FreeTHPPlayerWork__Fv();
void free__13gfHeapManagerFPv(void*);
void freeMovieBuffer__9movieOpenFv();
void DVDClose(void*);
void Enable__Q34nw4r2ut2LCFv();
void AXRegisterCallback(int);
void __dl__FPv(void*);
extern "C" S lbl_8049DDF0[2];
extern "C" int lbl_805A0210;
extern "C" int lbl_805A0214;
extern "C" void* lbl_805A0218;
extern "C" void* lbl_805A021C;
extern "C" void* lbl_805A0220;
extern "C" void* lbl_805A0224;
extern "C" void* lbl_805A0228;
extern "C" void* lbl_805A022C;
extern "C" int lbl_805A0230;
extern "C" unsigned char lbl_805A024C[2];
extern "C" int lbl_805A0250;
extern "C" int lbl_805A025C;
extern "C" void* lbl_805A0260;

void* __dt__13mvMoviePlayerFv(void* p, int n) {
    if (p != 0) {
        if (lbl_805A0260 != 0) {
            exit__6gfTaskFv(lbl_805A0260);
            lbl_805A0260 = 0;
        }
        if (lbl_805A0214 != 0) {
            fn_8007C9DC();
            THPPlayerClose__Fv();
            lbl_805A0214 = 0;
        }
        FreeTHPReadWork__Fv();
        fn_8007E3C8();
        FreeTHPAudioDecWork__Fv();
        FreeTHPPlayerWork__Fv();
        if (lbl_805A0218 != 0) { free__13gfHeapManagerFPv(lbl_805A0218); lbl_805A0218 = 0; }
        if (lbl_805A021C != 0) { free__13gfHeapManagerFPv(lbl_805A021C); lbl_805A021C = 0; }
        if (lbl_805A0220 != 0) { free__13gfHeapManagerFPv(lbl_805A0220); lbl_805A0220 = 0; }
        if (lbl_805A0224 != 0) { free__13gfHeapManagerFPv(lbl_805A0224); lbl_805A0224 = 0; }
        if (lbl_805A0228 != 0) { free__13gfHeapManagerFPv(lbl_805A0228); lbl_805A0228 = 0; }
        if (lbl_805A022C != 0) { free__13gfHeapManagerFPv(lbl_805A022C); lbl_805A022C = 0; }
        freeMovieBuffer__9movieOpenFv();
        for (int i = 0; i < 2; i++) {
            if (lbl_805A024C[i] != 0) {
                DVDClose(&lbl_8049DDF0[i]);
                lbl_805A024C[i] = 0;
            }
        }
        Enable__Q34nw4r2ut2LCFv();
        lbl_805A0210--;
        AXRegisterCallback(lbl_805A0230);
        lbl_805A0230 = 0;
        lbl_805A0250 = 0;
        lbl_805A025C = 0;
        if (n > 0) __dl__FPv(p);
    }
    return p;
}
}
}

// TODO: clean up, give variables real names
namespace jj_8007EF70 {
extern "C" int THPPlayerGetState__Fv(void);
extern "C" void fn_8007C9DC(void);
extern "C" int THPPlayerClose__Fv(void);
extern "C" void THPPlayerPlay__Fv(void);
extern "C" int THPPlayerPause__Fv(void);
extern "C" int THPPlayerDrawCurrentFrame__Fiiiiii(int, int, int, int, int, int);
extern "C" void THPPlayerDrawDone__Fv(void);
extern "C" void fn_8007FE00(int, int, int, int, int, int, int);
extern "C" char g_thpObject[];
extern "C" int lbl_8049DDB0[4];
extern "C" int lbl_8059DBA0;
extern "C" int lbl_8059DBA4;
extern "C" int lbl_805A0214;
extern "C" int lbl_805A0238;
extern "C" int lbl_805A023C;
extern "C" unsigned int lbl_805A0240;
extern "C" unsigned char lbl_805A024E;
extern "C" int lbl_805A0250;
extern "C" int lbl_805A0254;

extern "C" int drawFrame__13mvMoviePlayerFiiii(int a1, int a2, int a3, int a4, int a5) {
    int ret = -1;
    int st;
    int t;
    unsigned int sum;
    unsigned int* e8;

    if (lbl_805A0214 != 0) {
        st = THPPlayerGetState__Fv();
        if (st == 5) {
            fn_8007C9DC();
            THPPlayerClose__Fv();
            lbl_805A0214 = 0;
            return lbl_805A0238 - 1;
        }
        if (st == 3) {
            return lbl_805A0238 - 1;
        }
        if (lbl_805A024E != 0) {
            if (st == 2) {
                THPPlayerPause__Fv();
            }
        } else if (st == 4) {
            THPPlayerPlay__Fv();
        }
        if (a4 < 0) {
            a4 = lbl_8049DDB0[0];
        }
        if (a5 < 0) {
            a5 = lbl_8049DDB0[1];
        }
        if (lbl_805A0250 == 0) {
            ret = THPPlayerDrawCurrentFrame__Fiiiiii(lbl_8059DBA0, lbl_8059DBA4, a2, a3, a4, a5);
            THPPlayerDrawDone__Fv();
        } else if (*(int*)(g_thpObject + 0xA0) != 0 &&
                   *(unsigned char*)(g_thpObject + 0xA4) != 0 &&
                   *(void**)(g_thpObject + 0xE8) != 0) {
            e8 = *(unsigned int**)(g_thpObject + 0xE8);
            fn_8007FE00(lbl_805A0250, lbl_805A0254, e8[0], e8[1], e8[2], a4, a5);
            e8 = *(unsigned int**)(g_thpObject + 0xE8);
            sum = e8[3] + *(unsigned int*)(g_thpObject + 0xC0);
            ret = sum % *(unsigned int*)(g_thpObject + 0x50);
            THPPlayerDrawDone__Fv();
            if (lbl_805A023C != ret) {
                lbl_805A023C = ret;
                lbl_805A0240 = 0;
            } else if (lbl_805A024E == 0) {
                t = lbl_805A0238 - 30;
                lbl_805A0240++;
                if (t < 0) {
                    t = 0;
                }
                if (lbl_805A023C >= t &&
                    (unsigned int)((lbl_805A0238 - lbl_805A023C) * 6) < lbl_805A0240) {
                    ret = lbl_805A0238 - 1;
                }
            }
        }
    }
    return ret;
}
}

// TODO: clean up - lbl_805A025C is the instance pointer (stored by the constructor)
extern "C" int lbl_805A025C;

mvMoviePlayer* mvMoviePlayer::getInstance() {
    return reinterpret_cast<mvMoviePlayer*>(lbl_805A025C);
}
