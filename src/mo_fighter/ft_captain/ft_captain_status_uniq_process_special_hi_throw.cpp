#include <ft/captain/ft_captain_status_uniq_process_special_hi_cling.h>
#include <so/so_module_accesser.h>
#include <so/link/so_link_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

struct ftCaptainSpecialHiThrowLinkEvent : soLinkEventArgs {
    s32 m_unk8;
    s32 m_unkC;
    s32 m_unk10;
    s32 m_unk14;
    float m_unk18;
    bool m_unk1C;

    ftCaptainSpecialHiThrowLinkEvent(int eventKind)
        : soLinkEventArgs(eventKind), m_unk8(-1), m_unkC(-1), m_unk10(-1), m_unk14(-1), m_unk18(1.0f), m_unk1C(true) {}
};

class ftCaptainStatusUniqProcessSpecialHiThrow : public ftCaptainStatusUniqProcessSpecialHiCling {
public:
    virtual ~ftCaptainStatusUniqProcessSpecialHiThrow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

void ftCaptainStatusUniqProcessSpecialHiThrow::initStatus(soModuleAccesser* moduleAccesser) {
    ftCaptainSpecialHiThrowLinkEvent event(0xb);
    moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
    moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getLinkModule().getNodeTaskId(0), 0x20000002);
    moduleAccesser->getWorkManageModule().setInt(event.m_unkC, 0x20000003);
    moduleAccesser->getWorkManageModule().setInt(event.m_unk10, 0x20000004);
}

ftCaptainStatusUniqProcessSpecialHiThrow g_ftCaptainStatusUniqProcessSpecialHiThrow;

// Names are corroborated by the map and the shared target-work callers.
class ftCaptainFinalSubRoutine {
public:
    // HYPOTHESIS: setup/cleanup return void; their callers consume no result.
    static void init_target_work();
    static bool check_status_transition(soModuleAccesser* moduleAccesser, int nextStatus);
    static void free_target_work();
};

class ftCaptainStatusUniqProcessFinalStart : public soStatusUniqProcess {
public:
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftCaptainStatusUniqProcessFinalStart::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManageModule = moduleAccesser->getWorkManageModule();
    workManageModule.offFlag(0x22000010);
    workManageModule.setInt(0, 0x20000000);
    ftCaptainFinalSubRoutine::init_target_work();
}

void ftCaptainStatusUniqProcessFinalStart::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (!ftCaptainFinalSubRoutine::check_status_transition(moduleAccesser, nextStatus)) {
        ftCaptainFinalSubRoutine::free_target_work();
    }
}
