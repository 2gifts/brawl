// Local BrawlHeaders shadow: adds pad status and direction queries.
#pragma once

#include <StaticAssert.h>
#include <types.h>

// Local shadow of the header of the shared headers (this directory comes first in the include paths): the constructor
// and the destructor of the controller of a menu are inline, so each module that has an array of controllers carries
// its own copy of them.
// main.dol functions that have no names yet: muMenuController::init(pad) and muMenuController::main()
struct gfPadStatus;
class muMenuController;
extern "C" void fn_800B96E8(muMenuController* controller, int pad);
extern "C" void fn_800B9984(muMenuController* controller);

class muMenuController {
public:
    void init(int pad);
    static int exchangeButton2Dir8(u32 buttons);
    static int getButtonPushedControllerID(u32 buttons);
    union {
        char _unk0[0x8];
        struct {
            u8 unk0[4];
            u8 unk4; // HYPOTHESIS: mask of the pads selected by this controller.
            u8 unk5[3];
        };
    };
    u32 m_held;         // 0x08 HYPOTHESIS: the buttons that are held (the name entry copies it to m_buttonsCurrentFrame2)
    u32 m_pressed;      // 0x0C HYPOTHESIS: the buttons that were just pressed (1 left, 2 right in the Yes/No windows)
    u32 m_trigger;      // 0x10 HYPOTHESIS: the buttons pressed in this frame, in the bits of gfPadButtons
    char _unk14[0x94];

    muMenuController() { fn_800B96E8(this, 0xFF); }
    ~muMenuController() { }

    // The controller whose A button was pressed last (-1 for none).
    static int getAButtonPushedControllerID();
};
static_assert(sizeof(muMenuController) == 0xA8, "Wrong size for class!");
