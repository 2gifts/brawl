#include <types.h>
#include <FAT/pf2.h>
#include <revolution/os/OSTime.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
u32 lbl_805A1260 = 0;

// Function bodies below are derived from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own.

void PF_enterCritical(void* unk) {
    PF_CRITICAL* critical = unk;
    u32 context;

    if (critical->count == 0) {
        fn_803EC3D4(critical->resource);
        fn_803EC27C(&critical->owner);
    } else {
        fn_803EC27C(&context);
        if (critical->owner != context) {
            fn_803EC3D4(critical->resource);
            critical->owner = context;
        }
    }
    critical->count++;
}

void PF_exitCritical(void* unk) {
    PF_CRITICAL* critical = unk;

    critical->count--;
    if (critical->count != 0) {
        return;
    }
    critical->owner = 0;
    fn_803EC418(critical->resource);
}

void fn_803EBB4C(void) {
    lbl_805A1260 = 0;
}

int fn_803EBB58(PF_FILE* p_file) {
    PF_SFD* p_sfd;

    fn_803EC418(p_file->p_sfd->lock.resource);
    p_sfd = p_file->p_sfd;
    if (p_sfd->lock.count == 0 && p_sfd->lock.wcount == 0 && lbl_805A1260 != 0) {
        fn_803EC394(p_sfd->lock.resource);
        p_file->p_sfd->lock.resource = 0;
        lbl_805A1260--;
    }
    return 0;
}

void fn_803EBA40(PF_CRITICAL* critical) {
    PFK_SEMAPHORE_INFO info;

    if (critical->unk0 == 0) {
        info.initial = 1;
        info.max = 1;
        // HYPOTHESIS: the semaphore storage follows the PF_CRITICAL header.
        info.sem = (struct OSSemaphore*)(critical + 1);
        critical->resource = fn_803EC2F8(&info);
        critical->count = 0;
        critical->owner = 0;
        critical->unk0 = 1;
    }
}

void fn_803EBBE0(s32* date, s32* time) {
    OSCalendarTime cal;

    OSTicksToCalendarTime(OSGetTime(), &cal);
    date[0] = cal.year;
    date[1] = cal.month + 1;
    date[2] = cal.mday;
    time[0] = cal.hour;
    time[1] = cal.min;
    time[2] = cal.sec;
}
