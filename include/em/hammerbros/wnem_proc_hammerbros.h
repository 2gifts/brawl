#pragma once

#include <so/so_module_accesser.h>
#include <wn/wn_em_simple.h>

// The proc functions of the Hammer Bro's wnemSimple weapons (the thrown hammers), registered with the enemy weapon
// manager by the Enemy unit (HammerFly is proc 0x1C).
class wnemHammerbrosProc {
public:
    static void HammerFly(wnemSimple* wn, soModuleAccesser* acc);
};
