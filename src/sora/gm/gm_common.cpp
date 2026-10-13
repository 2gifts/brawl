#include <types.h>
#include <gm/gm_global.h>
#include <gm/gm_lib.h>

extern "C" s32 fn_80056780(s32 id);

bool gmCheckExistFigure(u16 id) {
    s32 index = fn_80056780(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    // HYPOTHESIS: unk842 is a u16 table in gmGlobalRecord indexed by figure index; bit 15 is the existence flag.
    u16 flags = *reinterpret_cast<u16*>(record + 0x842 + index * 2);
    return (flags >> 15) & 1;
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_804977E0[0x10] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_804977F0[0x10] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A00E8[0x1] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A00EC[0x4] __attribute__((aligned(4))) = { 0 };
