#include <types.h>
#include <string.h>

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
            TMCC_JPEGDEC_Stream stream;
            u8 scanCompIdx[4];
            s32 dcPred[4];
            u8 pad2C[0x10];
            u16 unk3C;
            u16 unk3E;
            u32 unk40;
            u8 pad44[0x400];
            u8 zzTbl[64];
        };
        struct {
            u8 pad0_44[0x44];
            s32 qtable[5][64];
            u8 pad544[0x1290];
            u8 qtableFlag[8];
        };
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
    u16 unk17DE;
    s32 unk17E0;
    s32 unk17E4;
    u8 pad17E8;
    u8 unk17E9;
    u8 unk17EA;
    u8 pad17EB;
    u16 unk17EC;
    u16 unk17EE;
    u8 pad17F0[0x16];
    u16 unk1806;
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
typedef struct TMCC_JPEGDEC_ExifCtx TMCC_JPEGDEC_ExifCtx;

typedef struct TMCC_JPEGDEC_ExifInfo {
    TMCC_JPEGDEC_ExifCtx* ctx;
    u8 kind;
    u8 pad5[3];
    u32 limit;
    u8 pad0C[0x10];
    u8 unk1C;
    u8 unk1D;
} TMCC_JPEGDEC_ExifInfo;

typedef struct TMCC_JPEGDEC_ExifStatus {
    TMCC_JPEGDEC_ExifCtx* ctx;
    u8 pad4[4];
    s32 kind;
    u8 pad0C[4];
    s32 unk10;
    u8 pad14[4];
    s32 unk18;
    u8 pad1C[0x10];
    u8 unk2C;
    u8 unk2D;
    u8 pad2E[0x3A];
    u8 unk68[0x654];
    u16 unk6BC;
    u8 pad6BE[0x16];
    u32 offset;
    u32 length;
    u32 base;
    u8 pad6E0[0x1C];
} TMCC_JPEGDEC_ExifStatus;

struct TMCC_JPEGDEC_ExifCtx {
    TMCC_JPEGDEC_Stream stream;
    u8 pad18[0x19B8];
    TMCC_JPEGDEC_ExifStatus* status;
};

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

s32 TMCC_JPEGDEC_get_byte(u8* out, TMCC_JPEGDEC_Stream* stream);
s32 TMCC_JPEGDEC_init_buff(TMCC_JPEGDEC_Stream* stream);
s32 fn_802635B4(void* unused, TMCC_JPEGDEC_Context* ctx);

typedef struct TMCC_JPEGDEC_ScaleTable {
    s32 vals[64];
} TMCC_JPEGDEC_ScaleTable;

extern const TMCC_JPEGDEC_ScaleTable lbl_8040DBE0;
extern const u8 lbl_8040DD10[64];

// HYPOTHESIS: Huffman & quant table area at ctx + 0x44
typedef struct TMCC_JPEGDEC_Tables {
    s32 qvals[4][64];
    u8 pad400[0x60];
    u8 dcLut[2][0x400];
    u8 acLut[2][0x400];
    TMCC_JPEGDEC_HuffPair dcPairs[2][17];
    TMCC_JPEGDEC_HuffPair acPairs[2][17];
    u8 dcVals[2][0x10];
    u8 acVals[2][0x100];
    u8 qFlags[4];
    u8 dcFlags[2];
    u8 acFlags[2];
} TMCC_JPEGDEC_Tables;

typedef struct TMCC_JPEGDEC_HuffOut {
    u8* table;
    u8* sizes;
    u8* codes;
    u8 count;
} TMCC_JPEGDEC_HuffOut;

void fn_80260DC4(TMCC_JPEGDEC_HuffOut* out, s32 dcAc, s32 id, TMCC_JPEGDEC_Tables* tbl) {
    if (dcAc == 0) {
        switch (id) {
        case 0:
            out->table = tbl->dcLut[0];
            out->codes = tbl->dcVals[0];
            out->sizes = (u8*)tbl->dcPairs[0];
            tbl->dcFlags[0] = 1;
            memset(tbl->dcLut[0], 0, 0x400);
            memset(tbl->dcVals[0], 0, 0x10);
            memset(tbl->dcPairs[0], 0, 0x44);
            break;
        case 1:
            out->table = tbl->dcLut[1];
            out->codes = tbl->dcVals[1];
            out->sizes = (u8*)tbl->dcPairs[1];
            tbl->dcFlags[1] = 1;
            memset(tbl->dcLut[1], 0, 0x400);
            memset(tbl->dcVals[1], 0, 0x10);
            memset(tbl->dcPairs[1], 0, 0x44);
            break;
        }
    } else {
        switch (id) {
        case 0:
            out->table = tbl->acLut[0];
            out->codes = tbl->acVals[0];
            out->sizes = (u8*)tbl->acPairs[0];
            tbl->acFlags[0] = 1;
            memset(tbl->acLut[0], 0, 0x400);
            memset(tbl->acVals[0], 0, 0x100);
            memset(tbl->acPairs[0], 0, 0x44);
            break;
        case 1:
            out->table = tbl->acLut[1];
            out->codes = tbl->acVals[1];
            out->sizes = (u8*)tbl->acPairs[1];
            tbl->acFlags[1] = 1;
            memset(tbl->acLut[1], 0, 0x400);
            memset(tbl->acVals[1], 0, 0x100);
            memset(tbl->acPairs[1], 0, 0x44);
            break;
        }
    }
}

typedef struct TMCC_JPEGDEC_DqtTable {
    s32 vals[5][64];
    u8 pad544[0x1290];
    u8 flags[8];
} TMCC_JPEGDEC_DqtTable;

#pragma use_lmw_stmw on

s32 fn_80263794(TMCC_JPEGDEC_Context* ctx) {
    TMCC_JPEGDEC_ScaleTable scale = lbl_8040DBE0;
    TMCC_JPEGDEC_DqtTable* dqt = (TMCC_JPEGDEC_DqtTable*)ctx->qtable;
    u16 len;
    u8 id;
    u8 val;
    s32 i;
    s32 result;

    result = TMCC_JPEGDEC_get_wbyte(&len, &ctx->stream);
    if (result < 0) {
        return result;
    }
    len -= 2;

    do {
        len -= 0x41;
        result = TMCC_JPEGDEC_get_byte(&id, &ctx->stream);
        if (result < 0) {
            return result;
        }
        if (id > 4) {
            return -0x41;
        }
        dqt->flags[id] = 1;
        for (i = 0; i < 64; i++) {
            s32 zz = lbl_8040DD10[i];
            if (zz > 0x3F) {
                return -0x41;
            }
            result = TMCC_JPEGDEC_get_byte(&val, &ctx->stream);
            if (result < 0) {
                return result;
            }
            if ((dqt->vals[id][zz] = val * scale.vals[zz]) == 0) {
                return -0x41;
            }
        }
    } while (len != 0);

    return 0;
}

#pragma use_lmw_stmw off

// HYPOTHESIS: scan geometry sub-struct at ctx + 0x17DC
typedef struct TMCC_JPEGDEC_ScanSub {
    u16 width;
    u16 height;
    s32 unk4;
    s32 unk8;
    u8 unkC;
    u8 mcuW;
    u8 mcuH;
    u8 padF;
    u16 mcusPerRow;
    u16 mcusPerCol;
    u32 totalMcus;
    u8 remX;
    u8 remY;
    u8 numComps;
    u8 scanNumComps;
    u8 compBlocks[4];
    u8 compH[4];
    u8 compV[4];
    u8 maxH;
    u8 maxV;
} TMCC_JPEGDEC_ScanSub;

s32 fn_80262F7C(TMCC_JPEGDEC_Context* ctx) {
    s32 i;
    TMCC_JPEGDEC_ScanSub* sub = (TMCC_JPEGDEC_ScanSub*)((u8*)ctx + 0x17DC);

    sub->unkC |= 0x100;
    if (sub->scanNumComps == 1) {
        u8 idx = ctx->scanCompIdx[0];
        sub->mcuW = (sub->maxH << 3) / sub->compH[idx];
        sub->mcuH = (sub->maxV << 3) / sub->compV[idx];
        sub->mcusPerRow = sub->width / sub->mcuW;
        sub->mcusPerCol = sub->height / sub->mcuH;
    } else {
        sub->mcuW = sub->maxH << 3;
        sub->mcuH = sub->maxV << 3;
        sub->mcusPerRow = sub->width / sub->mcuW;
        sub->mcusPerCol = sub->height / sub->mcuH;
    }

    sub->remX = sub->width % sub->mcuW;
    sub->remY = sub->height % sub->mcuH;
    sub->mcusPerRow += (sub->remX != 0);
    sub->mcusPerCol += (sub->remY != 0);
    sub->totalMcus = sub->mcusPerRow * sub->mcusPerCol;

    for (i = 0; i < sub->scanNumComps; i++) {
        u8 idx = ctx->scanCompIdx[i];
        sub->compBlocks[i] = (sub->scanNumComps == 1) ? 1 : (sub->compH[idx] * sub->compV[idx]);
    }
    for (; i < 4; i++) {
        sub->compBlocks[i] = 0;
    }

    return 0;
}

typedef struct TMCC_JPEGDEC_SosTable {
    u8 scanCompIdx[4];
    u8 dcSel[4];
    u8 acSel[4];
    u8 padC[8];
    u8 compId[4];
    u8 compQIdx[4];
    u8 compDcSel[4];
    u8 compAcSel[4];
} TMCC_JPEGDEC_SosTable;

#pragma use_lmw_stmw on

s32 fn_80263C58(TMCC_JPEGDEC_Context* ctx) {
    s32 i;
    TMCC_JPEGDEC_SosTable* sos = (TMCC_JPEGDEC_SosTable*)((u8*)ctx + 0x18);
    TMCC_JPEGDEC_DqtTable* dqt = (TMCC_JPEGDEC_DqtTable*)ctx->qtable;
    u16 len;
    u8 byte;
    u8* compPtr;
    s32 result;

    result = TMCC_JPEGDEC_get_wbyte(&len, &ctx->stream);
    if (result < 0) {
        return result;
    }
    if (len < 2) {
        return -0x51;
    }

    result = TMCC_JPEGDEC_get_byte(&byte, &ctx->stream);
    if (result < 0) {
        return result;
    }
    if ((*((u8*)ctx + 0x17F7) = byte) > 4 || *((u8*)ctx + 0x17F7) != *((u8*)ctx + 0x17F6)) {
        return -0x51;
    }

    for (i = 0; i < *((u8*)ctx + 0x17F7); i++) {
        s32 j;
        s32 dcSel;
        s32 acSel;
        s32 numComps;
        s32 id;

        result = TMCC_JPEGDEC_get_byte(&byte, &ctx->stream);
        if (result < 0) {
            return result;
        }
        numComps = *((u8*)ctx + 0x17F6);
        for (j = 0; j < numComps; j++) {
            id = byte;
            if (id == (s32)sos->compId[j]) {
                sos->scanCompIdx[i] = j;
                compPtr = &sos->scanCompIdx[i];
                goto found;
            }
        }
        return -0x51;

    found:
        result = TMCC_JPEGDEC_get_byte(&byte, &ctx->stream);
        if (result < 0) {
            return result;
        }
        dcSel = (s32)byte >> 4;
        acSel = byte & 0xF;
        if (dcSel > 1 || acSel > 1) {
            return -0x51;
        }

        sos->compDcSel[*compPtr] = dcSel;
        sos->compAcSel[*compPtr] = acSel;
        if (dqt->flags[4 + dcSel] != 1) {
            return -0x40;
        }
        if (dqt->flags[6 + acSel] != 1) {
            return -0x40;
        }
        if (dqt->flags[compPtr[0x18]] != 1) {
            return -0x41;
        }
    }

    result = TMCC_JPEGDEC_move_ptr(3, &ctx->stream);
    return result < 0 ? result : 0;
}

#pragma use_lmw_stmw off

static inline s32 skip_app_marker(TMCC_JPEGDEC_Context* ctx) {
    u16 len;
    s32 result = TMCC_JPEGDEC_get_wbyte(&len, &ctx->stream);
    if (result < 0) {
        return result;
    }
    if (len < 2) {
        return -0x45;
    }
    len -= 2;
    result = TMCC_JPEGDEC_move_ptr(len, &ctx->stream);
    result = result < 0 ? result : 0;
    return result < 0 ? result : 0;
}

static inline s32 parse_dri_marker(TMCC_JPEGDEC_Context* ctx) {
    u16 val;
    s32 result = TMCC_JPEGDEC_get_wbyte(&val, &ctx->stream);
    if (result < 0) {
        return result;
    }
    if (val != 4) {
        return -0x42;
    }
    result = TMCC_JPEGDEC_get_wbyte(&val, &ctx->stream);
    if (result < 0) {
        return result;
    }
    ctx->unk1806 = val;
    return 0;
}

static inline s32 parse_dnl_inline(TMCC_JPEGDEC_Context* ctx) {
    u16 val;
    s32 result = TMCC_JPEGDEC_get_wbyte(&val, &ctx->stream);
    if (result < 0) {
        return result;
    }
    if (val != 4) {
        return -0x43;
    }
    result = TMCC_JPEGDEC_get_wbyte(&val, &ctx->stream);
    if (result < 0) {
        return result;
    }
    ctx->unk17DE = val;
    return 0;
}

static inline s32 skip_com_marker(TMCC_JPEGDEC_Context* ctx) {
    u16 len;
    s32 result = TMCC_JPEGDEC_get_wbyte(&len, &ctx->stream);
    if (result < 0) {
        return result;
    }
    if (len < 2) {
        return -0x44;
    }
    len -= 2;
    result = TMCC_JPEGDEC_move_ptr(len, &ctx->stream);
    result = result < 0 ? result : 0;
    return result < 0 ? result : 0;
}

#pragma use_lmw_stmw on

