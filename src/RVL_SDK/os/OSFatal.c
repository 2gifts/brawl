// OSFatal: the blue screen of the sdk. tears the system down far enough to be able to draw, switches
// to a fresh stack, loads the font and prints the message into a yuv framebuffer by hand (no gx).
#include "revolution/os.h"
#include "revolution/os/OSBootInfo.h"
#include "revolution/exi.h"
#include "revolution/gx.h"
#include "revolution/vi.h"
#include "revolution/base/PPCArch.h"
#include <cstring>

#define FATAL_FB_W 640
#define FATAL_FB_H 480
#define FATAL_FB_SIZE (FATAL_FB_W * FATAL_FB_H * 2)

#define FATAL_CLAMP(lo, hi, v) ((v) > (hi) ? (hi) : ((v) < (lo) ? (lo) : (v)))

typedef struct OSFatalParam {
    GXColor textColor;
    GXColor bgColor;
    const char *msg;
} OSFatalParam;

static OSContext FatalContext;
static OSFatalParam FatalParam;

extern void OSDefaultExceptionHandler(__OSException, OSContext *);
extern void __OSStopAudioSystem(void);
extern BOOL __OSCallShutdownFunctions(BOOL pass, u32 event);

static void Halt(void);

static void ScreenClear(u8 *fb, u16 width, u16 height, GXColor yuv) {
    int x, y;

    for (y = 0; y < height; y += 1) {
        for (x = 0; x < width; x += 2) {
            fb[0] = yuv.r;
            fb[1] = yuv.g;

            fb[2] = yuv.r;
            fb[3] = yuv.b;

            fb += sizeof(u32);
        }
    }
}

static void ScreenReport(void *fb, u16 width, u16 height, GXColor msgYUV, s32 x, s32 y, u32 lf, const char *msg) {
    u32 texel[70];
    s32 charWidth;
    u8 *tmp;
    u8 bright;
    s32 xPos;
    u32 idx;
    u32 idx2;
    u32 idx3;
    u32 i;
    u32 j;

new_line:
    if (height - 24 < y) {
        return;
    }

    tmp = (u8 *)fb + ((x + (y * width)) * 2);
    xPos = x;

    while (*msg != '\0') {
        if (*msg == '\n') {
            msg++;
            y += lf;
            goto new_line;
        } else if (width - 48 < xPos) {
            y += lf;
            goto new_line;
        }

        for (i = 0; i < 24; i++) {
            idx = (i & 7) + ((i / 8) * 24);
            texel[idx] = 0;
            texel[idx + 8] = 0;
            texel[idx + 16] = 0;
        }

        msg = OSGetFontTexel(msg, texel, 0, 6, &charWidth);

        for (i = 0; i < 24; i++) {
            idx = (i & 7) + ((i / 8) * 24);

            for (j = 0; j < 24; j++) {
                idx2 = idx + ((j / 8) * 8);
                bright = (texel[idx2] >> ((7 - (j & 7)) * 4)) & 0x0F;

                if (bright != 0) {
                    // luma
                    bright = (((msgYUV.r * (bright * 0xEF)) / 255) / 15) + 16;
                    idx3 = (j + (width * i));
                    tmp[idx3 * 2] = bright;

                    // chroma goes first or second depending on which pixel of the pair this is
                    if ((xPos + j) % 2 != 0) {
                        tmp[(idx3 * 2) - 1] = msgYUV.g;
                        tmp[(idx3 * 2) + 1] = msgYUV.b;
                    } else {
                        tmp[(idx3 * 2) - 1] = msgYUV.b;
                        tmp[(idx3 * 2) + 1] = msgYUV.g;
                    }
                }
            }
        }

        tmp += charWidth * 2;
        xPos += charWidth;
    }
}

static void ConfigureVideo(u16 width, u16 height) {
    GXRenderModeObj rmode;

    rmode.fbWidth = width;
    rmode.efbHeight = FATAL_FB_H;
    rmode.xfbHeight = height;

    rmode.viXOrigin = 40;
    rmode.viWidth = FATAL_FB_W;
    rmode.viHeight = height;

    switch (VIGetTvFormat()) {
    case VI_NTSC:
    case VI_MPAL:
        if (*(volatile u16 *)0xCC00206C & 1) {
            rmode.viTVmode = VI_TVMODE_NTSC_PROG;
            rmode.viYOrigin = 0;
            rmode.xFBmode = VI_XFBMODE_SF;
        } else {
            rmode.viTVmode = VI_TVMODE_NTSC_INT;
            rmode.viYOrigin = 0;
            rmode.xFBmode = VI_XFBMODE_DF;
        }
        break;
    case VI_EURGB60:
        rmode.viTVmode = VI_TVMODE_EURGB60_INT;
        rmode.viYOrigin = 0;
        rmode.xFBmode = VI_XFBMODE_DF;
        break;
    case VI_PAL:
        rmode.viTVmode = VI_TVMODE_PAL_INT;
        rmode.viYOrigin = 47;
        rmode.xFBmode = VI_XFBMODE_DF;
        break;
    }

    VIConfigure(&rmode);
    VIConfigurePan(0, 0, FATAL_FB_W, FATAL_FB_H);
}

