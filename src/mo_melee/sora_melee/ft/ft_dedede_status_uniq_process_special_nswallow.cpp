#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Link event with a kind; soLinkEventArgs keeps its constructor protected.
class ftDededeSwallowLinkEvent : public soLinkEventArgs {
public:
    ftDededeSwallowLinkEvent(int eventKind) : soLinkEventArgs(eventKind) {}
};

class ftDededeStatusUniqProcessSpecialNSwallow : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialNSwallow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftDededeStatusUniqProcessSpecialNSwallow g_ftDededeStatusUniqProcessSpecialNSwallow;

void ftDededeStatusUniqProcessSpecialNSwallow::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialNSwallow::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000018)) {
        ftDededeSwallowLinkEvent event(0x36);
        moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
        moduleAccesser->getWorkManageModule().offFlag(0x22000018);
    }
}

void ftDededeStatusUniqProcessSpecialNSwallow::exitStatus(soModuleAccesser* moduleAccesser, int) {
}