s32 fn_802632F4(u16* out, TMCC_JPEGDEC_Context* ctx) {
    s32 done = 0;
    u16 initMarker;
    u16 marker;
    s32 result;

    initMarker = *out;

    do {
        result = TMCC_JPEGDEC_get_wbyte(&marker, &ctx->stream);
        if (result < 0) {
            return result;
        }
        while (marker == 0xFFFF) {
            u8 b;
            result = TMCC_JPEGDEC_get_byte(&b, &ctx->stream);
            if (result < 0) {
                return result;
            }
            marker = b | 0xFF00;
        }

        if (marker >= 0xFFE0 && marker <= 0xFFEF) {
            result = skip_app_marker(ctx);
        } else {
            switch (marker) {
            case 0xFFC4:
                result = fn_802635B4((void*)initMarker, ctx);
                break;
            case 0xFFDB:
                result = fn_80263794(ctx);
                break;
            case 0xFFDD:
                result = parse_dri_marker(ctx);
                break;
            case 0xFFDC:
                result = parse_dnl_inline(ctx);
                break;
            case 0xFFFE:
                result = skip_com_marker(ctx);
                break;
            case 0xFFC0:
                done = 1;
                break;
            case 0xFFC2:
                done = 1;
                break;
            case 0xFFDA:
                done = 1;
                break;
            case 0xFFD9:
                ctx->unk1808 = 1;
                done = 1;
                break;
            default:
                result = -0x2F;
                break;
            }
        }
        if (result < 0) {
            done = 1;
        }
    } while (!done);

    *out = marker;
    return result;
}

#pragma use_lmw_stmw off

static inline s32 parse_dnl_marker(TMCC_JPEGDEC_Context* ctx) {
    u16 val;
    s32 result = TMCC_JPEGDEC_get_wbyte(&val, &ctx->stream);
    if (result < 0) {
        return result;
    }
    if (val != 4) {
        return -0x43;
    }
    result = TMCC_JPEGDEC_get_wbyte(&val, &ctx->stream);
    if (result < 0) {
        return result;
    }
    ctx->unk17DE = val;
    return 0;
}

static inline s32 find_dnl(TMCC_JPEGDEC_Context* ctx) {
    u8 byte;
    u32 word = 0;
    s32 result;
    TMCC_JPEGDEC_Stream saved = ctx->stream;

    while ((u16)word != 0xFFDC) {
        result = TMCC_JPEGDEC_get_byte(&byte, &ctx->stream);
        if (result < 0) {
            break;
        }
        word = ((word & 0xFF) << 8) | byte;
    }

    if ((u16)word == 0xFFDC) {
        result = parse_dnl_marker(ctx);
        if (result < 0) {
            return result;
        }
    }

    ctx->stream = saved;
    return 0;
}

s32 TMCC_JPEGDEC_scanstart(TMCC_JPEGDEC_Context* ctx) {
    u16 marker;
    s32 result;

    marker = 0;
    result = fn_802632F4(&marker, ctx);
    if (result < 0) {
        return result;
    }
    if (marker != 0xFFDA) {
        return -0x22;
    }

    result = fn_80263C58(ctx);
    if (result < 0) {
        return result;
    }

    result = find_dnl(ctx);
    if (result < 0) {
        return result;
    }

    if (ctx->unk17DE == 0) {
        return -0x50;
    }

    result = fn_80262F7C(ctx);
    if (result < 0) {
        return result;
    }

    ctx->dcPred[0] = 0;
    ctx->dcPred[1] = 0;
    ctx->dcPred[2] = 0;
    ctx->dcPred[3] = 0;
    ctx->unk3C = 0;
    result = TMCC_JPEGDEC_init_buff(&ctx->stream);
    if (result < 0) {
        return result;
    }
    ctx->unk3E = 0;
    ctx->unk40 = 0;
    return 0;
}

typedef struct TMCC_JPEGDEC_Unaligned16 {
    u8 b0;
    u8 b1;
} TMCC_JPEGDEC_Unaligned16;

s32 fn_802644D4(u8* data, u16 size, void* info);
s32 fn_8026260C(TMCC_JPEGDEC_ExifCtx* ctx);
s32 fn_80267980(TMCC_JPEGDEC_ExifCtx* ctx);
s32 fn_80265EB0(TMCC_JPEGDEC_ExifCtx* ctx);

#pragma use_lmw_stmw on

s32 fn_80264074(s32* outOffset, s32* outSize, u8* buf, TMCC_JPEGDEC_ExifInfo* info) {
    TMCC_JPEGDEC_ExifCtx* ctx = info->ctx;
    s32 result;
    u16 len;
    u16 marker;
    u16 soi;
    u8 sig[4];

    if (ctx == 0) {
        return -1;
    }

    memset(ctx, 0, sizeof(TMCC_JPEGDEC_ExifCtx));
    result = TMCC_JPEGDEC_init_stream(&ctx->stream, buf, info->limit);
    if (result < 0) {
        return result;
    }

    result = TMCC_JPEGDEC_get_wbyte(&soi, &ctx->stream);
    if (result < 0) {
        return result;
    }
    if (soi != 0xFFD8) {
        return -0x20;
    }

    do {
        result = TMCC_JPEGDEC_get_wbyte(&marker, &ctx->stream);
        if (result < 0) {
            return result;
        }
        if (marker == 0xFFE1) {
            result = TMCC_JPEGDEC_get_wbyte(&len, &ctx->stream);
            if (result < 0) {
                return result;
            }
            if (len < 2) {
                return -0x45;
            }
            result = TMCC_JPEGDEC_get_sbyte(sig, 4, &ctx->stream);
            if (result < 0) {
                return result;
            }
            if (sig[0] == 'E' && sig[1] == 'x' && sig[2] == 'i' && sig[3] == 'f') {
                *outOffset = TMCC_JPEGDEC_get_position(&ctx->stream) - 8;
                *outSize = len + 2;
                return 0;
            }
            return -0x45;
        } else {
            result = TMCC_JPEGDEC_get_wbyte(&len, &ctx->stream);
            if (result < 0) {
                return result;
            }
            if (len < 2) {
                return -0x45;
            }
            len -= 2;
            if (marker >= 0xFFE0 && marker <= 0xFFEF) {
                result = TMCC_JPEGDEC_move_ptr(len, &ctx->stream);
                if (result < 0) {
                    return result;
                }
            } else {
                switch (marker) {
                case 0xFFC0:
                case 0xFFC2:
                case 0xFFC4:
                case 0xFFDB:
                case 0xFFDC:
                case 0xFFDD:
                case 0xFFFE:
                    result = TMCC_JPEGDEC_move_ptr(len, &ctx->stream);
                    if (result < 0) {
                        return result;
                    }
                    break;
                case 0xFFDA:
                    return -2;
                case 0xFFD9:
                    return -2;
                default:
                    return -0x2F;
                }
            }
        }
    } while (TMCC_JPEGDEC_chk_possible_size(&ctx->stream) > 4);

    return -2;
}

s32 fn_802642F0(TMCC_JPEGDEC_ExifStatus* status, u8* data, TMCC_JPEGDEC_ExifInfo* info) {
    TMCC_JPEGDEC_ExifCtx* ctx = info->ctx;
    s32 result;
    u32 raw;
    u16 marker;
    u16 fullLen;
    u16 len;

    if (ctx == 0) {
        return -1;
    }

    memset(status, 0, sizeof(TMCC_JPEGDEC_ExifStatus));
    memset(ctx, 0, sizeof(TMCC_JPEGDEC_ExifCtx));
    ctx->status = status;
    status->ctx = ctx;

    raw = ((TMCC_JPEGDEC_Unaligned16*)data)->b0 | (((TMCC_JPEGDEC_Unaligned16*)data)->b1 << 8);
    marker = ((raw >> 8) & 0xFF) | ((raw & 0xFF) << 8);
    if (marker != 0xFFE1) {
        return -0x45;
    }

    raw = ((TMCC_JPEGDEC_Unaligned16*)(data + 2))->b0 | (((TMCC_JPEGDEC_Unaligned16*)(data + 2))->b1 << 8);
    len = ((raw >> 8) & 0xFF) | ((raw & 0xFF) << 8);
    if (len + 2 > info->limit) {
        return -0x91;
    }
    if (len < 2) {
        return -0x45;
    }

    fullLen = len + 2;
    result = fn_802644D4(data + 10, (u16)(len - 8), status->unk68);
    if (result < 0) {
        return result;
    }

    if (info->unk1C == 0) {
        return 0;
    }
    if (info->unk1C == 1) {
        result = TMCC_JPEGDEC_ThumbnailCheck(info, ctx, fullLen, (u32)data);
        if (result < 0) {
            return result;
        }
    } else {
        return -1;
    }

    result = TMCC_JPEGDEC_HeaderAnalyze((TMCC_JPEGDEC_Context*)ctx);
    if (result < 0) {
        goto err;
    }
    result = TMCC_JPEGDEC_Decompscan((TMCC_JPEGDEC_Context*)ctx);
    if (result < 0) {
        goto err;
    }
    if (((TMCC_JPEGDEC_Context*)ctx)->unk1808 != 0) {
        goto err;
    }
    result = fn_8026260C(ctx);
    if (result < 0) {
        goto err;
    }

    switch (status->kind) {
    case 4:
        result = fn_80267980(ctx);
        if (result < 0) {
            goto err;
        }
        break;
    case 5:
        break;
    case 6:
        result = fn_80265EB0(ctx);
        if (result < 0) {
            goto err;
        }
        break;
    default:
        break;
    }

    status->unk10 = TMCC_JPEGDEC_get_position(&ctx->stream);
    return status->unk18;

err:
    return result < 0 ? result : -2;
}

typedef struct TMCC_JPEGDEC_DecStatus {
    TMCC_JPEGDEC_Context* ctx;
    s32 error;
    u8 pad8[4];
    u16 mcuX;
    u16 mcuY;
    s32 unk10;
    u8 pad14[4];
    s32 unk18;
    u16 mcusPerRow;
    u16 mcusPerCol;
    u8 mcuW;
    u8 mcuH;
    u8 pad22[0x26];
    u32 width;
    u32 height;
    u32 cWidth;
    u32 cHeight;
    void* buf0;
    void* buf1;
    void* buf2;
    void* pixels;
} TMCC_JPEGDEC_DecStatus;

// HYPOTHESIS: MCU decode status view.
typedef struct TMCC_JPEGDEC_McuStatus {
    u8 pad0[0x24];
    u32 edgeX;
    u32 edgeY;
} TMCC_JPEGDEC_McuStatus;

// HYPOTHESIS: MCU decode context view.
typedef struct TMCC_JPEGDEC_McuCtx {
    u8 pad0[0x18];
    u8 scanCompIdx[4];
    s32 dcPred[4];
    u8 pad2C[4];
    u8 compQIdx[4];
    u8 compDcSel[4];
    u8 compAcSel[4];
    u8 pad3C[8];
    u8 qtable[0x1798];
    u8 pad17DC[0x1B];
    u8 scanNumComps;
    u8 compBlocks[4];
    u8 pad17FC[0xA];
    u16 unk1806;
    u8 pad1808[4];
    void (*idctLumi)(s32*, u8*, s32, s32);
    void (*idctCol)(s32*, s8*, s32, s32);
    s32 (*decodeIquant)(s32*, s32*, s32*, struct TMCC_JPEGDEC_McuCtx*);
    void (*convNormal)(struct TMCC_JPEGDEC_McuCtx*, s32, s32);
    void (*convEdge)(struct TMCC_JPEGDEC_McuCtx*, s32, s32);
    u8 pad1820[8];
    u8* yBufs[4];
    s8* cBufs[4];
    u8 pad1848[0x182];
    u16 unk19CA;
    u8 pad19CC[4];
    TMCC_JPEGDEC_McuStatus* status;
} TMCC_JPEGDEC_McuCtx;

s32 fn_802629B8(s32 x, s32 y, TMCC_JPEGDEC_McuCtx* ctx, void* block);
s32 fn_80263E18(TMCC_JPEGDEC_Context* ctx);

s32 fn_80262444(TMCC_JPEGDEC_DecStatus* status, s32 maxMcus, void* buf) {
    TMCC_JPEGDEC_Context* ctx = status->ctx;
    u16 x;
    u16 mcusPerRow;
    u16 y;
    u16 mcusPerCol;
    u8 mcuW;
    u8 mcuH;
    s32 count;
    s32 result;
    u8 block[0x180];

    if (ctx == 0) {
        return -1;
    }

    count = 0;
    status->buf0 = buf;
    status->pixels = buf;
    status->buf1 = (u8*)buf + status->width * status->height;
    status->buf2 = (u8*)status->buf1 + status->cWidth * status->cHeight;

    x = status->mcuX;
    y = status->mcuY;
    mcusPerRow = status->mcusPerRow;
    mcusPerCol = status->mcusPerCol;
    mcuW = status->mcuW;
    mcuH = status->mcuH;

    while (y < mcusPerCol && count < maxMcus) {
        s32 py = y * mcuH;
        while (x < mcusPerRow && count < maxMcus) {
            result = fn_802629B8(x * mcuW, py, (TMCC_JPEGDEC_McuCtx*)ctx, block);
            if (result < 0) {
                goto err;
            }
            x++;
            count++;
        }
        if (mcusPerRow == x) {
            x = 0;
            y++;
        }
    }

    if (mcusPerCol == y && x == 0) {
        result = TMCC_JPEGDEC_imageend((TMCC_JPEGDEC_Ctx2*)ctx);
        if (result < 0) {
            return result;
        }
    }
    status->mcuX = x;
    status->mcuY = y;
    status->unk10 = TMCC_JPEGDEC_get_position(&ctx->stream);
    return status->unk18 - y * mcusPerRow - x;

err:
    if (ctx->unk1806 != 0 && result != -0xF0) {
        status->error = result;
        return fn_80263E18(ctx);
    }
    return result;
}

#pragma use_lmw_stmw off

// HYPOTHESIS: status view for restart interval handling.
typedef struct TMCC_JPEGDEC_RstStatus {
    u8 pad0[0x1C];
    u16 mcusPerRow;
    u16 mcusPerCol;
    u8 mcuW;
    u8 mcuH;
} TMCC_JPEGDEC_RstStatus;

