// OSTime: time base reads and the calendar conversion helpers.
#include "revolution/os.h"

#define USEC_MAX 1000
#define MSEC_MAX 1000
#define MONTH_MAX 12
#define WEEK_DAY_MAX 7
#define YEAR_DAY_MAX 365

#define SECS_IN_MIN 60
#define SECS_IN_HOUR (SECS_IN_MIN * 60)
#define SECS_IN_DAY (SECS_IN_HOUR * 24)
#define SECS_IN_YEAR (SECS_IN_DAY * 365)

#define BIAS 0xB2575

#define OS_SYSTEM_TIME (*(s64*)0x800030D8)

// the header's bus clock is volatile, the sdk code reads it as a plain constant so it can share one load
#undef OS_TIME_SPEED
#define OS_TIME_SPEED (*(u32*)0x800000F8 / 4)
#undef OS_SEC_TO_TICKS
#undef OS_MSEC_TO_TICKS
#undef OS_USEC_TO_TICKS
#define OS_SEC_TO_TICKS(x) ((x) * (OS_TIME_SPEED))
#define OS_MSEC_TO_TICKS(x) ((x) * (OS_TIME_SPEED / 1000))
#define OS_USEC_TO_TICKS(x) ((x) * (OS_TIME_SPEED / 125000) / 8)

#define OS_TICKS_TO_SEC(x) ((x) / OS_TIME_SPEED)
#define OS_TICKS_TO_MSEC(x) ((x) / (OS_TIME_SPEED / 1000))
#define OS_TICKS_TO_USEC(x) (((x)*8) / (OS_TIME_SPEED / 125000))

// days up to the start of each month, normal year then leap year
static s32 YearDays[MONTH_MAX] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
static s32 LeapYearDays[MONTH_MAX] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};

asm OSTime OSGetTime(void) {
    nofralloc

    mftbu r3
    mftb r4

    // the carry from the low word into the high word could have raced, read it again
    mftbu r5
    cmpw r3, r5
    bne OSGetTime

    blr
}

asm OSTick OSGetTick(void) {
    nofralloc

    mftb r3
    blr
}

OSTime __OSGetSystemTime(void) {
    BOOL enabled = OSDisableInterrupts();
    OSTime time = OSGetTime() + OS_SYSTEM_TIME;
    OSRestoreInterrupts(enabled);
    return time;
}

OSTime __OSTimeToSystemTime(OSTime time) {
    BOOL enabled = OSDisableInterrupts();
    OSTime sysTime = OS_SYSTEM_TIME + time;
    OSRestoreInterrupts(enabled);
    return sysTime;
}

static BOOL IsLeapYear(s32 year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static s32 GetYearDays(s32 year, s32 mon) {
    return (IsLeapYear(year) ? LeapYearDays : YearDays)[mon];
}

static s32 GetLeapDays(s32 year) {
    if (year < 1) {
        return 0;
    }
    return (year + 3) / 4 - (year - 1) / 100 + (year - 1) / 400;
}

// the helpers are inlined by hand here: GetDates itself has to stay out of line (dont_inline), and that
// pragma also stops the compiler from inlining anything inside it
#pragma dont_inline on
static void GetDates(s32 days, OSCalendarTime* cal) {
    s32 year;
    s32 totalDays;
    s32* p_days;
    s32 month;
    BOOL isLeap;

    cal->wday = (days + 6) % WEEK_DAY_MAX;

    for (year = days / YEAR_DAY_MAX;
         days < (totalDays = year * YEAR_DAY_MAX + (year < 1 ? 0 : (year + 3) / 4 - (year - 1) / 100 + (year - 1) / 400)); year--) {
        ;
    }
    days -= totalDays;
    cal->year = year;
    cal->yday = days;

    isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    p_days = isLeap ? LeapYearDays : YearDays;
    for (month = MONTH_MAX; days < p_days[--month];) {
        ;
    }
    cal->mon = month;
    cal->mday = days - p_days[month] + 1;
}
#pragma dont_inline reset

void OSTicksToCalendarTime(OSTime ticks, OSCalendarTime* cal) {
    s32 days, secs;
    OSTime d;

    d = ticks % OS_SEC_TO_TICKS(1);
    if (d < 0) {
        d += OS_SEC_TO_TICKS(1);
    }

    cal->usec = OS_TICKS_TO_USEC(d) % USEC_MAX;
    cal->msec = OS_TICKS_TO_MSEC(d) % MSEC_MAX;
    ticks -= d;

    days = (OS_TICKS_TO_SEC(ticks) / SECS_IN_DAY) + BIAS;
    secs = OS_TICKS_TO_SEC(ticks) % SECS_IN_DAY;
    if (secs < 0) {
        days -= 1;
        secs += SECS_IN_DAY;
    }

    GetDates(days, cal);
    cal->hour = secs / 60 / 60;
    cal->min = secs / 60 % 60;
    cal->sec = secs % 60;
}

OSTime OSCalendarTimeToTicks(const OSCalendarTime* cal) {
    OSTime seconds;
    s32 month;
    s32 ovMon;
    s32 year;

    ovMon = cal->mon / MONTH_MAX;
    month = cal->mon - (ovMon * MONTH_MAX);

    if (month < 0) {
        month += MONTH_MAX;
        ovMon--;
    }

    year = cal->year + ovMon;

    seconds = (OSTime)SECS_IN_YEAR * year + (OSTime)SECS_IN_DAY * (cal->mday + GetLeapDays(year) + GetYearDays(year, month) - 1) +
              (OSTime)SECS_IN_HOUR * cal->hour + (OSTime)SECS_IN_MIN * cal->min + cal->sec - (OSTime)0xEB1E1BF80ULL;

    return OS_SEC_TO_TICKS(seconds) + OS_MSEC_TO_TICKS((OSTime)cal->msec) + OS_USEC_TO_TICKS((OSTime)cal->usec);
}
