#include "revolution.h"

extern s32 NCDiGetEnabledConfigList(u32 *a, u32 *b, u32 *c);
extern s32 fn_8038F2B0(s32 err, s32 config);

// nibble-at-a-time tables: the low and high half of every byte go through the same 16 entry lookup
static const u16 crc16Table[16] = {
    0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
    0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400,
};

static const u32 crc32Table[16] = {
    0x00000000, 0x1DB71064, 0x3B6E20C8, 0x26D930AC, 0x76DC4190, 0x6B6B51F4, 0x4DB26158, 0x5005713C,
    0xEDB88320, 0xF00F9344, 0xD6D6A3E8, 0xCB61B38C, 0x9B64C2B0, 0x86D3D2D4, 0xA00AE278, 0xBDBDF21C,
};

u16 NETCalcCRC16(const u8 *data, u32 len) {
    u32 b;
    u32 crc = 0;

    for (; len != 0; len--) {
        b = *data;
        data++;
        crc = (crc >> 4) ^ crc16Table[(crc ^ b) & 0xF];
        crc = (crc >> 4) ^ crc16Table[(crc ^ (b >> 4)) & 0xF];
    }

    return crc;
}

u32 NETCalcCRC32(const u8 *data, u32 len) {
    u32 b;
    u32 crc = 0xFFFFFFFF;

    for (; len != 0; len--) {
        b = *data;
        data++;
        crc = (crc >> 4) ^ crc32Table[(crc ^ b) & 0xF];
        crc = (crc >> 4) ^ crc32Table[(crc ^ (b >> 4)) & 0xF];
    }

    return ~crc;
}

static inline s32 FirstSetBit_i(u32 word) {
    s32 i = 0;
    u32 mask = 1;

    for (; i < 32; i++) {
        if (word & mask) {
            return i;
        }

        mask <<= 1;
    }

    return -1;
}

// HYPOTHESIS: the enabled config list is three 32 bit masks, one per kind of connection, and only
// one of the masks may have bits set. this turns that into a single "config number" (20 + n for
// the first mask, 30 + n for the second, 40 + n for the third), 99 if it makes no sense.
#pragma push
#pragma auto_inline off
s32 fn_8038EFB8(u32 a, u32 b, u32 c) {
    s32 result = 99;

    if (a != 0) {
        if (b == 0 && c == 0) {
            result = FirstSetBit_i(a) + 20;
        }
    } else if (b != 0) {
        if (c == 0) {
            result = FirstSetBit_i(b) + 30;
        }
    } else if (c != 0) {
        result = FirstSetBit_i(c) + 40;
    }

    return result;
}
#pragma pop

s32 NETGetStartupErrorCode(s32 err) {
    u32 list[3];
    s32 code = 99;

    if (NCDiGetEnabledConfigList(&list[0], &list[1], &list[2]) >= 0) {
        code = fn_8038EFB8(list[0], list[1], list[2]);
    }

    if (code < 0) {
        err = 0x80000000;
        code = 99;
    }

    return fn_8038F2B0(err, code) - code;
}

// turns an SOStartup error into a net error number. the numbers are -50000 something, the config
// number from above decides between a couple of variants
s32 fn_8038F2B0(s32 err, s32 config) {
    if (err >= 0) {
        return 0;
    }

    switch (err) {
    case -0x2D:
        return -0xC418;
    case -0x1C:
        return -0xC47C;
    case -0x3E:
        return -0xC4E0;
    case -0x6F:
        return -0xCDDC;
    case -0x79:
        if (config >= 20 && config < 30) {
            return -0xC8C8;
        }
        return -0xC738;
    case -0x70:
    case -0x4C:
    case -0x30:
    case -0x27:
        if (config >= 20 && config < 30) {
            return -0xC8C8;
        }
        return -0xC864;
    case -0x66:
    case -0x65:
    case -0x64:
        return -0xCB20;
    case 0x80000000:
        return -0xC3B4;
    default:
        OSReport("Unknown SOStartup Error: %d\n", err);
        return -0xC3B4;
    }
}

static const char *netVersion = "<< REX-PPC 2.0.4.0 (RevoEX-2.0PR4) REL 070628174553 >>";

const char *fn_8038F428(void) {
    return netVersion;
}