// HYPOTHESIS: context view for restart interval handling.
typedef struct TMCC_JPEGDEC_RstContext {
    TMCC_JPEGDEC_Stream stream;
    u8 pad18[4];
    s32 dcPred[4];
    u8 pad2C[0x10];
    u16 unk3C;
    u16 unk3E;
    u32 unk40;
    u8 pad44[0x17C2];
    u16 unk1806;
    u8 unk1808;
    u8 pad1809[0x1C7];
    TMCC_JPEGDEC_RstStatus* status;
} TMCC_JPEGDEC_RstContext;

s32 fn_80263180(TMCC_JPEGDEC_RstContext* ctx, u32 x, u32 y) {
    TMCC_JPEGDEC_RstStatus* status = ctx->status;
    u16 marker;
    s32 result = 0;

    ctx->unk3C++;
    if (ctx->unk3C == ctx->unk1806) {
        result = TMCC_JPEGDEC_rewind_ptr(&ctx->stream);
        if (result < 0) {
            return result;
        }

        result = TMCC_JPEGDEC_get_wbyte(&marker, &ctx->stream);
        if (marker == 0xFFD9) {
            ctx->unk1808 = 1;
        }
        if (result < 0) {
            return result;
        }

        if (marker >= 0xFFC0 && (marker < 0xFFD0 || marker > 0xFFD7)) {
            result = TMCC_JPEGDEC_move_ptr(-2, &ctx->stream);
            if (result < 0) {
                return result;
            }
        } else {
            if (marker != ctx->unk3E + 0xFFD0) {
                return -0x23;
            }
        }
        ctx->unk3E = (ctx->unk3E + 1) & 7;

        {
            u16 mcusPerRow = status->mcusPerRow;
            u16 mcuY = y / status->mcuH;
            u16 mcuX = x / status->mcuW;
            u16 next = mcuY * mcusPerRow + mcuX + 1;
            ctx->dcPred[0] = 0;
            ctx->dcPred[1] = 0;
            ctx->dcPred[2] = 0;
            ctx->dcPred[3] = 0;
            ctx->unk3C = 0;
            ctx->unk40 = ((next % mcusPerRow) << 16) + (next / mcusPerRow);
        }

        result = TMCC_JPEGDEC_init_buff(&ctx->stream);
    }
    return result;
}

// HYPOTHESIS: context/status views for TMCC_JPEG_InitDecoder.
typedef struct TMCC_JPEGDEC_InitContext {
    TMCC_JPEGDEC_Stream stream;
    u8 pad18[0x17C4];
    u16 unk17DC;
    u16 unk17DE;
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
    u8 pad1809[0x1C7];
    void* status;
} TMCC_JPEGDEC_InitContext;

typedef struct TMCC_JPEGDEC_InitInfo {
    TMCC_JPEGDEC_InitContext* ctx;
    u8 kind;
    u8 pad5[3];
    u32 limit;
    u8 padC[0x10];
    u8 unk1C;
    u8 unk1D;
} TMCC_JPEGDEC_InitInfo;

typedef struct TMCC_JPEGDEC_InitStatus {
    TMCC_JPEGDEC_InitContext* ctx;
    u8 pad4[4];
    s32 kind;
    u8 padC[4];
    s32 unk10;
    u8 pad14[4];
    s32 unk18;
    u8 pad1C[0x10];
    u8 unk2C;
    u8 unk2D;
    u8 pad2E[0x6CE];
} TMCC_JPEGDEC_InitStatus;

static inline s32 check_init_info(TMCC_JPEGDEC_InitInfo* info, TMCC_JPEGDEC_InitStatus* status) {
    if (info->unk1C != 0) {
        return -1;
    }
    if (info->kind != 4 && info->kind != 6) {
        return -4;
    }
    if (info->unk1D != 1 && info->unk1D != 2 && info->unk1D != 4 && info->unk1D != 8) {
        return -1;
    }
    status->unk2D = info->unk1C;
    status->unk2C = info->unk1D;
    status->kind = info->kind;
    return 0;
}

static inline s32 inline_header_analyze(TMCC_JPEGDEC_InitContext* ctx) {
    s32 result = TMCC_JPEGDEC_imagestart((TMCC_JPEGDEC_Context*)ctx);
    if (result < 0) {
        return result;
    }
    if (ctx->unk17DC == 0) {
        result = -0x50;
    } else {
        ctx->unk17E0 = ctx->unk17E9 * ctx->unk17EC;
        ctx->unk17E4 = ctx->unk17EA * ctx->unk17EE;
    }
    return result;
}

static inline s32 inline_decompscan(TMCC_JPEGDEC_InitContext* ctx) {
    s32 result = TMCC_JPEGDEC_scanstart((TMCC_JPEGDEC_Context*)ctx);
    if (result < 0) {
        return result;
    }
    if (ctx->unk1808 == 1) {
        return 0;
    }
    return result;
}

s32 fn_80262218(TMCC_JPEGDEC_InitStatus* status, u8* data, TMCC_JPEGDEC_InitInfo* info) {
    TMCC_JPEGDEC_InitContext* ctx = info->ctx;
    s32 result;

    if (ctx == 0) {
        return -1;
    }

    memset(status, 0, sizeof(TMCC_JPEGDEC_InitStatus));
    memset(ctx, 0, sizeof(TMCC_JPEGDEC_InitContext));
    ctx->status = status;
    status->ctx = ctx;

    result = check_init_info(info, status);
    if (result < 0) {
        goto err;
    }

    result = TMCC_JPEGDEC_init_stream(&ctx->stream, data, info->limit);
    if (result < 0) {
        return result;
    }

    result = inline_header_analyze(ctx);
    if (result < 0) {
        goto err;
    }

    result = inline_decompscan(ctx);
    if (result < 0) {
        goto err;
    }

    if (ctx->unk1808 != 0) {
        goto err;
    }

    result = fn_8026260C((TMCC_JPEGDEC_ExifCtx*)ctx);
    if (result < 0) {
        goto err;
    }

    switch (status->kind) {
    case 4:
        result = fn_80267980((TMCC_JPEGDEC_ExifCtx*)ctx);
        if (result < 0) {
            goto err;
        }
        break;
    case 5:
        break;
    case 6:
        result = fn_80265EB0((TMCC_JPEGDEC_ExifCtx*)ctx);
        if (result < 0) {
            goto err;
        }
        break;
    default:
        break;
    }

    status->unk10 = TMCC_JPEGDEC_get_position(&ctx->stream);
    return status->unk18;

err:
    return result < 0 ? result : -2;
}

#pragma use_lmw_stmw on

s32 fn_802629B8(s32 x, s32 y, TMCC_JPEGDEC_McuCtx* ctx, void* unused) {
    u8* scanCompPtr;
    s32* dcPredPtr;
    u8** yPtr;
    s8** cPtr;
    u8* blkInfo;
    s32* qt;
    s32 i;
    s32 comp;
    s32 j;
    u16 stride;
    u8* sub = (u8*)ctx + 0x17DC;
    u8* tblBase = (u8*)ctx + 0x18;
    u8* qtable = ctx->qtable;
    u8** yBufs = ctx->yBufs;
    TMCC_JPEGDEC_McuStatus* status = ctx->status;
    void (*idctLumi)(s32*, u8*, s32, s32);
    void (*idctCol)(s32*, s8*, s32, s32);
    s32 (*decodeIquant)(s32*, s32*, s32*, TMCC_JPEGDEC_McuCtx*) = ctx->decodeIquant;
    void (*conv)(TMCC_JPEGDEC_McuCtx*, s32, s32);
    s32 result;
    s32 block[64];
    u8* compInfo;

    idctLumi = ctx->idctLumi;
    idctCol = ctx->idctCol;

    if (x != status->edgeX && y != status->edgeY) {
        conv = ctx->convNormal;
    } else {
        conv = ctx->convEdge;
    }

    stride = ctx->unk19CA;
    scanCompPtr = tblBase;
    dcPredPtr = (s32*)(tblBase + 4);

    for (i = 0; i < sub[0x1B]; i++, scanCompPtr++) {
        comp = *scanCompPtr;
        compInfo = tblBase + comp;
        qt = (s32*)(qtable + (compInfo[0x18] << 8));
        TMCC_JPEGDEC_set_entropytbl((TMCC_JPEGDEC_Context*)qtable, compInfo[0x1C], compInfo[0x20]);
        yPtr = yBufs;
        cPtr = (s8**)((u8*)yBufs + (comp << 2));
        blkInfo = sub + i;
        for (j = 0; j < blkInfo[0x1C]; j++) {
            result = decodeIquant(block, qt, dcPredPtr, ctx);
            if (result < 0) {
                return result;
            }
            if (comp == 0) {
                idctLumi(block, *yPtr, stride, result);
            } else {
                idctCol(block, cPtr[4], stride, result);
            }
            yPtr++;
        }
        dcPredPtr++;
    }

    conv(ctx, x, y);
    if (*(u16*)(sub + 0x2A) != 0) {
        result = fn_80263180((TMCC_JPEGDEC_RstContext*)ctx, x, y);
        if (result < 0) {
            return result;
        }
    }
    return 0;
}

#pragma use_lmw_stmw off

// TODO: clean up, give variables real names
/* jj_80265B24 */
static inline void clamp_store(signed char *p, int v)
{
    int flag = 0;
    int r;
    if (v < 0x80 && v > -0x81) flag = 1;
    if (flag != 0) { r = v; } else { if (v > 0) r = 0x7f; else r = -0x80; }
    *p = r;
}

// TODO: clean up, give variables real names
void fn_80265B24(int *arg0, signed char *arg1)
{
    int tmp[64];
    int *in;
    int *out;
    int i;

    in = arg0 + 24;
    out = tmp + 24;
    for (i = 0; i < 4; i++) {
        int a = in[0];
        int b = in[1];
        int c = in[2];
        int d = in[3];
        int t0 = a + c;
        int t1 = a - c;
        int m = ((b - d) * 0xB5) >> 8;
        int u = m + (b + d);
        out[0] = t0 + u;
        out[1] = t1 + m;
        out[2] = t1 - m;
        out[3] = t0 - u;
        in -= 8;
        out -= 8;
    }

    in = tmp + 3;
    for (i = 3; i >= 0; i--) {
        int a = in[0];
        int c = in[16];
        int b = in[8];
        int d = in[24];
        int t0 = a + c;
        int t1 = a - c;
        int m = ((b - d) * 0xB5) >> 8;
        int u = m + (b + d);
        signed char *o = arg1 + i;

        clamp_store(o, (t0 + u) >> 11);
        clamp_store(o + 8, (t1 + m) >> 11);
        clamp_store(o + 16, (t1 - m) >> 11);
        clamp_store(o + 24, (t0 - u) >> 11);
        in -= 1;
    }
}

s32 fn_802638DC(TMCC_JPEGDEC_Context* ctx);

s32 TMCC_JPEGDEC_imagestart(TMCC_JPEGDEC_Context* ctx) {
    s32 i;
    u16 marker;
    s32 result;

    ctx->unk1806 = 0;
    ctx->unk1808 = 0;
    for (i = 0; i < 64; i++) {
        ctx->zzTbl[i] = lbl_8040DD10[i] << 2;
    }

    result = TMCC_JPEGDEC_get_wbyte(&marker, &ctx->stream);
    if (result < 0) {
        return result;
    }
    if (marker != 0xFFD8) {
        return -0x20;
    }

    marker = 0;
    result = fn_802632F4(&marker, ctx);
    if (result < 0) {
        return result;
    }
    if (marker != 0xFFC0) {
        return -0x10;
    }

    result = fn_802638DC(ctx);
    return result < 0 ? result : 0;
}

void fn_80267874(TMCC_JPEGDEC_Image* image) {
    u16* pixels = image->pixels;
    u32 tilesPerRow = (image->width >> 2) << 1;
    u32 y;
    u32 x;
    u32 tile;
    for (y = 0; y < image->validHeight; y++) {
        for (x = image->validWidth; x < image->width; x++) {
            tile = (y >> 2) * tilesPerRow + ((x >> 2) << 1);
            pixels[(tile << 4) + ((y & 3) << 2) + (x & 3)] = 0xFFFF;
            pixels[((tile + 1) << 4) + ((y & 3) << 2) + (x & 3)] = 0xFFFF;
        }
    }
    for (y = image->validHeight; y < image->height; y++) {
        for (x = 0; x < image->width; x++) {
            tile = (y >> 2) * tilesPerRow + ((x >> 2) << 1);
            pixels[(tile << 4) + ((y & 3) << 2) + (x & 3)] = 0xFFFF;
            pixels[((tile + 1) << 4) + ((y & 3) << 2) + (x & 3)] = 0xFFFF;
        }
    }
}

// HYPOTHESIS: status and context views for YUV-to-texture converters.
typedef struct TMCC_JPEGDEC_ConvStatus {
    u8 pad0[0x22];
    u8 edgeW;
    u8 edgeH;
    u32 edgeX;
    u32 edgeY;
    u8 scale;
    u8 pad2D[0x1B];
    u32 width;
    u32 height;
    u8 pad50[0x14];
    u16* pixels;
} TMCC_JPEGDEC_ConvStatus;

typedef struct TMCC_JPEGDEC_ConvCtx {
    u8 pad0[0x1848];
    u8 buf[0x180];
    u8 pad19C8[8];
    TMCC_JPEGDEC_ConvStatus* status;
} TMCC_JPEGDEC_ConvCtx;

#define CLAMP_U8(v) ((v) > 255 ? 255 : ((v) < 0 ? 0 : (v)))

#define CLAMP_RGB(r, g, b)           \
    if (((r) | (g) | (b)) >> 8) {    \
        (b) = CLAMP_U8(b);           \
        (g) = CLAMP_U8(g);           \
        (r) = CLAMP_U8(r);           \
    }