static GXColor RGB2YUV(GXColor rgb) {
    GXColor yuv;

    f32 y = (0.257f * rgb.r + 0.504f * rgb.g + 0.098f * rgb.b + 16.0f) + 0.5f;
    f32 cb = (-0.148f * rgb.r - 0.291f * rgb.g + 0.439f * rgb.b + 128.0f) + 0.5f;
    f32 cr = (0.439f * rgb.r - 0.368f * rgb.g - 0.071f * rgb.b + 128.0f) + 0.5f;

    yuv.r = FATAL_CLAMP(16.0f, 235.0f, y);
    yuv.g = FATAL_CLAMP(16.0f, 240.0f, cb);
    yuv.b = FATAL_CLAMP(16.0f, 240.0f, cr);
    yuv.a = 0;

    return yuv;
}

void OSFatal(GXColor textColor, GXColor bgColor, const char *msg) {
    s32 retraceCount;
    s64 start;
    OSBootInfo *bootInfo;

    bootInfo = (OSBootInfo *)0x80000000;

    OSDisableInterrupts();

    OSDisableScheduler();
    OSClearContext(&FatalContext);
    OSSetCurrentContext(&FatalContext);
    __OSStopAudioSystem();

    VIInit();
    __OSUnmaskInterrupts(OS_INTERRUPTMASK_PI_VI);
    VISetBlack(TRUE);
    VIFlush();
    VISetPreRetraceCallback(NULL);
    VISetPostRetraceCallback(NULL);

    OSEnableInterrupts();

    retraceCount = VIGetRetraceCount();
    while ((s32)(VIGetRetraceCount() - retraceCount) < 1) {
        ;
    }

    start = OSGetTime();
    do {
        if (__OSCallShutdownFunctions(0, 0)) {
            break;
        }
    } while (OSGetTime() - start < OSMillisecondsToTicks(1000));

    OSDisableInterrupts();
    __OSCallShutdownFunctions(1, 0);

    EXISetExiCallback(0, NULL);
    EXISetExiCallback(2, NULL);
    while (!EXILock(0, 1, NULL)) {
        EXISync(0);
        EXIDeselect(0);
        EXIUnlock(0);
    }
    EXIUnlock(0);

    while ((*(volatile u32 *)0xCD00680C & 1) == 1) {
        ;
    }

    __OSSetExceptionHandler(8, OSDefaultExceptionHandler);

    GXAbortFrame();

    OSSetArenaLo((void *)0x81400000);
    OSSetArenaHi(bootInfo->FSTLocation);

    FatalParam.textColor = textColor;
    FatalParam.bgColor = bgColor;
    FatalParam.msg = msg;
    OSSwitchFiber((u32)Halt, (u32)OSGetArenaHi());
}

static void Halt(void) {
    OSFontHeader *msgFont;
    const char *msg;
    s32 retraceCount;
    size_t msgLen;
    void *msgBuf;
    void *fb;
    OSFatalParam *params;

    OSEnableInterrupts();

    params = &FatalParam;
    msg = params->msg;

    msgLen = strlen(msg) + 1;
    msgBuf = OSAllocFromMEM1ArenaLo(msgLen, 32);
    params->msg = (const char *)memmove(msgBuf, msg, msgLen);

    msgFont = (OSFontHeader *)OSAllocFromMEM1ArenaLo(0xA1004, 32);
    OSLoadFont(msgFont, OSGetArenaLo());

    fb = OSAllocFromMEM1ArenaLo(FATAL_FB_SIZE, 32);
    ScreenClear((u8 *)fb, FATAL_FB_W, FATAL_FB_H, RGB2YUV(params->bgColor));
    VISetNextFrameBuffer(fb);
    ConfigureVideo(FATAL_FB_W, FATAL_FB_H);
    VIFlush();

    retraceCount = VIGetRetraceCount();
    while ((s32)(VIGetRetraceCount() - retraceCount) < 2) {
        ;
    }

    ScreenReport(fb, FATAL_FB_W, FATAL_FB_H, RGB2YUV(params->textColor), 48, 100, msgFont->leading, params->msg);
    DCFlushRange(fb, FATAL_FB_SIZE);
    VISetBlack(FALSE);
    VIFlush();

    retraceCount = VIGetRetraceCount();
    while ((s32)(VIGetRetraceCount() - retraceCount) < 1) {
        ;
    }

    OSDisableInterrupts();
    OSReport("%s\n", params->msg);
    PPCHalt();
}
