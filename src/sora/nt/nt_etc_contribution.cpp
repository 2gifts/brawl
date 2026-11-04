#include <nt/nt_etc_contribution.h>
#include <nt/nt_etc_global.h>
#include <nt/nt_etc_sbs.h>
#include <revolution/OS/OSTime.h>
#include <types.h>

bool nteContribution::upload(s32 kind, void* data, u32 size) {
    bool terminated = OSIsThreadTerminated(&m_thread) == 1;
    if (terminated) {
        return false;
    }
    if (unk354 != 100) {
        return false;
    }
    if (!data || !size) {
        return false;
    }
    s32 job = -1;
    if (kind == 0) {
        job = 2;
    } else if (kind == 1) {
        job = 1;
    } else if (kind == 2) {
        job = 3;
    }
    if (job == -1) {
        return false;
    }
    unk354 = job;
    unk358 = data;
    unk35C = size;
    OSResumeThread(&m_thread);
    return true;
}

void nteContribution::notifySBSEndProc(s32 notification, s32 result) {
    unk380 = unk368->getErrorCode(result);
    if (unk380) {
        unk384 = true;
        unk36C = true;
    }
}

bool nteContribution::isAbleToContribute() {
    bool valid;
    bool able = false;
    s64 now = nteGlobal::getTime(&valid);
    if (valid) {
        s64 start = nteGlobal::getContribute_Start();
        s64 end = nteGlobal::getContribute_End();
        s64 last = nteGlobal::getLastContributed();
        if (start && end && now >= start && now <= end) {
            if (nteGlobal::isInfinityContribute()) {
                able = true;
            } else {
                OSCalendarTime currentDate;
                OSCalendarTime lastDate;
                OSTicksToCalendarTime(now, &currentDate);
                OSTicksToCalendarTime(last, &lastDate);
                if (currentDate.year != lastDate.year ||
                    currentDate.month != lastDate.month ||
                    currentDate.mday != lastDate.mday) {
                    able = true;
                }
            }
        }
    }
    return able;
}