#define STORE_RGB565(pixels, tilesPerRow, px, py, r, g, b) \
    (pixels)[((((py) >> 2) * (tilesPerRow) + ((px) >> 2)) << 4) + (((py) & 3) << 2) + ((px) & 3)] = \
        (((r) & 0xF8) << 8) + (((g) & 0xFC) << 3) + (((b) & 0xF8) >> 3)

#define STORE_RGBA8(pixels, tilesPerRow, px, py, r, g, b) \
    do { \
        (pixels)[((((py) >> 2) * (tilesPerRow) + (((px) >> 2) << 1)) << 4) + (((py) & 3) << 2) + ((px) & 3)] = 0xFF00 + ((r) & 0xFF); \
        (pixels)[(((((py) >> 2) * (tilesPerRow) + (((px) >> 2) << 1)) + 1) << 4) + (((py) & 3) << 2) + ((px) & 3)] = (((g) & 0xFF) << 8) + ((b) & 0xFF); \
    } while (0)

#pragma use_lmw_stmw on

void fn_80268F44(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s32 size;
    s32 px;
    s32 py;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = status->width >> 2;
    size = 8 / status->scale;
    xEnd = x + size;
    yEnd = y + size;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px++) {
            s32 yv = *yPtr++;
            s32 r;
            s32 g;
            s32 b;
            if (yv >> 8) {
                b = CLAMP_U8(yv);
                g = CLAMP_U8(yv);
                r = CLAMP_U8(yv);
            } else {
                b = yv;
                g = yv;
                r = yv;
            }
            STORE_RGB565(pixels, tilesPerRow, px, py, r, g, b);
        }
        yPtr += 8 - size;
    }
}

void fn_802675E0(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s32 size;
    s32 px;
    s32 py;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = (status->width >> 2) << 1;
    size = 8 / status->scale;
    xEnd = x + size;
    yEnd = y + size;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px++) {
            s32 yv = *yPtr++;
            s32 r;
            s32 g;
            s32 b;
            if (yv >> 8) {
                b = CLAMP_U8(yv);
                g = CLAMP_U8(yv);
                r = CLAMP_U8(yv);
            } else {
                b = yv;
                g = yv;
                r = yv;
            }
            STORE_RGBA8(pixels, tilesPerRow, px, py, r, g, b);
        }
        yPtr += 8 - size;
    }
}

void fn_8026905C(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s32 w;
    s32 h;
    s32 px;
    s32 py;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = status->width >> 2;
    if (status->edgeX == (u32)x) {
        w = status->edgeW;
    } else {
        w = 8 / status->scale;
    }
    if (status->edgeY == (u32)y) {
        h = status->edgeH;
    } else {
        h = 8 / status->scale;
    }
    xEnd = x + w;
    yEnd = y + h;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px++) {
            s32 yv = *yPtr++;
            s32 r;
            s32 g;
            s32 b;
            if (yv >> 8) {
                b = CLAMP_U8(yv);
                g = CLAMP_U8(yv);
                r = CLAMP_U8(yv);
            } else {
                b = yv;
                g = yv;
                r = yv;
            }
            STORE_RGB565(pixels, tilesPerRow, px, py, r, g, b);
        }
        yPtr += 8 - w;
    }
}

void fn_80267710(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s32 w;
    s32 h;
    s32 px;
    s32 py;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = (status->width >> 2) << 1;
    if (status->edgeX == (u32)x) {
        w = status->edgeW;
    } else {
        w = 8 / status->scale;
    }
    if (status->edgeY == (u32)y) {
        h = status->edgeH;
    } else {
        h = 8 / status->scale;
    }
    xEnd = x + w;
    yEnd = y + h;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px++) {
            s32 yv = *yPtr++;
            s32 r;
            s32 g;
            s32 b;
            if (yv >> 8) {
                b = CLAMP_U8(yv);
                g = CLAMP_U8(yv);
                r = CLAMP_U8(yv);
            } else {
                b = yv;
                g = yv;
                r = yv;
            }
            STORE_RGBA8(pixels, tilesPerRow, px, py, r, g, b);
        }
        yPtr += 8 - w;
    }
}

void fn_80268C38(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s8* cbPtr = (s8*)(ctx->buf + 0x40);
    s8* crPtr = (s8*)(ctx->buf + 0x80);
    s32 size;
    s32 px;
    s32 py;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = status->width >> 2;
    size = 8 / status->scale;
    xEnd = x + size;
    yEnd = y + size;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px++) {
            s32 cb = *cbPtr++;
            s32 cr = *crPtr++;
            s32 yv = *yPtr++;
            s32 g = yv + (-(cb * 0x58 + cr * 0xB7) >> 8);
            s32 r = yv + ((cr * 0x167) >> 8);
            s32 b = yv + ((cb * 0x1C6) >> 8);
            CLAMP_RGB(r, g, b);
            STORE_RGB565(pixels, tilesPerRow, px, py, r, g, b);
        }
        yPtr += 8 - size;
        cbPtr += 8 - size;
        crPtr += 8 - size;
    }
}

void fn_802672A4(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s8* cbPtr = (s8*)(ctx->buf + 0x40);
    s8* crPtr = (s8*)(ctx->buf + 0x80);
    s32 size;
    s32 px;
    s32 py;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = (status->width >> 2) << 1;
    size = 8 / status->scale;
    xEnd = x + size;
    yEnd = y + size;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px++) {
            s32 cb = *cbPtr++;
            s32 cr = *crPtr++;
            s32 yv = *yPtr++;
            s32 g = yv + (-(cb * 0x58 + cr * 0xB7) >> 8);
            s32 r = yv + ((cr * 0x167) >> 8);
            s32 b = yv + ((cb * 0x1C6) >> 8);
            CLAMP_RGB(r, g, b);
            STORE_RGBA8(pixels, tilesPerRow, px, py, r, g, b);
        }
        yPtr += 8 - size;
        cbPtr += 8 - size;
        crPtr += 8 - size;
    }
}

void fn_80268DA4(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s8* cbPtr = (s8*)(ctx->buf + 0x40);
    s8* crPtr = (s8*)(ctx->buf + 0x80);
    s32 w;
    s32 h;
    s32 px;
    s32 py;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = status->width >> 2;
    if (status->edgeX == (u32)x) {
        w = status->edgeW;
    } else {
        w = 8 / status->scale;
    }
    if (status->edgeY == (u32)y) {
        h = status->edgeH;
    } else {
        h = 8 / status->scale;
    }
    xEnd = x + w;
    yEnd = y + h;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px++) {
            s32 cb = *cbPtr++;
            s32 cr = *crPtr++;
            s32 yv = *yPtr++;
            s32 g = yv + (-(cb * 0x58 + cr * 0xB7) >> 8);
            s32 r = yv + ((cr * 0x167) >> 8);
            s32 b = yv + ((cb * 0x1C6) >> 8);
            CLAMP_RGB(r, g, b);
            STORE_RGB565(pixels, tilesPerRow, px, py, r, g, b);
        }
        yPtr += 8 - w;
        cbPtr += 8 - w;
        crPtr += 8 - w;
    }
}

void fn_80267428(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s8* cbPtr = (s8*)(ctx->buf + 0x40);
    s8* crPtr = (s8*)(ctx->buf + 0x80);
    s32 w;
    s32 h;
    s32 px;
    s32 py;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = (status->width >> 2) << 1;
    if (status->edgeX == (u32)x) {
        w = status->edgeW;
    } else {
        w = 8 / status->scale;
    }
    if (status->edgeY == (u32)y) {
        h = status->edgeH;
    } else {
        h = 8 / status->scale;
    }
    xEnd = x + w;
    yEnd = y + h;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px++) {
            s32 cb = *cbPtr++;
            s32 cr = *crPtr++;
            s32 yv = *yPtr++;
            s32 g = yv + (-(cb * 0x58 + cr * 0xB7) >> 8);
            s32 r = yv + ((cr * 0x167) >> 8);
            s32 b = yv + ((cb * 0x1C6) >> 8);
            CLAMP_RGB(r, g, b);
            STORE_RGBA8(pixels, tilesPerRow, px, py, r, g, b);
        }
        yPtr += 8 - w;
        cbPtr += 8 - w;
        crPtr += 8 - w;
    }
}

void fn_802688FC(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s8* cbPtr = (s8*)(ctx->buf + 0x80);
    s8* crPtr = (s8*)(ctx->buf + 0xC0);
    s32 px;
    s32 py;
    s32 w;
    s32 h;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = status->width >> 2;
    w = 8 / status->scale;
    h = 16 / status->scale;
    xEnd = x + w;
    yEnd = y + h;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px++) {
            s32 cb = *cbPtr++;
            s32 cr = *crPtr++;
            s32 yv = *yPtr++;
            s32 g = yv + (-(cb * 0x58 + cr * 0xB7) >> 8);
            s32 r = yv + ((cr * 0x167) >> 8);
            s32 b = yv + ((cb * 0x1C6) >> 8);
            CLAMP_RGB(r, g, b);
            STORE_RGB565(pixels, tilesPerRow, px, py, r, g, b);
        }
        yPtr += 8 - w;
        if (py & 1) {
            cbPtr += 8 - w;
            crPtr += 8 - w;
        } else {
            cbPtr -= w;
            crPtr -= w;
        }
    }
}

void fn_80266F38(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s8* cbPtr = (s8*)(ctx->buf + 0x80);
    s8* crPtr = (s8*)(ctx->buf + 0xC0);
    s32 px;
    s32 py;
    s32 w;
    s32 h;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = (status->width >> 2) << 1;
    w = 8 / status->scale;
    h = 16 / status->scale;
    xEnd = x + w;
    yEnd = y + h;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px++) {
            s32 cb = *cbPtr++;
            s32 cr = *crPtr++;
            s32 yv = *yPtr++;
            s32 g = yv + (-(cb * 0x58 + cr * 0xB7) >> 8);
            s32 r = yv + ((cr * 0x167) >> 8);
            s32 b = yv + ((cb * 0x1C6) >> 8);
            CLAMP_RGB(r, g, b);
            STORE_RGBA8(pixels, tilesPerRow, px, py, r, g, b);
        }
        yPtr += 8 - w;
        if (py & 1) {
            cbPtr += 8 - w;
            crPtr += 8 - w;
        } else {
            cbPtr -= w;
            crPtr -= w;
        }
    }
}

void fn_80268A84(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s8* cbPtr = (s8*)(ctx->buf + 0x80);
    s8* crPtr = (s8*)(ctx->buf + 0xC0);
    s32 px;
    s32 py;
    s32 w;
    s32 h;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = status->width >> 2;
    if (status->edgeX == (u32)x) {
        w = status->edgeW;
    } else {
        w = 8 / status->scale;
    }
    if (status->edgeY == (u32)y) {
        h = status->edgeH;
    } else {
        h = 16 / status->scale;
    }
    xEnd = x + w;
    yEnd = y + h;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px++) {
            s32 cb = *cbPtr++;
            s32 cr = *crPtr++;
            s32 yv = *yPtr++;
            s32 g = yv + (-(cb * 0x58 + cr * 0xB7) >> 8);
            s32 r = yv + ((cr * 0x167) >> 8);
            s32 b = yv + ((cb * 0x1C6) >> 8);
            CLAMP_RGB(r, g, b);
            STORE_RGB565(pixels, tilesPerRow, px, py, r, g, b);
        }
        yPtr += 8 - w;
        if (py & 1) {
            cbPtr += 8 - w;
            crPtr += 8 - w;
        } else {
            cbPtr -= w;
            crPtr -= w;
        }
    }
}

void fn_802670D8(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s8* cbPtr = (s8*)(ctx->buf + 0x80);
    s8* crPtr = (s8*)(ctx->buf + 0xC0);
    s32 px;
    s32 py;
    s32 w;
    s32 h;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = (status->width >> 2) << 1;
    if (status->edgeX == (u32)x) {
        w = status->edgeW;
    } else {
        w = 8 / status->scale;
    }
    if (status->edgeY == (u32)y) {
        h = status->edgeH;
    } else {
        h = 16 / status->scale;
    }
    xEnd = x + w;
    yEnd = y + h;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px++) {
            s32 cb = *cbPtr++;
            s32 cr = *crPtr++;
            s32 yv = *yPtr++;
            s32 g = yv + (-(cb * 0x58 + cr * 0xB7) >> 8);
            s32 r = yv + ((cr * 0x167) >> 8);
            s32 b = yv + ((cb * 0x1C6) >> 8);
            CLAMP_RGB(r, g, b);
            STORE_RGBA8(pixels, tilesPerRow, px, py, r, g, b);
        }
        yPtr += 8 - w;
        if (py & 1) {
            cbPtr += 8 - w;
            crPtr += 8 - w;
        } else {
            cbPtr -= w;
            crPtr -= w;
        }
    }
}

void fn_80268354(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s8* cbPtr = (s8*)(ctx->buf + 0x80);
    s8* crPtr = (s8*)(ctx->buf + 0xC0);
    s32 w;
    s32 h;
    s32 px;
    s32 py;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = status->width >> 2;
    s32 dr;
    s32 dg;
    s32 db;
    if (status->edgeX == (u32)x) {
        w = status->edgeW;
    } else {
        w = 16 / status->scale;
    }
    if (status->edgeY == (u32)y) {
        h = status->edgeH;
    } else {
        h = 8 / status->scale;
    }
    xEnd = x + w;
    yEnd = y + h;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px++) {
            s32 yv;
            s32 r;
            s32 g;
            s32 b;
            if ((px & 1) == 0) {
                s8 cr = *crPtr++;
                s8 cb = *cbPtr++;
                dr = (cr * 0x167) >> 8;
                dg = -(cb * 0x58 + cr * 0xB7) >> 8;
                db = (cb * 0x1C6) >> 8;
            }
            yv = *yPtr++;
            r = yv + dr;
            b = yv + db;
            g = yv + dg;
            CLAMP_RGB(r, g, b);
            STORE_RGB565(pixels, tilesPerRow, px, py, r, g, b);
        }
        yPtr += 16 - w;
        cbPtr += (16 - w) >> 1;
        crPtr += (16 - w) >> 1;
    }
}

