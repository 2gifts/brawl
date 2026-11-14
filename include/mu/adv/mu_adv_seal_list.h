#pragma once

#include <StaticAssert.h>
#include <types.h>

// A scrolling grid cursor of the sticker menu (m_cols columns, m_dispRows rows visible).
class muAdvSealList {
public:
    struct IVec2 {
        int x;
        int y;
    };
    struct FVec2 {
        float x;
        float y;
    };

    u8 unk0;
    union {
        // BYTES: some writers mask the whole byte.
        u8 m_flags;
        struct {
        u8 unk1_80 : 1;
        u8 m_noWrap : 1;        // 0x01 & 0x40 HYPOTHESIS: set when init's last argument is false
        u8 m_isBarMode2 : 1;    // 0x01 & 0x20
        u8 m_isBarMode : 1;     // 0x01 & 0x10
        u8 m_isChanged : 1;     // 0x01 & 0x08
        u8 m_isUpdated : 1;     // 0x01 & 0x04
        u8 m_isCursorMove : 1;  // 0x01 & 0x02
        u8 m_isScrollMove : 1;  // 0x01 & 0x01
        };
    };
    u8 unk2;                // input repeat counters
    u8 unk3;
    u8 unk4;
    char _5[3];
    int m_max;              // 0x08
    int m_cols;             // 0x0C
    int m_rows;             // 0x10
    int m_dispCols;         // 0x14
    int m_dispRows;         // 0x18
    IVec2 m_scroll;         // 0x1C
    FVec2 m_scrollPos;      // 0x24
    FVec2 m_scrollSpeed;    // 0x2C
    IVec2 m_cur;            // 0x34
    IVec2 m_dispCur;        // 0x3C
    FVec2 m_dispCurPos;     // 0x44
    FVec2 m_dispCurSpeed;   // 0x4C

    inline void reset() {
        m_scrollPos.y = m_scroll.y;
        m_scrollSpeed.y = 0.0f;
        m_dispCur.y = m_cur.y - m_scroll.y;
        m_dispCurPos.y = m_dispCur.y;
        m_dispCurSpeed.y = 0.0f;
        m_flags &= 0xFC;
        m_isChanged = true;
        m_scrollPos.x = m_scroll.x;
        m_scrollSpeed.x = 0.0f;
        m_dispCur.x = m_cur.x - m_scroll.x;
        m_dispCurPos.x = m_dispCur.x;
        m_dispCurSpeed.x = 0.0f;
    }

    void init(int max, int dispRows, int cols, bool wrap);
    void setMax(int max);
    void flush();
    void setCurIdx(int index);
    u32 setBarMode(bool barMode, bool barMode2);
    void update();
};
static_assert(sizeof(muAdvSealList) == 0x54, "Class is wrong size!");
