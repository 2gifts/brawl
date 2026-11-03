#pragma once

#include <StaticAssert.h>
#include <types.h>

// Local shadow of BrawlHeaders' gf/gf_homemenu.h: adds loadResource and releaseResource (names from the sora map).
class gfHomeMenu {
public:
    bool setBan(u32 p1, u32 p2);
    void kill();
    void start();
    // HYPOTHESIS: loads the files of the HOME Menu again after releaseResource.
    void loadResource();
    // HYPOTHESIS: frees the memory of the HOME Menu (the Wi-Fi interface needs it); false while it cannot.
    bool releaseResource();
};
// TODO size assertion

extern gfHomeMenu* g_gfHomeMenu;
