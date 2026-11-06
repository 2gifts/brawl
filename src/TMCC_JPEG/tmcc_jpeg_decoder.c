#include <types.h>

// Function bodies below are taken from https://github.com/jj-link/brawl.

void fn_80265AE0(int *arg0, signed char *arg1)
{
    int flag = 0;
    int v = (*arg0 >> 11) + 0x80;

    if (v < 0x100 && v > -1) {
        flag = 1;
    }

    *arg1 = flag ? v : (v < 0 ? 0 : 255);
}

void fn_80265E6C(s32 *arg0, s8 *arg1) {
    s32 value;
    s32 valid;

    valid = 0;
    value = *arg0 >> 11;
    if (value < 128 && value > -129) {
        valid = 1;
    }
    *arg1 = valid ? value : (value > 0 ? 127 : -128);
}

// HYPOTHESIS: stream buffer state used by the buffer_system functions.
typedef struct TMCC_JPEGDEC_Stream {
    s32 unk0;
    s32 unk4;
    u8* start;
    u8* cur;
    u8* end;
    u8* limit;
} TMCC_JPEGDEC_Stream;

s32 TMCC_JPEGDEC_get_position(TMCC_JPEGDEC_Stream* stream) {
    return stream->cur - stream->start;
}

s32 TMCC_JPEGDEC_chk_possible_size(TMCC_JPEGDEC_Stream* stream) {
    return stream->end - stream->cur;
}

s32 TMCC_JPEGDEC_set_thumbnaildata(TMCC_JPEGDEC_Stream* stream, u8* data, s32 size) {
    stream->cur = data;
    stream->start = data;
    stream->end = data + size;
    stream->limit = data + size - 10;
    return 0;
}

s32 TMCC_JPEGDEC_load_buff(TMCC_JPEGDEC_Stream* stream);

s32 TMCC_JPEGDEC_init_buff(TMCC_JPEGDEC_Stream* stream) {
    stream->unk0 = 0;
    stream->unk4 = 0;
    return TMCC_JPEGDEC_load_buff(stream);
}

s32 TMCC_JPEGDEC_init_stream(TMCC_JPEGDEC_Stream* stream, u8* data, s32 size) {
    if (data == 0) {
        return -1;
    }
    if (size == 0) {
        return -1;
    }
    if ((u32)data & 0x1F) {
        return -1;
    }
    stream->start = data;
    stream->cur = data;
    stream->end = data + size;
    stream->limit = data + size - 10;
    return 0;
}

s32 TMCC_JPEGDEC_get_byte(u8* out, TMCC_JPEGDEC_Stream* stream) {
    u8* cur = stream->cur;
    u8* end;
    if (cur >= stream->end) {
        return -0x90;
    }
    *out = *cur++;
    end = stream->end;
    stream->cur = cur;
    return cur >= end ? -0x90 : 0;
}

s32 TMCC_JPEGDEC_get_sbyte(u8* out, u32 count, TMCC_JPEGDEC_Stream* stream) {
    u32 i;
    s32 result;
    u8* end;
    u8* cur;
    u8 value;
    for (i = 0; i < count; i++) {
        cur = stream->cur;
        end = stream->end;
        if (cur >= end) {
            result = -0x90;
        } else {
            stream->cur = cur + 1;
            value = *cur;
            result = (cur + 1 >= end) ? -0x90 : 0;
        }
        *out++ = value;
        if (result < 0) {
            return result;
        }
    }
    return 0;
}

s32 TMCC_JPEGDEC_move_ptr(s32 offset, TMCC_JPEGDEC_Stream* stream) {
    s32 back;
    if (offset >= 0) {
        if (stream->end - stream->cur >= offset) {
            stream->cur = stream->cur + offset;
            if (stream->end <= stream->cur) {
                return -0x90;
            }
        } else {
            return -0x90;
        }
    } else {
        back = -offset;
        if (stream->cur - stream->start >= back) {
            stream->cur = stream->cur - back;
        } else {
            return -0x90;
        }
    }
    return 0;
}

