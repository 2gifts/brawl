// Local shadow of BrawlHeaders' gm/gm_stage_edit_data.h.
// The stage browser reads one word at 0x24A0; the rest of the record stays opaque.
// Size stays 0x24e4 so other units that only allocate the record are unchanged.
#pragma once

#include <StaticAssert.h>
#include <types.h>

struct gmStageEditData {
    char _head[0x24A0];
    int m_cursor; // 0x24A0 HYPOTHESIS: the collection cursor the stage browser restores
    char _tail[0x24e4 - 0x24A4];
};
static_assert(sizeof(gmStageEditData) == 0x24e4, "Class is wrong size!");
static_assert(offsetof(gmStageEditData, m_cursor) == 0x24A0, "cursor offset");
