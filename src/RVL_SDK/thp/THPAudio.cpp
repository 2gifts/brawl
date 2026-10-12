#include "revolution.h"
#include "revolution/thp.h"

// the game declares this as a plain c++ function (void*, void*, u32), so the symbol is mangled.
// that is why this unit is a .cpp even though the rest of the sdk is c.

// pulls the next adpcm nibble out of the stream. the returned value is already scaled up
// (nibble << scale, then << 11) so the caller can feed it straight into the predictor.
static inline s32 GetNewSample(THPAudioDecodeInfo *info) {
    s32 nibble;
    u8 byte;

    // every 16 nibbles there is a new header byte (predictor index + scale)
    if ((info->offsetNibbles % 16) == 0) {
        byte = *info->encodeData++;
        info->offsetNibbles += 2;
        info->predictor = (byte >> 4) & 0x7;
        info->scale = byte & 0xF;
    }

    if (info->offsetNibbles & 1) {
        byte = *info->encodeData++;
        nibble = (s32)(byte << 28) >> 28;
    } else {
        byte = *info->encodeData;
        nibble = (s32)((byte << 24) & 0xF0000000) >> 28;
    }

    info->offsetNibbles++;
    return (nibble << info->scale) << 11;
}

static inline void Initialize(THPAudioDecodeInfo *info, u8 *ptr) {
    info->offsetNibbles = 2;
    info->predictor = (*ptr >> 4) & 0x7;
    info->scale = *ptr & 0xF;
    info->encodeData = ptr + 1;
}

// s16 sample = ((yn1 * c0 + yn2 * c1 + new) * 32 + 0x8000) >> 16 in 64 bit, saturated to s32 first
static inline s32 Predict(THPAudioDecodeInfo *info, s16 coefs[8][2], s32 sample, s32 yn1, s32 yn2) {
    s64 acc;

    acc = (s64)((s16)yn2 * coefs[info->predictor][1]) + (s64)((s16)yn1 * coefs[info->predictor][0]);
    acc += sample;
    acc = (acc << 5) + 0x8000;

    if (acc > 0x7FFFFFFF) {
        acc = 0x7FFFFFFF;
    }

    if (acc < -0x80000000LL) {
        acc = -0x80000000LL;
    }

    return (s32)(acc >> 16);
}

u32 THPAudioDecode(void *dst, void *src, u32 flag) {
    s16 *audioBuffer = (s16 *)dst;
    u8 *audioFrame = (u8 *)src;
    THPAudioRecordHeader *header;
    u8 *ptr;
    s32 yn1;
    s32 yn2;
    u8 *rptr;
    THPAudioDecodeInfo info;
    s16 *left;
    s16 *right;
    u32 i;
    s32 sample;
    s32 step;

    if (audioBuffer == NULL || audioFrame == NULL) {
        return 0;
    }

    header = (THPAudioRecordHeader *)audioFrame;
    ptr = audioFrame + sizeof(THPAudioRecordHeader);
    rptr = ptr + header->offsetNextChannel;

    if ((s32)flag == 1) {
        right = audioBuffer;
        left = audioBuffer + header->sampleSize;
        step = 1;
    } else {
        right = audioBuffer;
        left = audioBuffer + 1;
        step = 2;
    }

    if (header->offsetNextChannel == 0) {
        // mono: same samples to both channels
        Initialize(&info, ptr);
        yn1 = header->lYn1;
        yn2 = header->lYn2;

        for (i = 0; i < header->sampleSize; i++) {
            sample = Predict(&info, header->lCoef, GetNewSample(&info), yn1, yn2);
            yn2 = yn1;
            yn1 = sample;
            *left = sample;
            left += step;
            *right = sample;
            right += step;
        }
    } else {
        Initialize(&info, ptr);
        yn1 = header->lYn1;
        yn2 = header->lYn2;

        for (i = 0; i < header->sampleSize; i++) {
            sample = Predict(&info, header->lCoef, GetNewSample(&info), yn1, yn2);
            yn2 = yn1;
            yn1 = sample;
            *left = sample;
            left += step;
        }

        Initialize(&info, rptr);
        yn1 = header->rYn1;
        yn2 = header->rYn2;

        for (i = 0; i < header->sampleSize; i++) {
            sample = Predict(&info, header->rCoef, GetNewSample(&info), yn1, yn2);
            yn2 = yn1;
            yn1 = sample;
            *right = sample;
            right += step;
        }
    }

    return header->sampleSize;
}
