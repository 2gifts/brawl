#include <gf/gf_application.h>
#include <types.h>

extern "C" {
void VISetBlack(BOOL black);
void VIFlush();
void VIWaitForRetrace();
void fn_801FF1AC();
}

void gfApplication::reset() {
    VISetBlack(TRUE);
    VIFlush();
    fn_801FF1AC();
    VIWaitForRetrace();
    VIWaitForRetrace();
}
