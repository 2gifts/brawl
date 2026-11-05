#include <types.h>
#include <ef/ef_screen.h>
#include <sr/sr_common.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
efScreen* g_efScreen = 0;

void efScreen::create() {
    if (!g_efScreen) {
        g_efScreen = new (Heaps::System) efScreen();
    }
}

void efScreen::destroy() {
    if (g_efScreen) {
        delete g_efScreen;
    }
}