// HYPOTHESIS: decoder context; only the fields the code touches are named.
typedef struct TMCC_JPEGDEC_Context {
    union {
        u8 pad0[0x17DC];
        struct {
            u8 pad0_440[0x440];
            u8* dcTable;
            u8* dcSizes;
            u8* dcCodes;
            u8 pad44C[4];
            u8* acTable;
            u8* acSizes;
            u8* acCodes;
            u8 pad45C[4];
            u8 dcTable0[0x400];
            u8 dcTable1[0x400];
            u8 acTable0[0x400];
            u8 acTable1[0x400];
            u8 dcSizes0[0x44];
            u8 dcSizes1[0x44];
            u8 acSizes0[0x44];
            u8 acSizes1[0x44];
            u8 dcCodes0[0x10];
            u8 dcCodes1[0x10];
            u8 acCodes0[0x10];
            u8 pad15A0[0xF0];
            u8 acCodes1[0x14C];
        };
    };
    u16 unk17DC;
    u8 pad17DE[2];
    s32 unk17E0;
    s32 unk17E4;
    u8 pad17E8;
    u8 unk17E9;
    u8 unk17EA;
    u8 pad17EB;
    u16 unk17EC;
    u16 unk17EE;
    u8 pad17F0[0x18];
    u8 unk1808;
} TMCC_JPEGDEC_Context;

s32 TMCC_JPEGDEC_scanstart(TMCC_JPEGDEC_Context* context);
s32 TMCC_JPEGDEC_imagestart(TMCC_JPEGDEC_Context* context);

s32 TMCC_JPEGDEC_Decompscan(TMCC_JPEGDEC_Context* context) {
    s32 result = TMCC_JPEGDEC_scanstart(context);
    if (result < 0) {
        return result;
    }
    if (context->unk1808 == 1) {
        return 0;
    }
    return result;
}

s32 TMCC_JPEGDEC_HeaderAnalyze(TMCC_JPEGDEC_Context* context) {
    s32 result = TMCC_JPEGDEC_imagestart(context);
    if (result < 0) {
        return result;
    }
    if (context->unk17DC == 0) {
        result = -0x50;
    } else {
        context->unk17E0 = context->unk17E9 * context->unk17EC;
        context->unk17E4 = context->unk17EA * context->unk17EE;
    }
    return result;
}

void TMCC_JPEGDEC_set_entropytbl(TMCC_JPEGDEC_Context* context, s32 dcSelect, s32 acSelect) {
    switch (dcSelect) {
    case 0:
        context->dcTable = context->dcTable0;
        context->dcCodes = context->dcCodes0;
        context->dcSizes = context->dcSizes0;
        break;
    case 1:
        context->dcTable = context->dcTable1;
        context->dcCodes = context->dcCodes1;
        context->dcSizes = context->dcSizes1;
        break;
    }
    switch (acSelect) {
    case 0:
        context->acTable = context->acTable0;
        context->acCodes = context->acCodes0;
        context->acSizes = context->acSizes0;
        break;
    case 1:
        context->acTable = context->acTable1;
        context->acCodes = context->acCodes1;
        context->acSizes = context->acSizes1;
        break;
    }
}

// HYPOTHESIS: image state used by the RGB565 tile functions; only touched fields are named.
typedef struct TMCC_JPEGDEC_Image {
    u8 pad0[0x30];
    u16 validWidth;
    u16 validHeight;
    u8 pad34[0x14];
    u32 width;
    u32 height;
    u8 pad50[0x14];
    u16* pixels;
} TMCC_JPEGDEC_Image;

void TMCC_JPEGDEC_fillerBlankTileRGB565(TMCC_JPEGDEC_Image* image) {
    u16* pixels = image->pixels;
    u32 tilesPerRow = image->width >> 2;
    u32 x;
    u32 y;
    for (y = 0; y < image->validHeight; y++) {
        for (x = image->validWidth; x < image->width; x++) {
            pixels[(((y >> 2) * tilesPerRow + (x >> 2)) << 4) + ((y & 3) << 2) + (x & 3)] = 0xFFFF;
        }
    }
    for (y = image->validHeight; y < image->height; y++) {
        for (x = 0; x < image->width; x++) {
            pixels[(((y >> 2) * tilesPerRow + (x >> 2)) << 4) + ((y & 3) << 2) + (x & 3)] = 0xFFFF;
        }
    }
}
