#include <types.h>

// The original calls local helpers out of line (e.g. move_ptr from rewind_ptr).
#pragma auto_inline off

// Function bodies below are taken from https://github.com/jj-link/brawl.

void TMCC_JPEGDEC_IdctBlock1x1(int *arg0, signed char *arg1)
{
    int flag = 0;
    int v = (*arg0 >> 11) + 0x80;

    if (v < 0x100 && v > -1) {
        flag = 1;
    }

    *arg1 = flag ? v : (v < 0 ? 0 : 255);
}

void TMCC_JPEGDEC_IdctBlock1x1_Col(s32 *arg0, s8 *arg1) {
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

s32 TMCC_JPEGDEC_load_buff(TMCC_JPEGDEC_Stream* stream) {
    u32 word;
    s32 shift;
    u8 byte;
    u8* cur;
    cur = stream->cur;

    if (cur < stream->limit) {
        word = stream->unk0;
        shift = stream->unk4;
        do {
            byte = *cur++;
            word = (word << 8) + byte;
            shift += 8;
            if (byte == 0xFF) {
                cur++;
            }
        } while (shift <= 0x18);
        stream->unk0 = word;
        stream->unk4 = shift;
        stream->cur = cur;
    } else {
        do {
            if (stream->end <= stream->cur) {
                break;
            }
            cur = stream->cur;
            byte = *cur++;
            stream->cur = cur;
            stream->unk0 = (stream->unk0 << 8) + byte;
            stream->unk4 += 8;
            if (byte == 0xFF) {
                stream->cur = cur + 1;
            }
        } while (stream->unk4 <= 0x18);
    }
    return 0;
}

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

void TMCC_JPEGDEC_IdctBlock2x2(s32* block, u8* out, s32 stride) {
    s32 flag = 0;
    s32 v;
    s32 a;
    s32 b;
    s32 c;
    s32 d;
    s32 sum0;
    s32 diff0;
    s32 sum1;
    s32 diff1;

    a = block[0] + 0x40000;
    b = block[1];
    sum0 = a + b;
    diff0 = a - b;
    c = block[8];
    d = block[9];
    sum1 = c + d;
    diff1 = c - d;

    v = (sum0 + sum1) >> 11;
    if (v < 0x100 && v > -1) {
        flag = 1;
    }
    out[0] = flag ? v : (v < 0 ? 0 : 255);

    v = (sum0 - sum1) >> 11;
    flag = 0;
    if (v < 0x100 && v > -1) {
        flag = 1;
    }
    out[stride] = flag ? v : (v < 0 ? 0 : 255);

    v = (diff0 + diff1) >> 11;
    flag = 0;
    if (v < 0x100 && v > -1) {
        flag = 1;
    }
    out[1] = flag ? v : (v < 0 ? 0 : 255);

    v = (diff0 - diff1) >> 11;
    flag = 0;
    if (v < 0x100 && v > -1) {
        flag = 1;
    }
    out[stride + 1] = flag ? v : (v < 0 ? 0 : 255);
}

void TMCC_JPEGDEC_IdctBlock2x2_Col(s32* block, s8* out) {
    s32 flag = 0;
    s32 v;
    s32 a;
    s32 b;
    s32 c;
    s32 d;
    s32 sum0;
    s32 diff0;
    s32 sum1;
    s32 diff1;

    a = block[0];
    b = block[1];
    c = block[8];
    d = block[9];
    sum0 = a + b;
    diff0 = a - b;
    sum1 = c + d;
    diff1 = c - d;

    v = (sum0 + sum1) >> 11;
    if (v < 128 && v > -129) {
        flag = 1;
    }
    out[0] = flag ? v : (v > 0 ? 127 : -128);

    v = (sum0 - sum1) >> 11;
    flag = 0;
    if (v < 128 && v > -129) {
        flag = 1;
    }
    out[8] = flag ? v : (v > 0 ? 127 : -128);

    v = (diff0 + diff1) >> 11;
    flag = 0;
    if (v < 128 && v > -129) {
        flag = 1;
    }
    out[1] = flag ? v : (v > 0 ? 127 : -128);

    v = (diff0 - diff1) >> 11;
    flag = 0;
    if (v < 128 && v > -129) {
        flag = 1;
    }
    out[9] = flag ? v : (v > 0 ? 127 : -128);
}

s32 TMCC_JPEGDEC_move_ptr(s32 offset, TMCC_JPEGDEC_Stream* stream);

s32 TMCC_JPEGDEC_rewind_ptr(TMCC_JPEGDEC_Stream* stream) {
    u32 word = stream->unk0;
    s32 shift = stream->unk4;
    s32 result;

    shift -= 8;
    while (shift >= 0) {
        result = TMCC_JPEGDEC_move_ptr(-1, stream);
        if (result < 0) {
            return result;
        }
        if ((word & 0xFF) == 0xFF) {
            result = TMCC_JPEGDEC_move_ptr(-1, stream);
            if (result < 0) {
                return result;
            }
        }
        word >>= 8;
        shift -= 8;
    }
    stream->unk4 = 0;
    stream->unk0 = 0;
    return 0;
}

// HYPOTHESIS: the context starts with the stream; unk19D0 points at a status block.
typedef struct TMCC_JPEGDEC_Status {
    u8 pad0[4];
    s32 error;
    u8 pad8[0x25];
    u8 mode;
} TMCC_JPEGDEC_Status;

typedef struct TMCC_JPEGDEC_Ctx2 {
    TMCC_JPEGDEC_Stream stream;
    u8 pad18[0x17F0];
    u8 unk1808;
    u8 pad1809[0x1C7];
    TMCC_JPEGDEC_Status* status;
} TMCC_JPEGDEC_Ctx2;

s32 TMCC_JPEGDEC_get_wbyte(u16* out, TMCC_JPEGDEC_Stream* stream);

s32 TMCC_JPEGDEC_imageend(TMCC_JPEGDEC_Ctx2* context) {
    u16 marker;
    s32 result;

    if (context->status->mode == 1) {
        return 0;
    }
    if (context->unk1808 == 0) {
        result = TMCC_JPEGDEC_rewind_ptr(&context->stream);
        if (result < 0) {
            return result;
        }
        result = TMCC_JPEGDEC_get_wbyte(&marker, &context->stream);
        if (result < 0) {
            if (result != -0x90) {
                return result;
            }
        }
        if (marker != 0xFFD9 && marker != 0xFFDC) {
            context->status->error = -0x21;
            return 0;
        }
    }
    return 0;
}

// HYPOTHESIS: EXIF thumbnail check; layouts inferred from the offsets used.
typedef struct TMCC_JPEGDEC_ExifInfo {
    u8 pad0[4];
    u8 kind;
    u8 pad5[3];
    u32 limit;
    u8 pad0C[0x10];
    u8 unk1C;
    u8 unk1D;
} TMCC_JPEGDEC_ExifInfo;

typedef struct TMCC_JPEGDEC_ExifStatus {
    u8 pad0[8];
    s32 kind;
    u8 pad0C[0x20];
    u8 unk2C;
    u8 unk2D;
    u8 pad2E[0x68E];
    u16 unk6BC;
    u8 pad6BE[0x16];
    u32 offset;
    u32 length;
    u32 base;
} TMCC_JPEGDEC_ExifStatus;

typedef struct TMCC_JPEGDEC_ExifCtx {
    TMCC_JPEGDEC_Stream stream;
    u8 pad18[0x19B8];
    TMCC_JPEGDEC_ExifStatus* status;
} TMCC_JPEGDEC_ExifCtx;

s32 TMCC_JPEGDEC_ThumbnailCheck(TMCC_JPEGDEC_ExifInfo* info, TMCC_JPEGDEC_ExifCtx* context, u32 arg2, u32 arg3) {
    TMCC_JPEGDEC_ExifStatus* status = context->status;
    u32 end;
    u32 size;
    s32 result;

    if (status->offset == 0) {
        return -0xA0;
    }
    if (status->length == 0) {
        return -0xA0;
    }
    if (status->unk6BC != 6) {
        return -0xA0;
    }
    end = status->base - arg3 + (status->length + status->offset);
    if (end > info->limit) {
        return -0xA0;
    }
    size = status->length - (end - arg2);
    if (end <= arg2) {
        size = status->length;
    }
    if (info->unk1C != 1) {
        return -1;
    }
    if (info->kind != 4 && info->kind != 6) {
        return -4;
    }
    if (info->unk1D != 1) {
        return -1;
    }
    status->unk2D = 0;
    status->unk2C = info->unk1D;
    status->kind = info->kind;
    result = TMCC_JPEGDEC_set_thumbnaildata(&context->stream, (u8*)(status->base + status->offset), size);
    return result < 0 ? result : 0;
}

s32 TMCC_JPEGDEC_get_wbyte(u16* out, TMCC_JPEGDEC_Stream* stream) {
    u8* next;
    u8* end;
    u8* cur;
    u8 byte;
    u32 acc;
    s32 result;

    end = stream->end;
    cur = stream->cur;
    if (cur >= end) {
        result = -0x90;
    } else {
        next = cur + 1;
        stream->cur = next;
        byte = *cur;
        result = (next >= end) ? -0x90 : 0;
        cur = next;
    }
    acc = byte << 8;
    if (result < 0) {
        return result;
    }
    if (cur >= end) {
        result = -0x90;
    } else {
        u8* p = stream->cur;
        end = stream->end;
        next = p + 1;
        stream->cur = next;
        byte = *p;
        result = (next >= end) ? -0x90 : 0;
    }
    *out = acc + byte;
    return result < 0 ? result : 0;
}

// HYPOTHESIS: one Huffman code-length slot, two big-endian halfwords.
typedef struct TMCC_JPEGDEC_HuffPair {
    u16 maxcode;
    u16 offset;
} TMCC_JPEGDEC_HuffPair;

s32 TMCC_JPEGDEC_vl_decode_rc(u8* table, u8* vals, TMCC_JPEGDEC_Stream* stream) {
    TMCC_JPEGDEC_HuffPair* entry;
    TMCC_JPEGDEC_HuffPair pair;
    u32 word;
    u16 maxcode;
    u32 bits;
    u32 code;
    u32 len;
    u32 bit;
    u32 index;
    s32 result;

    if (stream->unk4 <= 0x11) {
        result = TMCC_JPEGDEC_load_buff(stream);
        if (result < 0) {
            return result;
        }
    }
    entry = (TMCC_JPEGDEC_HuffPair*)(table + 0x24);
    bits = (u32)stream->unk4 - 9;
    code = (*(u32*)&stream->unk0 >> bits) & 0x1FF;
    stream->unk4 = (s32)bits;
    len = 9;
    goto check;
    for (;;) {
        len += 1;
        entry += 1;
        if (len > 0x10) {
            return -0x64;
        }
        bits = (u32)stream->unk4 - 1;
        bit = *(u32*)&stream->unk0 >> bits;
        bit &= 1;
        code <<= 1;
        code |= bit;
        stream->unk4 = (s32)bits;
    check:
        pair = *entry;
        word = *(u32*)&pair;
        maxcode = *(u16*)&word;
        if (code <= maxcode) {
            index = code - pair.maxcode;
            index = index + pair.offset;
            return vals[index];
        }
    }
}
