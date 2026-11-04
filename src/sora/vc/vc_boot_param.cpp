#include <gf/gf_application.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_global.h>
#include <revolution/OS/OSError.h>
#include <revolution/SC/scapi.h>
#include <revolution/WPAD/WPAD.h>
#include <string.h>
#include <types.h>

extern const char* lbl_8059C770[2];
extern u32 lbl_8059C778;
extern u32 lbl_8059C77C;
extern char lbl_80423958[0x28];

// OSLaunchPartitionl (the map name); src/RVL_SDK/os/OSExec.c calls it by this stand-in name.
extern "C" void fn_801DB2B8(u64 tid, u32 argValue, char* arg0, ...);

namespace {
// HYPOTHESIS: the launch payload is a 0x4C-byte serialized array of words.
void VC_setup(u32* params, u8 key, u8 language, int duration, bool widescreen)
{
    memset(params, 0, 0x4C);
    u8* bytes = (u8*)params;
    bytes[0] = key;
    memcpy(bytes + 1, lbl_8059C770[0], 4);
    bytes[6] = language;
    params[2] = duration;
    bytes[5] = widescreen ? 2 : 1;
    u32* controller = params;
    for (int channel = 0; channel < 4; channel++, controller += 2) {
        u32 type;
        controller[3] = WPADProbe(channel, &type);
        controller[4] = type;
    }
    for (int channel = 0; channel < 4; channel++, params += 2) {
        gfPadStatus status;
        g_gfPadSystem->getSysPadStatus(channel, &status);
        params[11] = status.m_error;
        params[12] = 0;
    }
}
}

void VC_launch(const char* title, int duration)
{
    u8 widescreen = g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen;
    union {
        u32 words[19];
        u8 bytes[0x4C];
    } params;
    VC_setup(params.words, 0x10, SCGetLanguage(), duration, widescreen);
    char encoded[0x4D];
    const u8* input = params.bytes;
    char key = *input;
    encoded[0] = key;
    char* output = encoded + 1;
    input++;
    int index;
    for (index = 1; index < sizeof(params); index++) {
        *output++ = key + *input++;
    }
    encoded[index] = 0;
    int titleId = (title[3] | (title[2] << 8)) | ((title[0] << 24) | (title[1] << 16));
    u64 partitionId = ((u64)lbl_8059C778 << 32) | lbl_8059C77C;
    partitionId |= titleId;
    lbl_8059C77C = partitionId;
    lbl_8059C778 = partitionId >> 32;
    OSReport(lbl_80423958, partitionId, title);
    fn_801DB2B8(((u64)lbl_8059C778 << 32) | lbl_8059C77C, 0, encoded, 0);
}
