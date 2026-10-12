#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftKirbyStatusUniqProcessSpecialNSwallow : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessSpecialNSwallow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

// Link event with only the base fields; the event kind is passed in.
struct ftKirbySwallowLinkEvent : public soLinkEventArgs {
    ftKirbySwallowLinkEvent(int eventKind) : soLinkEventArgs(eventKind) { }
};

// Catch module is a void* in the accesser. Placeholder virtuals pad the vtable so that
// unk0x1c and unk0x2c land at their native vtable offsets.
class ftKirbyCatchModuleView {
public:
    virtual ~ftKirbyCatchModuleView() { }
    virtual void unk0x0c();
    virtual void unk0x10();
    virtual void unk0x14();
    virtual void unk0x1c(int);
    virtual void unk0x18();
    virtual void unk0x20();
    virtual void unk0x24();
    virtual void unk0x28();
    virtual void unk0x2c(int, int);
};

void ftKirbyStatusUniqProcessSpecialNSwallow::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x119:
        reinterpret_cast<ftKirbyCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule)->unk0x1c(-1);
        break;
    case 0x11a:
        moduleAccesser->getCollisionHitModule().setXluGlobal(0);
        break;
    }
}

void ftKirbyStatusUniqProcessSpecialNSwallow::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000019)) {
        ftKirbySwallowLinkEvent event(0x36);
        moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
        moduleAccesser->getWorkManageModule().offFlag(0x22000019);
    }
}

void ftKirbyStatusUniqProcessSpecialNSwallow::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status == 0x11b || status == 0x121) {
        if (moduleAccesser->getLinkModule().isLinked(0) == true) {
            ftKirbySwallowLinkEvent event(0x13);
            moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
        }
    } else if (status != 0x11a) {
        if (moduleAccesser->getLinkModule().isLinked(0) == true) {
            reinterpret_cast<ftKirbyCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule)->unk0x2c(0, 1);
        }
        moduleAccesser->getCollisionHitModule().cancelXluGlobal(0);
    }
}

ftKirbyStatusUniqProcessSpecialNSwallow g_ftKirbyStatusUniqProcessSpecialNSwallow;
