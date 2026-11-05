// WiiConnect24 SDK: UserId.
#include "revolution.h"
#include "revolution/nwc24.h"
#include "revolution/nwc24/NWC24Internal.h"
#include "revolution/sc.h"
#include "revolution/vf.h"
#include <cstring>
#include <cstdarg>

// BYTES: preserve calls across SDK source boundaries; original inline helpers
// are marked inline explicitly.
#pragma auto_inline off

static u64 getUnScrambleId(u64 id);

// the user id has a checksum in its low bits: after the decode step the whole thing has to be a multiple
// of the generator polynomial 0x635
NWC24Err NWC24iCheckUserIdCrc(NWC24UserId id) {
    u64 v = getUnScrambleId(id);
    int i;

    for (i = 0; i < 43; i++) {
        if ((v >> (53 - (i + 1))) & 1) {
            v ^= (u64)0x635 << (42 - i);
        }
    }

    if (v != 0) {
        return NWC24_ERR_ID_CRC;
    }

    return NWC24_OK;
}

static const u8 IdInverseNibble[16] = {13, 5, 9, 7, 0, 15, 10, 2, 12, 3, 14, 1, 8, 6, 11, 4};
static const u8 IdByteOrder[8] = {1, 5, 0, 4, 2, 3, 6, 7};

static inline u8 GetIdByte(u64 value, u8 index) {
    return (value >> (index * 8)) & 0xFF;
}

static inline u64 SetIdByte(u64 value, u8 index, u8 set) {
    return (value & ~(0xFFULL << ((u64)8 * index))) | ((u64)set << ((u64)8 * index));
}

// Decode the 53-bit ID before checking its CRC. The two six-byte loops
// preserve the upper five bits while undoing the permutation and substitution.
static u64 getUnScrambleId(u64 v) {
    u64 input;
    u8 i;

    // Remove the key, then rotate the bounded 53-bit value right once.
    v &= 0x1FFFFFFFFFFFFFULL;
    v ^= 0x5E5E5E5E5E5EULL;
    v &= 0x1FFFFFFFFFFFFFULL;
    v |= (((v & 0xFF) << 5) & 0x20) << 48;
    v >>= 1;

    // Every permutation source comes from the same snapshot.
    input = v;
    for (i = 0; i < 6; ++i) {
        v = SetIdByte(v, i, GetIdByte(input, IdByteOrder[i]));
    }

    for (i = 0; i < 6; ++i) {
        v = SetIdByte(v, i, (IdInverseNibble[(GetIdByte(v, i) >> 4) & 0xF] << 4) | IdInverseNibble[GetIdByte(v, i) & 0xF]);
    }

    // Undo the final 53-bit rotation and key.
    v = ((v & 0x7FFFFFFFFFFULL) << 10) | ((v >> 43) & 0x3FF);
    v ^= 0xB3B3B3B3B3B3ULL;
    return v;
}