void fn_80266930(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s8* cbPtr = (s8*)(ctx->buf + 0x80);
    s8* crPtr = (s8*)(ctx->buf + 0xC0);
    s32 w;
    s32 h;
    s32 px;
    s32 py;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = (status->width >> 2) << 1;
    s32 dr;
    s32 dg;
    s32 db;
    if (status->edgeX == (u32)x) {
        w = status->edgeW;
    } else {
        w = 16 / status->scale;
    }
    if (status->edgeY == (u32)y) {
        h = status->edgeH;
    } else {
        h = 8 / status->scale;
    }
    xEnd = x + w;
    yEnd = y + h;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px++) {
            s32 yv;
            s32 r;
            s32 g;
            s32 b;
            if ((px & 1) == 0) {
                s8 cr = *crPtr++;
                s8 cb = *cbPtr++;
                dr = (cr * 0x167) >> 8;
                dg = -(cb * 0x58 + cr * 0xB7) >> 8;
                db = (cb * 0x1C6) >> 8;
            }
            yv = *yPtr++;
            r = yv + dr;
            b = yv + db;
            g = yv + dg;
            CLAMP_RGB(r, g, b);
            STORE_RGBA8(pixels, tilesPerRow, px, py, r, g, b);
        }
        yPtr += 16 - w;
        cbPtr += (16 - w) >> 1;
        crPtr += (16 - w) >> 1;
    }
}

void fn_80268734(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s8* cbPtr = (s8*)(ctx->buf + 0x100);
    s8* crPtr = (s8*)(ctx->buf + 0x140);
    s32 w;
    s32 h;
    s32 px;
    s32 py;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = status->width >> 2;
    s32 dr;
    s32 dg;
    s32 db;
    if (status->edgeX == (u32)x) {
        w = status->edgeW;
    } else {
        w = 16 / status->scale;
    }
    if (status->edgeY == (u32)y) {
        h = status->edgeH;
    } else {
        h = 16 / status->scale;
    }
    xEnd = x + w;
    yEnd = y + h;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px++) {
            s32 yv;
            s32 r;
            s32 g;
            s32 b;
            if ((px & 1) == 0) {
                s8 cr = *crPtr++;
                s8 cb = *cbPtr++;
                dr = (cr * 0x167) >> 8;
                dg = -(cb * 0x58 + cr * 0xB7) >> 8;
                db = (cb * 0x1C6) >> 8;
            }
            yv = *yPtr++;
            r = yv + dr;
            b = yv + db;
            g = yv + dg;
            CLAMP_RGB(r, g, b);
            STORE_RGB565(pixels, tilesPerRow, px, py, r, g, b);
        }
        yPtr += 16 - w;
        if (py & 1) {
            cbPtr += (16 - w) >> 1;
            crPtr += (16 - w) >> 1;
        } else {
            cbPtr -= (w + 1) >> 1;
            crPtr -= (w + 1) >> 1;
        }
    }
}

void fn_80266D58(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s8* cbPtr = (s8*)(ctx->buf + 0x100);
    s8* crPtr = (s8*)(ctx->buf + 0x140);
    s32 w;
    s32 h;
    s32 px;
    s32 py;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = (status->width >> 2) << 1;
    s32 dr;
    s32 dg;
    s32 db;
    if (status->edgeX == (u32)x) {
        w = status->edgeW;
    } else {
        w = 16 / status->scale;
    }
    if (status->edgeY == (u32)y) {
        h = status->edgeH;
    } else {
        h = 16 / status->scale;
    }
    xEnd = x + w;
    yEnd = y + h;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px++) {
            s32 yv;
            s32 r;
            s32 g;
            s32 b;
            if ((px & 1) == 0) {
                s8 cr = *crPtr++;
                s8 cb = *cbPtr++;
                dr = (cr * 0x167) >> 8;
                dg = -(cb * 0x58 + cr * 0xB7) >> 8;
                db = (cb * 0x1C6) >> 8;
            }
            yv = *yPtr++;
            r = yv + dr;
            b = yv + db;
            g = yv + dg;
            CLAMP_RGB(r, g, b);
            STORE_RGBA8(pixels, tilesPerRow, px, py, r, g, b);
        }
        yPtr += 16 - w;
        if (py & 1) {
            cbPtr += (16 - w) >> 1;
            crPtr += (16 - w) >> 1;
        } else {
            cbPtr -= (w + 1) >> 1;
            crPtr -= (w + 1) >> 1;
        }
    }
}

void fn_80267F8C(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s8* cbPtr = (s8*)(ctx->buf + 0x100);
    s8* crPtr = (s8*)(ctx->buf + 0x140);
    s32 w;
    s32 h;
    s32 px;
    s32 py;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = status->width >> 2;
    s32 dr;
    s32 dg;
    s32 db;
    if (status->edgeX == (u32)x) {
        w = status->edgeW;
    } else {
        w = 32 / status->scale;
    }
    if (status->edgeY == (u32)y) {
        h = status->edgeH;
    } else {
        h = 8 / status->scale;
    }
    xEnd = x + w;
    yEnd = y + h;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px++) {
            s32 yv;
            s32 r;
            s32 g;
            s32 b;
            if ((px & 3) == 0) {
                s8 cr = *crPtr++;
                s8 cb = *cbPtr++;
                dr = (cr * 0x167) >> 8;
                dg = -(cb * 0x58 + cr * 0xB7) >> 8;
                db = (cb * 0x1C6) >> 8;
            }
            yv = *yPtr++;
            r = yv + dr;
            b = yv + db;
            g = yv + dg;
            CLAMP_RGB(r, g, b);
            STORE_RGB565(pixels, tilesPerRow, px, py, r, g, b);
        }
        yPtr += 32 - w;
        cbPtr += (32 - w) >> 2;
        crPtr += (32 - w) >> 2;
    }
}

void fn_80266520(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s8* cbPtr = (s8*)(ctx->buf + 0x100);
    s8* crPtr = (s8*)(ctx->buf + 0x140);
    s32 w;
    s32 h;
    s32 px;
    s32 py;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = (status->width >> 2) << 1;
    s32 dr;
    s32 dg;
    s32 db;
    if (status->edgeX == (u32)x) {
        w = status->edgeW;
    } else {
        w = 32 / status->scale;
    }
    if (status->edgeY == (u32)y) {
        h = status->edgeH;
    } else {
        h = 8 / status->scale;
    }
    xEnd = x + w;
    yEnd = y + h;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px++) {
            s32 yv;
            s32 r;
            s32 g;
            s32 b;
            if ((px & 3) == 0) {
                s8 cr = *crPtr++;
                s8 cb = *cbPtr++;
                dr = (cr * 0x167) >> 8;
                dg = -(cb * 0x58 + cr * 0xB7) >> 8;
                db = (cb * 0x1C6) >> 8;
            }
            yv = *yPtr++;
            r = yv + dr;
            b = yv + db;
            g = yv + dg;
            CLAMP_RGB(r, g, b);
            STORE_RGBA8(pixels, tilesPerRow, px, py, r, g, b);
        }
        yPtr += 32 - w;
        cbPtr += (32 - w) >> 2;
        crPtr += (32 - w) >> 2;
    }
}

void fn_80268134(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s8* cbPtr = (s8*)(ctx->buf + 0x80);
    s8* crPtr = (s8*)(ctx->buf + 0xC0);
    s32 w;
    s32 h;
    s32 px;
    s32 py;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = status->width >> 2;
    w = 16 / status->scale;
    h = 8 / status->scale;
    xEnd = x + w;
    yEnd = y + h;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px += 2) {
            s32 cb = *cbPtr++;
            s32 cr = *crPtr++;
            s32 dr = (cr * 0x167) >> 8;
            s32 dg = -(cb * 0x58 + cr * 0xB7) >> 8;
            s32 db = (cb * 0x1C6) >> 8;
            s32 yv = yPtr[0];
            s32 r = yv + dr;
            s32 g = yv + dg;
            s32 b = yv + db;
            CLAMP_RGB(r, g, b);
            STORE_RGB565(pixels, tilesPerRow, px, py, r, g, b);

            yv = yPtr[1];
            yPtr += 2;
            r = yv + dr;
            b = yv + db;
            g = yv + dg;
            CLAMP_RGB(r, g, b);
            STORE_RGB565(pixels, tilesPerRow, px + 1, py, r, g, b);
        }
        yPtr += 16 - w;
        cbPtr += (16 - w) >> 1;
        crPtr += (16 - w) >> 1;
    }
}

void fn_802666E0(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s8* cbPtr = (s8*)(ctx->buf + 0x80);
    s8* crPtr = (s8*)(ctx->buf + 0xC0);
    s32 w;
    s32 h;
    s32 px;
    s32 py;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = (status->width >> 2) << 1;
    w = 16 / status->scale;
    h = 8 / status->scale;
    xEnd = x + w;
    yEnd = y + h;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px += 2) {
            s32 cb = *cbPtr++;
            s32 cr = *crPtr++;
            s32 dr = (cr * 0x167) >> 8;
            s32 dg = -(cb * 0x58 + cr * 0xB7) >> 8;
            s32 db = (cb * 0x1C6) >> 8;
            s32 yv = yPtr[0];
            s32 r = yv + dr;
            s32 g = yv + dg;
            s32 b = yv + db;
            CLAMP_RGB(r, g, b);
            STORE_RGBA8(pixels, tilesPerRow, px, py, r, g, b);

            yv = yPtr[1];
            yPtr += 2;
            r = yv + dr;
            b = yv + db;
            g = yv + dg;
            CLAMP_RGB(r, g, b);
            STORE_RGBA8(pixels, tilesPerRow, px + 1, py, r, g, b);
        }
        yPtr += 16 - w;
        cbPtr += (16 - w) >> 1;
        crPtr += (16 - w) >> 1;
    }
}

void fn_80268500(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s8* cbPtr = (s8*)(ctx->buf + 0x100);
    s8* crPtr = (s8*)(ctx->buf + 0x140);
    s32 size;
    s32 px;
    s32 py;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = status->width >> 2;
    size = 16 / status->scale;
    xEnd = x + size;
    yEnd = y + size;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px += 2) {
            s32 cb = *cbPtr++;
            s32 cr = *crPtr++;
            s32 dr = (cr * 0x167) >> 8;
            s32 dg = -(cb * 0x58 + cr * 0xB7) >> 8;
            s32 db = (cb * 0x1C6) >> 8;
            s32 yv = yPtr[0];
            s32 r = yv + dr;
            s32 g = yv + dg;
            s32 b = yv + db;
            CLAMP_RGB(r, g, b);
            STORE_RGB565(pixels, tilesPerRow, px, py, r, g, b);

            yv = yPtr[1];
            yPtr += 2;
            r = yv + dr;
            b = yv + db;
            g = yv + dg;
            CLAMP_RGB(r, g, b);
            STORE_RGB565(pixels, tilesPerRow, px + 1, py, r, g, b);
        }
        yPtr += 16 - size;
        if (py & 1) {
            cbPtr += (16 - size) >> 1;
            crPtr += (16 - size) >> 1;
        } else {
            cbPtr -= (size + 1) >> 1;
            crPtr -= (size + 1) >> 1;
        }
    }
}

void fn_80266AF4(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s8* cbPtr = (s8*)(ctx->buf + 0x100);
    s8* crPtr = (s8*)(ctx->buf + 0x140);
    s32 size;
    s32 px;
    s32 py;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = (status->width >> 2) << 1;
    size = 16 / status->scale;
    xEnd = x + size;
    yEnd = y + size;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px += 2) {
            s32 cb = *cbPtr++;
            s32 cr = *crPtr++;
            s32 dr = (cr * 0x167) >> 8;
            s32 dg = -(cb * 0x58 + cr * 0xB7) >> 8;
            s32 db = (cb * 0x1C6) >> 8;
            s32 yv = yPtr[0];
            s32 r = yv + dr;
            s32 g = yv + dg;
            s32 b = yv + db;
            CLAMP_RGB(r, g, b);
            STORE_RGBA8(pixels, tilesPerRow, px, py, r, g, b);

            yv = yPtr[1];
            yPtr += 2;
            r = yv + dr;
            b = yv + db;
            g = yv + dg;
            CLAMP_RGB(r, g, b);
            STORE_RGBA8(pixels, tilesPerRow, px + 1, py, r, g, b);
        }
        yPtr += 16 - size;
        if (py & 1) {
            cbPtr += (16 - size) >> 1;
            crPtr += (16 - size) >> 1;
        } else {
            cbPtr -= (size + 1) >> 1;
            crPtr -= (size + 1) >> 1;
        }
    }
}

void fn_80267C28(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s8* cbPtr = (s8*)(ctx->buf + 0x100);
    s8* crPtr = (s8*)(ctx->buf + 0x140);
    s32 w;
    s32 h;
    s32 px;
    s32 py;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = status->width >> 2;
    w = 32 / status->scale;
    h = 8 / status->scale;
    xEnd = x + w;
    yEnd = y + h;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px += 4) {
            s32 cb = *cbPtr++;
            s32 cr = *crPtr++;
            s32 dr = (cr * 0x167) >> 8;
            s32 dg = -(cb * 0x58 + cr * 0xB7) >> 8;
            s32 db = (cb * 0x1C6) >> 8;
            s32 yv;
            s32 r;
            s32 g;
            s32 b;

            yv = yPtr[0];
            r = yv + dr; g = yv + dg; b = yv + db;
            CLAMP_RGB(r, g, b);
            STORE_RGB565(pixels, tilesPerRow, px, py, r, g, b);

            yv = yPtr[1];
            r = yv + dr; b = yv + db; g = yv + dg;
            CLAMP_RGB(r, g, b);
            STORE_RGB565(pixels, tilesPerRow, px + 1, py, r, g, b);

            yv = yPtr[2];
            r = yv + dr; b = yv + db; g = yv + dg;
            CLAMP_RGB(r, g, b);
            STORE_RGB565(pixels, tilesPerRow, px + 2, py, r, g, b);

            yv = yPtr[3];
            yPtr += 4;
            r = yv + dr; b = yv + db; g = yv + dg;
            CLAMP_RGB(r, g, b);
            STORE_RGB565(pixels, tilesPerRow, px + 3, py, r, g, b);
        }
        yPtr += 32 - w;
        cbPtr += (32 - w) >> 2;
        crPtr += (32 - w) >> 2;
    }
}

