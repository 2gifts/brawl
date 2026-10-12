#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Catch module view. The accesser holds it as a void*; the catch vtable is only partly declared
// in so/catch/so_catch_module.h, so slots are padded to their native offsets (catchCut at 0x2c).
class ftWarioCatchModuleView {
public:
    virtual ~ftWarioCatchModuleView() { }
    virtual void unk0x04() { }
    virtual void unk0x08() { }
    virtual void unk0x0c() { }
    virtual void unk0x10() { }
    virtual void unk0x14() { }
    virtual void unk0x18() { }
    virtual void unk0x1c() { }
    virtual void unk0x20() { }
    virtual void catchCut(bool, bool);
};

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
        reinterpret_cast<ftWarioCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule)->catchCut(false, true);
    } else {
        reinterpret_cast<ftWarioCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule)->catchCut(false, false);
    }
}
