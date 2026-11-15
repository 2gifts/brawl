#define SO_CATCH_MODULE_CLING_SLOTS // the catch module slots after catchCut (so_catch_module.h)
#include <so/catch/so_catch_module.h>
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: the link event the Force Palm grab sends to its victim (the same record as Ganondorf's Flame Choke
// sends): the payload picks the victim's status; the victim answers in the three words after it.
struct ftLucarioSpecialSLinkEvent : soLinkEventArgs {
    s32 m_payload;
    s32 m_unkC;
    s32 m_unk10;
    s32 m_unk14;
    float m_unk18;
    bool m_unk1C;
    ftLucarioSpecialSLinkEvent(int kind, s32 payload, bool flag) : soLinkEventArgs(kind), m_payload(payload), m_unkC(-1), m_unk10(-1), m_unk14(-1), m_unk18(1.0f), m_unk1C(flag) { }
};

class ftLucarioStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

class ftLucarioStatusUniqProcessSpecialSThrow : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialSThrow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

static inline soCatchModule* getCatchModule(soModuleAccesser* moduleAccesser) {
    return static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule);
}

void ftLucarioStatusUniqProcessSpecialS::initStatus(soModuleAccesser*) {}
void ftLucarioStatusUniqProcessSpecialS::execStatus(soModuleAccesser*) {}
void ftLucarioStatusUniqProcessSpecialS::exitStatus(soModuleAccesser*, int) {}

void ftLucarioStatusUniqProcessSpecialSThrow::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().offFlag(0x22000010);
    getCatchModule(moduleAccesser)->setCatch(-1);
}

// Starts the throw once the grab connected (flag 0x22000010) and the event was not sent yet: the victim gets the
// throw event, and its answer is kept in the work area.
void ftLucarioStatusUniqProcessSpecialSThrow::execFixPos(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    bool sent = workManage.isFlag(0x22000011);
    bool caught = workManage.isFlag(0x22000010);
    if (!sent && caught) {
        ftLucarioSpecialSLinkEvent event(0xb, 0x77, true);
        moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
        moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getLinkModule().getNodeTaskId(0), 0x20000002);
        moduleAccesser->getWorkManageModule().setInt(event.m_unkC, 0x20000003);
        moduleAccesser->getWorkManageModule().setInt(event.m_unk10, 0x20000004);
        moduleAccesser->getWorkManageModule().setFloat(event.m_unk18, 0x21000004);
        workManage.onFlag(0x22000011);
    }
}

void ftLucarioStatusUniqProcessSpecialSThrow::exitStatus(soModuleAccesser* moduleAccesser, int) {
    getCatchModule(moduleAccesser)->catchCut(false, false);
}

bool ftLucarioStatusUniqProcessSpecialSThrow::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
        return false;
    }
    return getCatchModule(moduleAccesser)->checkDamage();
}

ftLucarioStatusUniqProcessSpecialS g_ftLucarioStatusUniqProcessSpecialS;
ftLucarioStatusUniqProcessSpecialSThrow g_ftLucarioStatusUniqProcessSpecialSThrow;
