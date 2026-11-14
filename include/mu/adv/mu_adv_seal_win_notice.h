#pragma once

#include <StaticAssert.h>
#include <types.h>

class MuMsg;

// The yes/no notice window of the sticker menu (an effect model with up to three lines of text).
class muAdvSealWinNotice {
public:
    int m_state;            // 0x0 0 closed, 1 open, 2 closing
    u32 m_timer;            // 0x4
    int m_select;           // 0x8 1 yes, -1 no
    u32 m_effect;           // 0xC the ecMgr handle of the window model
    MuMsg* m_msg;           // 0x10
    u32 m_msgBase;          // 0x14 the first of the four message slots of the window
    int m_lines[3];         // 0x18 the message index of each line (-1 none); a second line makes it a yes/no window
    u32 m_cancelMask;       // 0x24 the buttons that close the window

    void init(int priority, MuMsg* msg, u32 msgBase);
    virtual void start(int line0, int line1, int line2, int select, u32 cancelMask);
    virtual void end();
    void update();
    int input(u32 held, u32 trigger);
};
static_assert(sizeof(muAdvSealWinNotice) == 0x2C, "Class is wrong size!");

class muAdvSealWinNoticeSeal : public muAdvSealWinNotice {
public:
    virtual void start(int line0, int line1, int line2, int select, u32 cancelMask);
    virtual void end();
};
