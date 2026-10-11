// boot time drive check. this sat in front of dvd_broadway in the dol but has its own statics (messages,
// flag, scratch buffer), so it is split out as its own unit; it only talks to DVDLow* through the headers
#include "private/iostypes.h"
#include "revolution/dvd.h"
#include "revolution/os.h"
#include "revolution/sc.h"

extern void __DVDShowFatalMessage(void);
extern u16 OSSetFontEncode(u16);

// error screens for the drive check below, indexed by SC language
static char sMsgJa[] = "\n\n\n\203G\203\211\201[\203R\201[\203h\202O\202O\202P\201B\n\225s\226\276\202\310\203f\203o\203C\203X\202\252\214\251\202\302\202\251\202\350\202\334\202\265\202\275\201B";
static char sMsgEn[] = "\n\n\nError #001,\nunauthorized device has been detected.";
static char sMsgDe[] = "\n\n\nFehler #001:\nEs wurde eine unzul\344ssige Komponente\nentdeckt.";
static char sMsgFr[] = "\n\n\nErreur 001:\nun dispositif non autoris\351 a \351t\351 d\351tect\351.";
static char sMsgEs[] = "\n\n\nError 001:\nSe ha detectado un dispositivo no\nautorizado.";
static char sMsgIt[] = "\n\n\nErrore #001:\nrilevato un dispositivo non autorizzato.";
static char sMsgNl[] = "\n\n\nFout #001:\nongeoorloofd onderdeel gevonden.";

static const char* const sDriveMsgs[7] = {sMsgJa, sMsgEn, sMsgDe, sMsgFr, sMsgEs, sMsgIt, sMsgNl};

static u8 sProbeBuf[32] __attribute__((aligned(32)));
static volatile int sProbeDone = 1;
static volatile s32 sProbeStatus;

static void driveProbeCallback(u32 intType) {
    sProbeStatus = intType;
    sProbeDone = 1;
}

static inline void showDriveError(void) {
    static GXColor fgInit = {0xFF, 0xFF, 0xFF, 0x00};
    GXColor fg = fgInit;
    GXColor bg = {0, 0, 0, 0};
    const char* msg;
    const char* const* tbl;

    if (SCGetLanguage() == 0) {
        OSSetFontEncode(1);
    } else {
        OSSetFontEncode(0);
    }
    tbl = sDriveMsgs;
    msg = (SCGetLanguage() > 6) ? tbl[1] : tbl[SCGetLanguage()];
    OSFatal(fg, bg, msg);
}

// HYPOTHESIS: boot time check that the drive is a real nintendo one. it pokes two reads at addresses a
// genuine drive should reject and puts up a fatal error if they work. called once from OSInit when
// the ios is new enough. no name evidence, so it keeps its address name
BOOL fn_801FC4EC(void) {
    OSIOSRev rev;
    u32 offset = 0x460A0000;

    if (OSGetPhysicalMem2Size() == 0x8000000) {
        return TRUE;
    }

    __OSGetIOSRev(&rev);
    if (rev.major < 30 || rev.major >= 254) {
        return TRUE;
    }

    if (*(volatile u8*)0x8000319C == 0x81) {
        offset = 0x7ED40000;
    }

    sProbeDone = 0;
    DVDLowUnencryptedRead(sProbeBuf, 32, offset, driveProbeCallback);
    while (sProbeDone == 0) {
    }

    switch (sProbeStatus) {
    case 2:
        sProbeDone = 0;
        DVDLowRequestError(driveProbeCallback);
        while (sProbeDone == 0) {
        }
        DVDLowGetImmBufferReg();
        switch (sProbeStatus) {
        case 1:
            if ((s32)DVDLowGetImmBufferReg() & 0xFF000000) {
                goto retfalse;
            }
            switch ((s32)DVDLowGetImmBufferReg() & 0xFFFFFF) {
            case 0x52100:
                sProbeDone = 0;
                DVDLowReportKey((DVDVideoReportKey*)sProbeBuf, 0x40000, 0, driveProbeCallback);
                while (sProbeDone == 0) {
                }
                switch (sProbeStatus) {
                case 2:
                    sProbeDone = 0;
                    DVDLowRequestError(driveProbeCallback);
                    while (sProbeDone == 0) {
                    }
                    DVDLowGetImmBufferReg();
                    switch (sProbeStatus) {
                    case 1:
                        if ((s32)DVDLowGetImmBufferReg() & 0xFF000000) {
                            goto retfalse;
                        }
                        switch ((s32)DVDLowGetImmBufferReg() & 0xFFFFFF) {
                        case 0x53100:
                        case 0x52000:
                            return TRUE;
                        default:
                            goto fatal;
                        }
                    default:
                        goto fail;
                    }
                case 1:
                    goto fatal;
                default:
                    goto fail;
                }
            default:
                goto fatal;
            }
        default:
            goto fail;
        }
    case 1:
        goto fatal;
    default:
        goto fail;
    }

fatal:
    showDriveError();
    return FALSE;

retfalse:
    return FALSE;

fail:
    __DVDShowFatalMessage();
    return FALSE;
}
