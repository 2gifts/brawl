#pragma once

#include <sr/sr_common.h>
#include <types.h>

// The global slow-motion / hit-stop controller. Only the pieces fighters poll are described; the rest of the
// 0x4C bytes of state is unreconstructed.
class soSlow {
    u8 unk00[0x4C];
public:
    soSlow();
    ~soSlow();
    // HYPOTHESIS: true while this frame's state is the one that should be estimated (the update pass), and while it
    // should be adjusted (the position fix pass); both are false during a hit pause.
    bool isEstimate();
    bool isAdjust();
    // HYPOTHESIS: names of the fields StageObject::processUpdate reads: the kind of global slow motion (0..2, how the
    // motion frames are advanced), the rate to advance them by, and whether the slow motion is currently on.
    int getKind() const { return *reinterpret_cast<const int*>(unk00 + 0x40); }
    float getRate() const { return *reinterpret_cast<const float*>(unk00 + 0x44); }
    u8 isOn() const { return unk00[0x48]; }

#ifdef SO_SLOW_GET_INSTANCE_OUT_OF_LINE
    // The fighter RELs keep one out-of-line copy of this accessor instead of expanding it at each call.
    static soSlow* getInstance() __attribute__((never_inline)) {
#else
    static soSlow* getInstance() {
#endif
        if (ms_instance == NULL) {
            ms_instance = new (Heaps::System) soSlow();
        }
        return ms_instance;
    }
    static soSlow* ms_instance;
};
