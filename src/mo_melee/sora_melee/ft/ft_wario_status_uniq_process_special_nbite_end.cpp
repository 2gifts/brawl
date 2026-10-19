#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/catch/so_catch_module.h>
#include <types.h>

// Link event passed to the linked articles. The last four fields are read back after the send.
struct ftWarioBiteEndLinkEvent : public soLinkEventArgs {
    ftWarioBiteEndLinkEvent(int eventKind) : soLinkEventArgs(eventKind) { }
    int m_unk08;
    int m_unk0C;
    int m_unk10;
    int m_unk14;
    float m_unk18;
    bool m_unk1C;
};

class ftWarioStatusUniqProcessSpecialNBiteEnd : public soStatusUniqProcess {
public:
    virtual ~ftWarioStatusUniqProcessSpecialNBiteEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

ftWarioStatusUniqProcessSpecialNBiteEnd g_ftWarioStatusUniqProcessSpecialNBiteEnd;

void ftWarioStatusUniqProcessSpecialNBiteEnd::initStatus(soModuleAccesser* moduleAccesser) {
    ftWarioBiteEndLinkEvent event(0x10);
    event.m_unk08 = 0;
    event.m_unk0C = -1;
    event.m_unk10 = -1;
    event.m_unk14 = -1;
    event.m_unk18 = 1.0f;
    event.m_unk1C = true;
    moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
    int value = moduleAccesser->getLinkModule().getNodeTaskId(0);
    moduleAccesser->getWorkManageModule().setInt(value, 0x20000002);
    moduleAccesser->getWorkManageModule().setInt(event.m_unk0C, 0x20000003);
    moduleAccesser->getWorkManageModule().setInt(event.m_unk10, 0x20000004);
}

void ftWarioStatusUniqProcessSpecialNBiteEnd::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus == 0x116) {
        static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->catchCut(false, true);
    } else {
        static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->catchCut(false, false);
    }
}
