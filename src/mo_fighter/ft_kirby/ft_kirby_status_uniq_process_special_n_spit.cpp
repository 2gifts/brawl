#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: float constant from lbl_96_rodata_80 (value not verified).
extern const float lbl_96_rodata_80;

class ftKirbyStatusUniqProcessSpecialNSpit : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessSpecialNSpit() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

// Link event sent to the attached node while spitting; field names are HYPOTHESIS.
struct ftKirbySpitLinkEvent : public soLinkEventArgs {
    ftKirbySpitLinkEvent(int eventKind) : soLinkEventArgs(eventKind) { }
    float m_value;
    int m_taskId;
    bool m_flag;
};

// Catch module is a void* in the accesser. The view pads the vtable with placeholder slots (unkNN names are
// not real members) so that unk0x2c lands at vtable offset +0x2c.
class ftKirbyCatchModuleView {
public:
    virtual ~ftKirbyCatchModuleView() { }
    virtual void unk0x10();
    virtual void unk0x14();
    virtual void unk0x18();
    virtual void unk0x1c();
    virtual void unk0x20();
    virtual void unk0x24();
    virtual void unk0x28();
    virtual void unk0x2a();
    virtual void unk0x2c(int, int);
};

void ftKirbyStatusUniqProcessSpecialNSpit::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().setInt(0, 0x1000004b);
    moduleAccesser->getLinkModule().getNodeTaskId(0);
    moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getLinkModule().getNodeTaskId(0), 0x20000006);
    moduleAccesser->getWorkManageModule().setInt(0, 0x20000007);
    moduleAccesser->getWorkManageModule().setInt(0, 0x20000008);
}

void ftKirbyStatusUniqProcessSpecialNSpit::execStatus(soModuleAccesser*) { }

void ftKirbyStatusUniqProcessSpecialNSpit::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000015)) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000015);
        moduleAccesser->getWorkManageModule().onFlag(0x22000016);
        ftKirbySpitLinkEvent event(0x17);
        event.m_value = lbl_96_rodata_80;
        event.m_flag = false;
        event.m_taskId = moduleAccesser->getWorkManageModule().getInt(0x1000004a);
        moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
    }
}

void ftKirbyStatusUniqProcessSpecialNSpit::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (!moduleAccesser->getWorkManageModule().isFlag(0x22000016)) {
        if (moduleAccesser->getLinkModule().isLinked(0) == true) {
            reinterpret_cast<ftKirbyCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule)->unk0x2c(0, 1);
        }
    }
}

ftKirbyStatusUniqProcessSpecialNSpit g_ftKirbyStatusUniqProcessSpecialNSpit;
