#include <types.h>

// HYPOTHESIS: unnamed zero-initialized FAT driver buffers; sizes from the
// original .bss layout. Names are the address labels until their users are
// identified.
u8 lbl_8053A6E0[0xB68] = { 0 };
u8 lbl_8053B248[0x27FB8] = { 0 };
u8 lbl_80563200[0x18] = { 0 };
u8 lbl_80563218[0x68] = { 0 };
u8 lbl_80563280[0xD0] = { 0 };
u8 lbl_80563350[0x10] = { 0 };
u8 lbl_80563360[0x230] = { 0 };
u8 lbl_80563590[0x208] = { 0 };

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A1200[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1208[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A120C[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1210[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1214[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1218[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A121C[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1220[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1224[0x4] __attribute__((aligned(4))) = { 0 };
