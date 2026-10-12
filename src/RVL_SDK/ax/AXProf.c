// AXProf: hands out profile slots round-robin (profiling itself is compiled out in the retail audio engine).
#include "revolution/ax.h"

typedef struct {
    u8 data[0x38];
} AXProfileSlot;

static AXProfileSlot* __AXProfile;
static u32 __AXMaxProfiles;
static u32 __AXCurrentProfile;
static BOOL __AXProfileInitialized = FALSE;

AXProfileSlot* __AXGetCurrentProfile(void) {
    if (__AXProfileInitialized) {
        AXProfileSlot* prof = &__AXProfile[__AXCurrentProfile];

        __AXCurrentProfile++;
        __AXCurrentProfile %= __AXMaxProfiles;

        return prof;
    }

    return NULL;
}
