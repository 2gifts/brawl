#pragma once

#include <StaticAssert.h>

// Local shadow of the header of the shared headers (this directory comes first in the include paths): the constructor
// and the destructor of the controller of a menu are inline, so each module that has an array of controllers carries
// its own copy of them.
// main.dol functions that have no names yet: muMenuController::init(pad) and muMenuController::main()
class muMenuController;
extern "C" void fn_800B96E8(muMenuController* controller, int pad);
extern "C" void fn_800B9984(muMenuController* controller);

class muMenuController {
public:
    char _unk[0xA8];

    muMenuController() { fn_800B96E8(this, 0xFF); }
    ~muMenuController() { }
};
static_assert(sizeof(muMenuController) == 0xA8, "Wrong size for class!");
