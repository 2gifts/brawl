#pragma once

#include <StaticAssert.h>
#include <types.h>

// A cache of 65 sticker textures: entries are reference counted and the unused ones are kept in a
// least-recently-used list, so a texture is only reloaded once its slot is recycled.
class muAdvSealTexMgr {
public:
    struct Entry {
        Entry* m_prev;      // 0x0
        Entry* m_next;      // 0x4
        u16 m_index;        // 0x8
        u16 m_refCount;     // 0xA
        const s32* m_key;   // 0xC HYPOTHESIS: points at the sticker id
        void* m_tex;        // 0x10

        Entry() : m_tex(NULL) { }
    };

    Entry m_entries[65];    // 0x0
    u32 unk514;             // 0x514
    Entry* m_loading;       // 0x518 HYPOTHESIS: the entry update() is loading
    Entry* m_head;          // 0x51C the free list (oldest first)
    Entry* m_tail;          // 0x520

    muAdvSealTexMgr();
    void update();
    int regist(const s32* key);
    void un_regist(int* handle);
};
static_assert(sizeof(muAdvSealTexMgr) == 0x524, "Class is wrong size!");
