#pragma once
// Local BrawlHeaders shadow: adds the sticker menu task's pad handling and its window classes.

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_task.h>
#include <types.h>

class gfPadStatus;

struct muAdvSealTaskParam {
    u32 unk0;
    u32 unk4;
};
static_assert(sizeof(muAdvSealTaskParam) == 0x8, "Size is the wrong size!");

struct muAdvSealTask : public gfTask {
    u32 unk40;
    u8 m_padKind; // 0x44 HYPOTHESIS: 1 when the menu is driven by the Wii Remote held sideways

    static void getResourcePackData(const char** ppath, u32* psize);
    static muAdvSealTask* create(const muAdvSealTaskParam* param, gfArchive* arc);
    void set_pad_type(const gfPadStatus* pad);
};
// TODO size

// The sticker list window (owned sticker kinds and counts).
class muAdvSealWinList {
public:
    struct Item {
        const u32* m_data;  // 0x0 HYPOTHESIS: the sticker record (its id first)
        s16 m_count;        // 0x4
    };

    Item m_items[0x500];    // 0x0
    int m_numItems;         // 0x2800
    char _2804[0x3C09 - 0x2804];
    u8 m_flags;             // 0x3C09 0x8 | 0x4: the list must be redrawn
    char _3C0A[0x3CB0 - 0x3C0A];
    int m_sortKind;         // 0x3CB0
    int m_sortOrder;        // 0x3CB4

    void list_sort(int kind, int order);
    void list_sort_kind(int kind, int order);
    bool add_seal_num(u32 id, int num);
};
