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