void fn_80266158(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y) {
    TMCC_JPEGDEC_ConvStatus* status = ctx->status;
    u16* pixels = status->pixels;
    u8* yPtr = ctx->buf;
    s8* cbPtr = (s8*)(ctx->buf + 0x100);
    s8* crPtr = (s8*)(ctx->buf + 0x140);
    s32 w;
    s32 h;
    s32 px;
    s32 py;
    s32 xEnd;
    s32 yEnd;
    u32 tilesPerRow = (status->width >> 2) << 1;
    w = 32 / status->scale;
    h = 8 / status->scale;
    xEnd = x + w;
    yEnd = y + h;

    for (py = y; py < yEnd; py++) {
        for (px = x; px < xEnd; px += 4) {
            s32 cb = *cbPtr++;
            s32 cr = *crPtr++;
            s32 dr = (cr * 0x167) >> 8;
            s32 dg = -(cb * 0x58 + cr * 0xB7) >> 8;
            s32 db = (cb * 0x1C6) >> 8;
            s32 yv;
            s32 r;
            s32 g;
            s32 b;

            yv = yPtr[0];
            r = yv + dr; g = yv + dg; b = yv + db;
            CLAMP_RGB(r, g, b);
            STORE_RGBA8(pixels, tilesPerRow, px, py, r, g, b);

            yv = yPtr[1];
            r = yv + dr; b = yv + db; g = yv + dg;
            CLAMP_RGB(r, g, b);
            STORE_RGBA8(pixels, tilesPerRow, px + 1, py, r, g, b);

            yv = yPtr[2];
            r = yv + dr; b = yv + db; g = yv + dg;
            CLAMP_RGB(r, g, b);
            STORE_RGBA8(pixels, tilesPerRow, px + 2, py, r, g, b);

            yv = yPtr[3];
            yPtr += 4;
            r = yv + dr; b = yv + db; g = yv + dg;
            CLAMP_RGB(r, g, b);
            STORE_RGBA8(pixels, tilesPerRow, px + 3, py, r, g, b);
        }
        yPtr += 32 - w;
        cbPtr += (32 - w) >> 2;
        crPtr += (32 - w) >> 2;
    }
}

s32 fn_80260AA4(u8* bits, u8* vals, TMCC_JPEGDEC_HuffOut* out);

s32 fn_802635B4(void* unused, TMCC_JPEGDEC_Context* ctx) {
    TMCC_JPEGDEC_Tables* tbl = (TMCC_JPEGDEC_Tables*)((u8*)ctx + 0x44);
    u8 byte;
    u16 len;
    TMCC_JPEGDEC_HuffOut out;
    u8 bits[0x11];
    u8 vals[0x100];
    s32 result;
    s32 dcAc;
    s32 id;
    s32 i;
    s32 count;

    memset(bits, 0, sizeof(bits));
    result = TMCC_JPEGDEC_get_wbyte(&len, &ctx->stream);
    if (result < 0) {
        return result;
    }
    len -= 2;

    do {
        len -= 0x11;
        result = TMCC_JPEGDEC_get_byte(&byte, &ctx->stream);
        if (result < 0) {
            return result;
        }
        dcAc = byte >> 4;
        id = byte & 0xF;
        if (dcAc >= 2 || id >= 2) {
            return -0x40;
        }

        bits[0] = 0;
        for (i = 1; i <= 16; i++) {
            result = TMCC_JPEGDEC_get_byte(&byte, &ctx->stream);
            if (result < 0) {
                return result;
            }
            bits[i] = byte;
            len -= byte;
        }

        count = 0;
        for (i = 1; i <= 16; i++) {
            count += bits[i];
        }
        if (count > 0xB0) {
            return -0x40;
        }

        result = TMCC_JPEGDEC_get_sbyte(vals, (u8)count, &ctx->stream);
        if (result < 0) {
            return result;
        }

        out.count = count;
        fn_80260DC4(&out, dcAc, id, tbl);
        result = fn_80260AA4(bits, vals, &out);
        if (result < 0) {
            return result;
        }
    } while (len != 0);

    return 0;
}

#pragma use_lmw_stmw off

static inline void clamp_store_u8(u8* p, s32 v) {
    s32 flag = 0;
    if (v < 0x100 && v > -1) {
        flag = 1;
    }
    *p = flag ? v : (v < 0 ? 0 : 255);
}

void fn_802657A4(s32* arg0, u8* arg1, u16 stride) {
    s32 tmp[64];
    s32* in;
    s32* out;
    s32 i;

    in = arg0 + 24;
    out = tmp + 24;
    for (i = 0; i < 4; i++) {
        s32 u;
        s32 m;
        s32 t0;
        s32 d;
        s32 a;
        s32 b;
        s32 c;
        s32 t1;
        a = in[0];
        b = in[1];
        c = in[2];
        d = in[3];
        t0 = a + c;
        t1 = a - c;
        m = ((b - d) * 0xB5) >> 8;
        u = m + (b + d);
        out[0] = t0 + u;
        out[1] = t1 + m;
        out[2] = t1 - m;
        out[3] = t0 - u;
        in -= 8;
        out -= 8;
    }

    in = tmp + 3;
    for (i = 3; i >= 0; i--) {
        s32 u;
        s32 t0;
        s32 m;
        s32 a = in[0] + 0x40000;
        s32 c = in[16];
        s32 b = in[8];
        s32 d = in[24];
        s32 t1;
        u8* o = arg1 + i;
        t0 = a + c;
        t1 = a - c;
        m = ((b - d) * 0xB5) >> 8;
        u = m + (b + d);

        clamp_store_u8(o, (t0 + u) >> 11);
        clamp_store_u8(o + stride, (t1 + m) >> 11);
        clamp_store_u8(o + stride * 2, (t1 - m) >> 11);
        clamp_store_u8(o + stride * 3, (t0 - u) >> 11);
        in -= 1;
    }
}

s32 fn_80263E18(TMCC_JPEGDEC_Context* ctx) {
    TMCC_JPEGDEC_DecStatus* status = (TMCC_JPEGDEC_DecStatus*)((TMCC_JPEGDEC_ExifCtx*)ctx)->status;
    u8 byte;
    s32 result;
    s32 found;
    u16 mcuX;
    u16 mcuY;

    if (ctx->unk1808 == 1) {
        return 0;
    }
    result = TMCC_JPEGDEC_rewind_ptr(&ctx->stream);
    if (result < 0) {
        return result;
    }
    byte = *ctx->stream.cur;
    found = 0;
    while (!found) {
        while (byte != 0xFF) {
            result = TMCC_JPEGDEC_get_byte(&byte, &ctx->stream);
            if (result < 0) {
                return result;
            }
        }
        result = TMCC_JPEGDEC_get_byte(&byte, &ctx->stream);
        if (byte == 0xD9) {
            if (result < 0 && result != -0x90) {
                return result;
            }
            return 0;
        }
        if (result < 0) {
            return result;
        }
        if (byte >= 0xD0 && byte <= 0xD7) {
            found = 1;
        }
    }

    {
        s32 prev = ctx->unk3E + 0xCF;
        u8 diff;
        u8 skip;
        u16 mcusPerRow;
        u32 cur;
        s32 total;
        if (byte > prev) {
            diff = byte - prev;
        } else {
            diff = byte + 8 - prev;
        }
        skip = diff * ctx->unk1806;
        mcusPerRow = status->mcusPerRow;
        cur = ctx->unk40;
        ctx->unk3E = (byte + 1) & 7;
        ctx->dcPred[0] = 0;
        ctx->dcPred[1] = 0;
        ctx->dcPred[2] = 0;
        ctx->dcPred[3] = 0;
        ctx->unk3C = 0;
        total = (u8)cur * mcusPerRow + ((cur >> 16) + skip);
        mcuY = total / mcusPerRow;
        mcuX = total % mcusPerRow;
        ctx->unk40 = (mcuX << 16) + mcuY;
    }

    result = TMCC_JPEGDEC_init_buff(&ctx->stream);
    if (result < 0) {
        return result;
    }
    status->mcuX = mcuX;
    status->mcuY = mcuY;
    status->unk10 = TMCC_JPEGDEC_get_position(&ctx->stream);
    return status->unk18 - status->mcuY * status->mcusPerRow - status->mcuX;
}

// HYPOTHESIS: status and context views for TMCC_JPEGDEC_Setsize and set_converter*.
typedef struct TMCC_JPEGDEC_SizeStatus {
    u8 pad0[0x18];
    u32 totalMcus;
    u16 mcusPerRow;
    u16 mcusPerCol;
    u8 mcuW;
    u8 mcuH;
    u8 edgeW;
    u8 edgeH;
    u32 edgeX;
    u32 edgeY;
    u8 scale;
    u8 pad2D;
    u8 format;
    u8 pad2F;
    u16 validWidth;
    u16 validHeight;
    u16 cValidWidth;
    u16 cValidHeight;
    u8 pad38[0x10];
    u32 width;
    u32 height;
} TMCC_JPEGDEC_SizeStatus;

typedef struct TMCC_JPEGDEC_BufArea {
    u32 unk0;
    u8 data[0x180];
} TMCC_JPEGDEC_BufArea;

typedef struct TMCC_JPEGDEC_SizeCtx {
    u8 pad0[0x17DC];
    TMCC_JPEGDEC_ScanSub sub;
    u8 pad1808[4];
    void* idctLumi;
    void* idctCol;
    void* decodeIquant;
    void* convNormal;
    void* convEdge;
    u32 unk1820;
    u32 unk1824;
    u8* yBufs[4];
    s8* cBufs[3];
    TMCC_JPEGDEC_BufArea bufArea;
    u8 blockDim;
    u8 pad19C9;
    u16 unk19CA;
    u8 unk19CC;
    u8 pad19CD[3];
    TMCC_JPEGDEC_SizeStatus* status;
} TMCC_JPEGDEC_SizeCtx;

void fn_80261318(void);
void fn_8026171C(void);
s32 fn_80261E34(void);
s32 fn_8026541C();

s32 fn_8026260C(TMCC_JPEGDEC_ExifCtx* rawCtx) {
    TMCC_JPEGDEC_SizeCtx* ctx = (TMCC_JPEGDEC_SizeCtx*)rawCtx;
    TMCC_JPEGDEC_SizeStatus* status = ctx->status;

    switch (status->scale) {
    case 1:
        ctx->unk1820 = 0x40;
        ctx->unk1824 = 0x40;
        ctx->idctLumi = (void*)fn_80261318;
        ctx->idctCol = (void*)fn_8026171C;
        ctx->decodeIquant = (void*)fn_80261E34;
        ctx->blockDim = 8;
        break;
    case 2:
        ctx->unk1820 = 0x19;
        ctx->unk1824 = 0x6C;
        ctx->idctLumi = (void*)fn_802657A4;
        ctx->idctCol = (void*)fn_80265B24;
        ctx->decodeIquant = (void*)fn_8026541C;
        ctx->blockDim = 4;
        break;
    case 4:
        ctx->unk1820 = 5;
        ctx->unk1824 = 0x24;
        ctx->idctLumi = (void*)TMCC_JPEGDEC_IdctBlock2x2;
        ctx->idctCol = (void*)TMCC_JPEGDEC_IdctBlock2x2_Col;
        ctx->decodeIquant = (void*)fn_8026541C;
        ctx->blockDim = 2;
        break;
    case 8:
        ctx->unk1820 = 1;
        ctx->unk1824 = 0;
        ctx->idctLumi = (void*)TMCC_JPEGDEC_IdctBlock1x1;
        ctx->idctCol = (void*)TMCC_JPEGDEC_IdctBlock1x1_Col;
        ctx->decodeIquant = (void*)fn_8026541C;
        ctx->blockDim = 1;
        break;
    default:
        return -0x70;
    }

    status->format = ctx->sub.unkC;
    status->totalMcus = ctx->sub.totalMcus;
    status->mcusPerRow = ctx->sub.mcusPerRow;
    status->mcusPerCol = ctx->sub.mcusPerCol;

    status->mcuW = (status->scale + ctx->sub.mcuW - 1) / status->scale;
    status->mcuH = (status->scale + ctx->sub.mcuH - 1) / status->scale;
    status->validWidth = (ctx->sub.width + (status->scale - 1)) / status->scale;
    status->validHeight = (status->scale + ctx->sub.height - 1) / status->scale;
    status->edgeW = (status->scale + ctx->sub.remX - 1) / status->scale;
    status->edgeH = (status->scale + ctx->sub.remY - 1) / status->scale;

    status->edgeX = (status->edgeW != 0) ? ((status->mcusPerRow - 1) * status->mcuW) : ((status->mcusPerRow + 1) * status->mcuW);
    status->edgeY = (status->edgeH != 0) ? ((status->mcusPerCol - 1) * status->mcuH) : ((status->mcusPerCol + 1) * status->mcuH);

    switch (ctx->sub.unkC) {
    case 4:
        status->cValidWidth = status->validWidth;
        status->cValidHeight = status->validHeight;
        break;
    case 1:
        status->cValidWidth = (status->validWidth + 1) / 2;
        status->cValidHeight = status->validHeight;
        break;
    case 2:
        status->cValidWidth = (status->validWidth + 1) / 2;
        status->cValidHeight = (status->validHeight + 1) / 2;
        break;
    case 0:
        status->cValidWidth = (status->validWidth + 3) / 4;
        status->cValidHeight = status->validHeight;
        break;
    case 3:
        status->cValidWidth = status->validWidth;
        status->cValidHeight = (status->validHeight + 1) / 2;
        break;
    case 5:
        break;
    default:
        return -0x70;
    }

    return 0;
}

