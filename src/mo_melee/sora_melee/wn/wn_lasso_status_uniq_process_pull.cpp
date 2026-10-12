#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <wn/wn_lasso_status_view.h>
#include <types.h>

class wnLassoStatusUniqProcessPull : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessPull() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnLassoStatusUniqProcessPull g_wnLassoStatusUniqProcessPull;

class wnLassoPullLinkEvent : public soLinkEventArgs {
public:
    wnLassoPullLinkEvent(int eventKind) : soLinkEventArgs(eventKind) { }
};

void wnLassoStatusUniqProcessPull::initStatus(soModuleAccesser* moduleAccesser) {
    wnLassoGetAltStatus(moduleAccesser)->unkA4Int(4);
}

void wnLassoStatusUniqProcessPull::execFixPos(soModuleAccesser* moduleAccesser) {
    wnLassoGetAltStatus(moduleAccesser)->unkB4Int(1);
    if (wnLassoGetAltStatus(moduleAccesser)->unk88Int() <= 0) {
        // HYPOTHESIS: event 0x839 is sent to the linked parents when the pull has no status kind yet.
        wnLassoPullLinkEvent eventInfo(0x839);
        moduleAccesser->getLinkModule().sendEventParents(1, eventInfo);
    }
}
