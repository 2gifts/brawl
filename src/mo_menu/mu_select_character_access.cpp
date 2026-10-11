#include <types.h>

// One entry of the table of the characters of the character select screen: the kind of the character and a flag.
// HYPOTHESIS: the meaning of the flag and the names; every entry of the table has the flag set.
struct MuSelCharTblEntry {
    u8 m_charKind;
    u8 m_flag;
};

// The characters of the select screen in the order that they are listed (the numbers are the kinds of the characters of
// the menu).
static MuSelCharTblEntry sTable[] = {
    {0x00, 1}, {0x01, 1}, {0x02, 1}, {0x03, 1}, {0x06, 1}, {0x07, 1}, {0x08, 1}, {0x11, 1}, {0x12, 1},
    {0x09, 1}, {0x1A, 1}, {0x0E, 1}, {0x17, 1}, {0x16, 1}, {0x13, 1}, {0x1B, 1}, {0x21, 1}, {0x26, 1},
    {0x0D, 1}, {0x05, 1}, {0x14, 1}, {0x10, 1}, {0x1F, 1}, {0x25, 1}, {0x20, 1}, {0x0B, 1}, {0x27, 1},
    {0x0C, 1}, {0x15, 1}, {0x24, 1}, {0x22, 1}, {0x18, 1}, {0x0A, 1}, {0x23, 1}, {0x19, 1},
};

// The number of characters of the table.
extern "C" int MuSelCharTblAcces_GetTableLen() {
    return 35;
}

// The kind of the character at an index of the table; an index outside of the table gives 0x28 (no character).
extern "C" int MuSelCharTblAcces_GetCharKind(int index) {
    if (index < 0 || index >= 35) {
        return 0x28;
    }
    return sTable[index].m_charKind;
}
