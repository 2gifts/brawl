#include "revolution.h"

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
    u32 crc = 0;

    for (; len != 0; len--) {
        u8 b = *data++;
        crc = (crc >> 4) ^ crc16Table[(crc ^ b) & 0xF];
        crc = (crc >> 4) ^ crc16Table[(crc ^ (b >> 4)) & 0xF];
    }

    return crc;
}

u32 NETCalcCRC32(const u8 *data, u32 len) {
    u32 crc = 0xFFFFFFFF;

    for (; len != 0; len--) {
        u8 b = *data++;
        crc = (crc >> 4) ^ crc32Table[(crc ^ b) & 0xF];
        crc = (crc >> 4) ^ crc32Table[(crc ^ (b >> 4)) & 0xF];
    }

    return ~crc;
}
