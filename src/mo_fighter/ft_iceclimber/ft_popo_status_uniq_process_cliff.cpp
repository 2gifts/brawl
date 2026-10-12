#include <ft/ft_status_uniq_process_cliff.h>
#include <types.h>

class ftPopoStatusUniqProcessCliff : public ftStatusUniqProcessCliff {
public:
    virtual ~ftPopoStatusUniqProcessCliff() { }
    virtual bool isLeaveCliff(int status);
};

// Status 0x11e (the popo-only cliff hold) never leaves the cliff on its own.
bool ftPopoStatusUniqProcessCliff::isLeaveCliff(int status) {
    switch (status) {
    case 0x11e:
        return false;
    default:
        return ftStatusUniqProcessCliff::isLeaveCliff(status);
    }
}

ftPopoStatusUniqProcessCliff g_ftPopoStatusUniqProcessCliff;