void fn_80266158(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y);
void fn_80266520(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y);
void fn_802666E0(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y);
void fn_80266930(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y);
void fn_80266AF4(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y);
void fn_80266D58(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y);
void fn_80266F38(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y);
void fn_802670D8(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y);
void fn_80267428(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y);

void fn_80267C28(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y);
void fn_80267F8C(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y);
void fn_80268134(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y);
void fn_80268354(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y);
void fn_80268500(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y);
void fn_80268734(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y);
void fn_80268A84(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y);
void fn_80268DA4(TMCC_JPEGDEC_ConvCtx* ctx, s32 x, s32 y);

s32 fn_80265EB0(TMCC_JPEGDEC_ExifCtx* rawCtx) {
    TMCC_JPEGDEC_SizeCtx* ctx = (TMCC_JPEGDEC_SizeCtx*)rawCtx;
    TMCC_JPEGDEC_BufArea* buf = &ctx->bufArea;
    TMCC_JPEGDEC_SizeStatus* status = ctx->status;
    u8 dim;

    switch (ctx->sub.unkC) {
    case 0:
        dim = ctx->blockDim;
        ctx->convNormal = (void*)fn_80266158;
        ctx->convEdge = (void*)fn_80266520;
        ctx->yBufs[0] = &buf->data[0];
        ctx->yBufs[1] = ctx->yBufs[0] + dim;
        ctx->yBufs[2] = ctx->yBufs[1] + dim;
        ctx->yBufs[3] = ctx->yBufs[2] + dim;
        ctx->cBufs[1] = (s8*)&buf->data[0x100];
        ctx->cBufs[2] = (s8*)&buf->data[0x140];
        ctx->unk19CA = 0x20;
        ctx->unk19CC = 0;
        break;
    case 1:
        dim = ctx->blockDim;
        ctx->convNormal = (void*)fn_802666E0;
        ctx->convEdge = (void*)fn_80266930;
        ctx->yBufs[0] = &buf->data[0];
        ctx->yBufs[1] = ctx->yBufs[0] + dim;
        ctx->cBufs[1] = (s8*)&buf->data[0x80];
        ctx->cBufs[2] = (s8*)&buf->data[0xC0];
        ctx->unk19CA = 0x10;
        ctx->unk19CC = 0;
        break;
    case 2:
        dim = ctx->blockDim;
        ctx->convNormal = (void*)fn_80266AF4;
        ctx->convEdge = (void*)fn_80266D58;
        ctx->yBufs[0] = &buf->data[0];
        ctx->yBufs[1] = ctx->yBufs[0] + dim;
        ctx->yBufs[2] = ctx->yBufs[0] + (dim << 4);
        ctx->yBufs[3] = ctx->yBufs[2] + dim;
        ctx->cBufs[1] = (s8*)&buf->data[0x100];
        ctx->cBufs[2] = (s8*)&buf->data[0x140];
        ctx->unk19CA = 0x10;
        ctx->unk19CC = 0;
        break;
    case 3:
        dim = ctx->blockDim;
        ctx->convNormal = (void*)fn_80266F38;
        ctx->convEdge = (void*)fn_802670D8;
        ctx->yBufs[0] = &buf->data[0];
        ctx->yBufs[1] = ctx->yBufs[0] + (dim << 3);
        ctx->cBufs[1] = (s8*)&buf->data[0x80];
        ctx->cBufs[2] = (s8*)&buf->data[0xC0];
        ctx->unk19CA = 8;
        ctx->unk19CC = 0;
        break;
    case 4:
        ctx->convNormal = (void*)fn_802672A4;
        ctx->convEdge = (void*)fn_80267428;
        ctx->yBufs[0] = &buf->data[0];
        ctx->cBufs[1] = (s8*)&buf->data[0x40];
        ctx->cBufs[2] = (s8*)&buf->data[0x80];
        ctx->unk19CA = 8;
        ctx->unk19CC = 0;
        break;
    case 5:
        ctx->convNormal = (void*)fn_802675E0;
        ctx->convEdge = (void*)fn_80267710;
        ctx->yBufs[0] = &buf->data[0];
        ctx->unk19CA = 8;
        ctx->unk19CC = 0;
        break;
    default:
        return -0x70;
    }

    status->width = ((status->validWidth >> 2) + ((status->validWidth % 4) != 0)) << 2;
    status->height = ((status->validHeight >> 2) + ((status->validHeight % 4) != 0)) << 2;
    return 0;
}

s32 fn_80267980(TMCC_JPEGDEC_ExifCtx* rawCtx) {
    TMCC_JPEGDEC_SizeCtx* ctx = (TMCC_JPEGDEC_SizeCtx*)rawCtx;
    TMCC_JPEGDEC_BufArea* buf = &ctx->bufArea;
    TMCC_JPEGDEC_SizeStatus* status = ctx->status;
    u8 dim;

    switch (ctx->sub.unkC) {
    case 0:
        dim = ctx->blockDim;
        ctx->convNormal = (void*)fn_80267C28;
        ctx->convEdge = (void*)fn_80267F8C;
        ctx->yBufs[0] = &buf->data[0];
        ctx->yBufs[1] = ctx->yBufs[0] + dim;
        ctx->yBufs[2] = ctx->yBufs[1] + dim;
        ctx->yBufs[3] = ctx->yBufs[2] + dim;
        ctx->cBufs[1] = (s8*)&buf->data[0x100];
        ctx->cBufs[2] = (s8*)&buf->data[0x140];
        ctx->unk19CA = 0x20;
        ctx->unk19CC = 0;
        break;
    case 1:
        dim = ctx->blockDim;
        ctx->convNormal = (void*)fn_80268134;
        ctx->convEdge = (void*)fn_80268354;
        ctx->yBufs[0] = &buf->data[0];
        ctx->yBufs[1] = ctx->yBufs[0] + dim;
        ctx->cBufs[1] = (s8*)&buf->data[0x80];
        ctx->cBufs[2] = (s8*)&buf->data[0xC0];
        ctx->unk19CA = 0x10;
        ctx->unk19CC = 0;
        break;
    case 2:
        dim = ctx->blockDim;
        ctx->convNormal = (void*)fn_80268500;
        ctx->convEdge = (void*)fn_80268734;
        ctx->yBufs[0] = &buf->data[0];
        ctx->yBufs[1] = ctx->yBufs[0] + dim;
        ctx->yBufs[2] = ctx->yBufs[0] + (dim << 4);
        ctx->yBufs[3] = ctx->yBufs[2] + dim;
        ctx->cBufs[1] = (s8*)&buf->data[0x100];
        ctx->cBufs[2] = (s8*)&buf->data[0x140];
        ctx->unk19CA = 0x10;
        ctx->unk19CC = 0;
        break;
    case 3:
        dim = ctx->blockDim;
        ctx->convNormal = (void*)fn_802688FC;
        ctx->convEdge = (void*)fn_80268A84;
        ctx->yBufs[0] = &buf->data[0];
        ctx->yBufs[1] = ctx->yBufs[0] + (dim << 3);
        ctx->cBufs[1] = (s8*)&buf->data[0x80];
        ctx->cBufs[2] = (s8*)&buf->data[0xC0];
        ctx->unk19CA = 8;
        ctx->unk19CC = 0;
        break;
    case 4:
        ctx->convNormal = (void*)fn_80268C38;
        ctx->convEdge = (void*)fn_80268DA4;
        ctx->yBufs[0] = &buf->data[0];
        ctx->cBufs[1] = (s8*)&buf->data[0x40];
        ctx->cBufs[2] = (s8*)&buf->data[0x80];
        ctx->unk19CA = 8;
        ctx->unk19CC = 0;
        break;
    case 5:
        ctx->convNormal = (void*)fn_80268F44;
        ctx->convEdge = (void*)fn_8026905C;
        ctx->yBufs[0] = &buf->data[0];
        ctx->unk19CA = 8;
        ctx->unk19CC = 0;
        break;
    default:
        return -0x70;
    }

    status->width = ((status->validWidth >> 2) + ((status->validWidth % 4) != 0)) << 2;
    status->height = ((status->validHeight >> 2) + ((status->validHeight % 4) != 0)) << 2;
    return 0;
}

extern const u8 lbl_805A3AD0[];
extern const u8 lbl_8040DCE0[6][4];
extern const u8 lbl_8040DCF8[6][4];

#pragma use_lmw_stmw on

static inline s32 check_sof_comps(TMCC_JPEGDEC_Context* ctx) {
    TMCC_JPEGDEC_ScanSub* sub = (TMCC_JPEGDEC_ScanSub*)((u8*)ctx + 0x17DC);
    TMCC_JPEGDEC_SosTable* sos = (TMCC_JPEGDEC_SosTable*)((u8*)ctx + 0x18);
    s32 i;
    for (i = 0; i < sub->numComps; i++) {
        if (sub->compH[i] < 1 || sub->compH[i] > 4) {
            return -0x50;
        }
        if (sub->compV[i] < 1 || sub->compV[i] > 4) {
            return -0x50;
        }
        if (sos->compQIdx[i] > 4) {
            return -0x50;
        }
    }
    return 0;
}

s32 fn_802638DC(TMCC_JPEGDEC_Context* ctx) {
    s32 maxH;
    s32 maxV;
    TMCC_JPEGDEC_ScanSub* sub = (TMCC_JPEGDEC_ScanSub*)((u8*)ctx + 0x17DC);
    TMCC_JPEGDEC_SosTable* sos = (TMCC_JPEGDEC_SosTable*)((u8*)ctx + 0x18);
    u8 byte;
    u16 word;
    s32 result;
    s32 i;
    s32 j;

    result = TMCC_JPEGDEC_get_wbyte(&word, &ctx->stream);
    if (result < 0) {
        return result;
    }
    if (word < 2) {
        return -0x50;
    }

    result = TMCC_JPEGDEC_get_byte(&byte, &ctx->stream);
    if (result < 0) {
        return result;
    }
    if (byte != 8) {
        return -0x50;
    }

    result = TMCC_JPEGDEC_get_wbyte(&word, &ctx->stream);
    if (result < 0) {
        return result;
    }
    sub->height = word;

    result = TMCC_JPEGDEC_get_wbyte(&word, &ctx->stream);
    if (result < 0) {
        return result;
    }
    sub->width = word;

    result = TMCC_JPEGDEC_get_byte(&byte, &ctx->stream);
    if (result < 0) {
        return result;
    }
    {
        s32 n = byte;
        sub->numComps = n;
        if (n <= 0 || n > 4) {
            return -0x50;
        }
    }

    maxV = 0;
    maxH = 0;
    for (i = 0; i < sub->numComps; i++) {
        u8 hv;
        s32 h;
        result = TMCC_JPEGDEC_get_byte(&byte, &ctx->stream);
        if (result < 0) {
            return result;
        }
        sos->compId[i] = byte;

        result = TMCC_JPEGDEC_get_byte(&byte, &ctx->stream);
        if (result < 0) {
            return result;
        }
        hv = byte;
        h = hv >> 4;
        sub->compH[i] = h;
        sub->compV[i] = hv & 0xF;
        if (h > maxH) {
            maxH = h;
        }
        if (sub->compV[i] > maxV) {
            maxV = sub->compV[i];
        }

        result = TMCC_JPEGDEC_get_byte(&byte, &ctx->stream);
        if (result < 0) {
            return result;
        }
        sos->compQIdx[i] = byte;
    }

    if ((u16)maxH == 0 || (u16)maxV == 0) {
        return -0x50;
    }

    sub->maxH = maxH;
    sub->maxV = maxV;
    sub->unkC = 6;

    for (i = 0; i < 6; i++) {
        if (sub->numComps == lbl_805A3AD0[i]) {
            for (j = 0; j < sub->numComps; j++) {
                if (sub->compH[j] != lbl_8040DCE0[i][j]) {
                    goto next_fmt;
                }
                if (sub->compV[j] != lbl_8040DCF8[i][j]) {
                    goto next_fmt;
                }
            }
            sub->unkC = i;
        }
    next_fmt:;
    }

    if (sub->unkC == 6) {
        return -0x70;
    }

    sub->mcuW = maxH << 3;
    sub->mcuH = maxV << 3;
    sub->mcusPerRow = sub->width / sub->mcuW;
    sub->mcusPerCol = sub->height / sub->mcuH;
    sub->remX = sub->width % sub->mcuW;
    sub->remY = sub->height % sub->mcuH;
    sub->mcusPerRow += (sub->remX != 0);
    sub->mcusPerCol += (sub->remY != 0);
    sub->totalMcus = sub->mcusPerRow * sub->mcusPerCol;

    return check_sof_comps(ctx);
}

// HYPOTHESIS: context view for decode_iquant_rc.
typedef struct TMCC_JPEGDEC_LutEntry {
    u16 len;
    u16 val;
} TMCC_JPEGDEC_LutEntry;

typedef struct TMCC_JPEGDEC_DecRcCtx {
    TMCC_JPEGDEC_Stream stream;
    u8 pad18[0x46C];
    TMCC_JPEGDEC_LutEntry* dcTable;
    u8* dcSizes;
    u8* dcCodes;
    u8 pad490[4];
    TMCC_JPEGDEC_LutEntry* acTable;
    u8* acSizes;
    u8* acCodes;
    u8 pad4A0[0x1380];
    s32 maxAcIdx;
    u32 clearBytes;
} TMCC_JPEGDEC_DecRcCtx;

