// the current wall clock as an OSCalendarTime, from the wc24 rtc counter when that is usable and the console
// tick counter when it isn't. the rtc counter is only resynced about once a minute.
#include "revolution.h"
#include "revolution/nwc24.h"
#include "revolution/nwc24/NWC24Time.h"
#include "revolution/nwc24/internal/NWC24iDateParser.h"

static s64 lastSync;

// HYPOTHESIS: returns TRUE when the calendar time came from the wc24 clock
BOOL fn_8038EC6C(OSCalendarTime *out) {
    NWC24Date date;
    s64 universal;

    if (lastSync == 0 || lastSync + OS_TIMER_CLOCK * 60 < __OSGetSystemTime()) {
        NWC24iSynchronizeRtcCounter(FALSE);
        lastSync = __OSGetSystemTime();
    }

    if (NWC24iGetUniversalTime(&universal) >= 0 && NWC24iEpochSecondsToDate(&date, universal) >= 0 &&
        NWC24iDateToOSCalendarTime(out, &date) >= 0) {
        return TRUE;
    }

    OSTicksToCalendarTime(OSGetTime(), out);
    return FALSE;
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A1140[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1144[0x4] __attribute__((aligned(4))) = { 0 };