s32 fn_8026541C(s32* block, s32* qt, s32* dcPred, TMCC_JPEGDEC_DecRcCtx* ctx) {
    TMCC_JPEGDEC_LutEntry* acTable;
    s32 ssss;
    s32 idx;
    s32 maxAcIdx;
    u8* acSizes;
    u8* acCodes;
    TMCC_JPEGDEC_LutEntry* dcTable = ctx->dcTable;
    s32 result;
    s32 sym;
    u32 code;
    u32 len;

    if (ctx->stream.unk4 <= 8) {
        result = TMCC_JPEGDEC_load_buff(&ctx->stream);
        if (result < 0) {
            return result;
        }
    }

    code = (*(u32*)&ctx->stream.unk0 >> (ctx->stream.unk4 - 8)) & 0xFF;
    len = dcTable[code].len;
    if (len != 0) {
        sym = dcTable[code].val;
        ctx->stream.unk4 -= len;
    } else {
        sym = TMCC_JPEGDEC_vl_decode_rc(ctx->dcSizes, ctx->dcCodes, &ctx->stream);
        if (sym < 0) {
            return sym;
        }
    }

    if (sym != 0) {
        u32 bits;
        u32 mask;
        u32 half;
        if (ctx->stream.unk4 <= sym) {
            result = TMCC_JPEGDEC_load_buff(&ctx->stream);
            if (result < 0) {
                return result;
            }
        }
        ctx->stream.unk4 -= sym;
        mask = (1u << sym) - 1;
        half = (1u << sym) >> 1;
        bits = (*(u32*)&ctx->stream.unk0 >> ctx->stream.unk4) & mask;
        if (half > bits) {
            bits -= mask;
        }
        *dcPred += (s32)bits;
    }

    block[0] = (*dcPred) * qt[0];
    acTable = ctx->acTable;
    acSizes = ctx->acSizes;
    acCodes = ctx->acCodes;
    maxAcIdx = ctx->maxAcIdx;
    memset(block + 1, 0, ctx->clearBytes);

    idx = 1;
    do {
        if (ctx->stream.unk4 <= 8) {
            result = TMCC_JPEGDEC_load_buff(&ctx->stream);
            if (result < 0) {
                return result;
            }
        }
        code = (*(u32*)&ctx->stream.unk0 >> (ctx->stream.unk4 - 8)) & 0xFF;
        len = acTable[code].len;
        if (len != 0) {
            sym = acTable[code].val;
            ctx->stream.unk4 -= len;
        } else {
            sym = TMCC_JPEGDEC_vl_decode_rc(acSizes, acCodes, &ctx->stream);
            if (sym < 0) {
                return sym;
            }
        }

        ssss = sym & 0xF;
        if (ssss != 0) {
            idx += sym >> 4;
            if (ctx->stream.unk4 <= ssss) {
                result = TMCC_JPEGDEC_load_buff(&ctx->stream);
                if (result < 0) {
                    return result;
                }
            }
            if (idx > maxAcIdx) {
                ctx->stream.unk4 -= ssss;
                if (idx >= 64) {
                    return -0x64;
                }
                idx++;
            } else {
                u32 bits;
                u32 mask = (1u << ssss) - 1;
                u32 half = (1u << ssss) >> 1;
                ctx->stream.unk4 -= ssss;
                bits = (*(u32*)&ctx->stream.unk0 >> ctx->stream.unk4) & mask;
                if (half > bits) {
                    bits -= mask;
                }
                if (idx >= 64) {
                    return -0x64;
                }
                {
                    s32 zz = lbl_8040DD10[idx];
                    idx++;
                    block[zz] = (s32)bits * qt[zz];
                }
            }
        } else {
            if (sym == 0) {
                return 0;
            }
            idx += 0x10;
        }
    } while (idx < 64);

    return 0;
}

s32 fn_80260AA4(u8* bits, u8* vals, TMCC_JPEGDEC_HuffOut* out) {
    s32 huffcode[256];
    s32 huffsize[256];
    TMCC_JPEGDEC_LutEntry* lut = (TMCC_JPEGDEC_LutEntry*)out->table;
    TMCC_JPEGDEC_HuffPair* pairs = (TMCC_JPEGDEC_HuffPair*)out->sizes;
    u8* codes = out->codes;
    u8 count = out->count;
    s32 i;
    s32 j;
    s32 k;
    u16 code;
    u8 si;

    k = 0;
    i = 1;
    j = 1;
    for (;;) {
        while (j <= bits[i]) {
            huffsize[k] = i;
            k++;
            if (k > count) {
                return -0x40;
            }
            j++;
        }
        i++;
        j = 1;
        if (i > 16) {
            break;
        }
    }
    huffsize[k] = 0;

    k = 0;
    code = 0;
    si = huffsize[0];
    while (huffsize[k] != 0) {
        while ((u8)huffsize[k] == si) {
            huffcode[k] = code;
            k++;
            code++;
        }
        if (code >= (1 << si)) {
            return -0x40;
        }
        code <<= 1;
        si++;
    }

    memset(pairs, 0, 0x44);
    for (k = 0; k < count; k++) {
        if ((u32)huffsize[k] <= 16) {
            pairs[huffsize[k]].offset = k;
            pairs[huffsize[k]].maxcode = huffcode[k];
        }
    }

    k = 0;
    for (i = 1; i <= 8; i++) {
        s32 shift = 8 - i;
        s32 rep = 1 << shift;
        for (j = 1; j <= bits[i]; j++) {
            s32 idx = huffcode[k] << shift;
            s32 n;
            for (n = rep; n > 0; n--) {
                lut[idx].len = i;
                lut[idx].val = vals[k];
                idx++;
            }
            k++;
        }
    }

    memcpy(codes, vals, count);
    return 0;
}

#pragma use_lmw_stmw off

// HYPOTHESIS: EXIF tag info structure stored at status + 0x68.
typedef struct TMCC_JPEGDEC_ExifTagInfo {
    u8 pad0[0xC];
    u16 orientation;
    u8 padE[2];
    u32 xResNum;
    u32 xResDen;
    u32 yResNum;
    u32 yResDen;
    u16 resUnit;
    u16 transferFunc[3][256];
    u8 dateTime[20];
    u16 ycbcrPos;
    u32 exifIfdOff;
    u8 exifVersion[4];
    u8 compConfig[4];
    u8 flashpixVersion[4];
    u16 colorSpace;
    u8 pad64A[2];
    u32 pixelXDim;
    u32 pixelYDim;
    u16 thumbComp;
    u8 pad656[2];
    u32 thumbXResNum;
    u32 thumbXResDen;
    u32 thumbYResNum;
    u32 thumbYResDen;
    u16 thumbResUnit;
    u8 pad66A[2];
    u32 thumbOffset;
    u32 thumbLength;
    u8* tiffBase;
    u8* tiffEnd;
} TMCC_JPEGDEC_ExifTagInfo;

static inline u16 exif_read_u16(u8* p, u16 endian) {
    u32 raw = p[0] | (p[1] << 8);
    if (endian == 0x4949) {
        return (u16)raw;
    }
    return (u16)(((raw >> 8) & 0xFF) | ((raw & 0xFF) << 8));
}

static inline u32 exif_read_u32(u8* p, u16 endian) {
    u32 raw = p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
    if (endian == 0x4949) {
        return raw;
    }
    return ((raw & 0xFF) << 24) | ((raw & 0xFF00) << 8) | ((raw >> 8) & 0xFF00) | ((raw >> 24) & 0xFF);
}

void fn_8026481C(TMCC_JPEGDEC_ExifTagInfo* info, u16 endian, u8* entry) {
    u16 tag = exif_read_u16(entry, endian);
    u16 type = exif_read_u16(entry + 2, endian);
    u32 off;
    u8* p;
    u32 i;
    u32 j;

    switch (tag) {
    case 0x103:
    case 0x111:
    case 0x113:
        break;
    case 0x112:
        info->orientation = exif_read_u16(entry + 8, endian);
        break;
    case 0x11A:
        off = exif_read_u32(entry + 8, endian);
        p = info->tiffBase + off;
        if (p <= info->tiffEnd - 4) {
            info->xResNum = exif_read_u32(p, endian);
            p = info->tiffBase + off + 4;
            if (p <= info->tiffEnd - 4) {
                info->xResDen = exif_read_u32(p, endian);
            }
        }
        break;
    case 0x11B:
        off = exif_read_u32(entry + 8, endian);
        p = info->tiffBase + off;
        if (p <= info->tiffEnd - 4) {
            info->yResNum = exif_read_u32(p, endian);
            p = info->tiffBase + off + 4;
            if (p <= info->tiffEnd - 4) {
                info->yResDen = exif_read_u32(p, endian);
            }
        }
        break;
    case 0x128:
        info->resUnit = exif_read_u16(entry + 8, endian);
        break;
    case 0x12D:
        off = exif_read_u32(entry + 8, endian);
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 256; j++) {
                p = info->tiffBase + off;
                if (p > info->tiffEnd - 2) {
                    break;
                }
                info->transferFunc[i][j] = exif_read_u16(p, endian);
                off += 2;
            }
        }
        break;
    case 0x132:
        off = exif_read_u32(entry + 8, endian);
        p = info->tiffBase + off;
        if (p <= info->tiffEnd - 20) {
            for (i = 0; i < 20; i++) {
                info->dateTime[i] = p[i];
            }
        }
        break;
    case 0x201:
    case 0x202:
        break;
    case 0x213:
        info->ycbcrPos = exif_read_u16(entry + 8, endian);
        break;
    case 0x8769:
        info->exifIfdOff = exif_read_u32(entry + 8, endian);
        break;
    case 0x9000:
        for (i = 0; i < 4; i++) {
            info->exifVersion[i] = entry[8 + i];
        }
        break;
    case 0x9101:
        for (i = 0; i < 4; i++) {
            info->compConfig[i] = entry[8 + i];
        }
        break;
    case 0xA000:
        for (i = 0; i < 4; i++) {
            info->flashpixVersion[i] = entry[8 + i];
        }
        break;
    case 0xA001:
        info->colorSpace = exif_read_u16(entry + 8, endian);
        break;
    case 0xA002:
        if (type == 3) {
            info->pixelXDim = exif_read_u16(entry + 8, endian);
        } else if (type == 4) {
            info->pixelXDim = exif_read_u32(entry + 8, endian);
        }
        break;
    case 0xA003:
        if (type == 3) {
            info->pixelYDim = exif_read_u16(entry + 8, endian);
        } else if (type == 4) {
            info->pixelYDim = exif_read_u32(entry + 8, endian);
        }
        break;
    case 0xA004:
        break;
    }
}

void fn_80264F68(TMCC_JPEGDEC_ExifTagInfo* info, u16 endian, u8* entry) {
    u16 tag = exif_read_u16(entry, endian);
    u32 off;
    u8* p;

    switch (tag) {
    case 0x103:
        info->thumbComp = exif_read_u16(entry + 8, endian);
        break;
    case 0x111:
    case 0x11C:
    case 0x12D:
    case 0x132:
    case 0x213:
    case 0x8769:
    case 0x9000:
    case 0x9101:
    case 0xA004:
        break;
    case 0x11A:
        off = exif_read_u32(entry + 8, endian);
        p = info->tiffBase + off;
        if (p <= info->tiffEnd - 4) {
            info->thumbXResNum = exif_read_u32(p, endian);
            p = info->tiffBase + off + 4;
            if (p <= info->tiffEnd - 4) {
                info->thumbXResDen = exif_read_u32(p, endian);
            }
        }
        break;
    case 0x11B:
        off = exif_read_u32(entry + 8, endian);
        p = info->tiffBase + off;
        if (p <= info->tiffEnd - 4) {
            info->thumbYResNum = exif_read_u32(p, endian);
            p = info->tiffBase + off + 4;
            if (p <= info->tiffEnd - 4) {
                info->thumbYResDen = exif_read_u32(p, endian);
            }
        }
        break;
    case 0x128:
        info->thumbResUnit = exif_read_u16(entry + 8, endian);
        break;
    case 0x201:
        info->thumbOffset = exif_read_u32(entry + 8, endian);
        break;
    case 0x202:
        info->thumbLength = exif_read_u32(entry + 8, endian);
        break;
    }
}

#pragma use_lmw_stmw on

s32 fn_802644D4(u8* data, u16 size, void* infoPtr) {
    TMCC_JPEGDEC_ExifTagInfo* info = (TMCC_JPEGDEC_ExifTagInfo*)infoPtr;
    u16 endian;
    u16 magic;
    u32 offset;
    u8* p;
    u16 rem;
    u16 count;
    u16 i;

    info->tiffBase = data;
    info->tiffEnd = data + size;
    info->exifIfdOff = 0;

    if (size < 8) {
        return -0xA1;
    }

    endian = data[0] | (data[1] << 8);
    if (endian != 0x4D4D && endian != 0x4949) {
        return -0xA1;
    }

    magic = exif_read_u16(data + 2, endian);
    if (magic != 0x2A) {
        return -0xA1;
    }

    offset = exif_read_u32(data + 4, endian);
    if (size < offset) {
        return -0xA1;
    }

    p = data + offset;
    rem = size - (u16)offset;
    if (rem < 2) {
        return -0xA1;
    }

    count = exif_read_u16(p, endian);
    p += 2;
    rem -= 2;
    if (rem < count * 12) {
        return -0xA1;
    }

    for (i = 0; i < count; i++) {
        fn_8026481C(info, endian, p);
        p += 12;
    }

    if (rem < 4) {
        return -0xA1;
    }

    offset = exif_read_u32(p, endian);
    if (offset == 0) {
        return 0;
    }

    if (size < offset) {
        return -0xA1;
    }

    p = data + offset;
    rem = size - (u16)offset;
    if (rem < 2) {
        return -0xA1;
    }

    count = exif_read_u16(p, endian);
    p += 2;
    rem -= 2;
    if (rem < count * 12) {
        return -0xA1;
    }

    for (i = 0; i < count; i++) {
        fn_80264F68(info, endian, p);
        p += 12;
    }

    offset = info->exifIfdOff;
    if (size < offset) {
        return -0xA1;
    }

    p = data + offset;
    rem = size - (u16)offset;
    if (rem < 2) {
        return -0xA1;
    }

    count = exif_read_u16(p, endian);
    p += 2;
    rem -= 2;
    if (rem < count * 12) {
        return -0xA1;
    }

    for (i = 0; i < count; i++) {
        fn_8026481C(info, endian, p);
        p += 12;
    }

    return 0;
}

#pragma use_lmw_stmw off

