#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/catch/so_catch_module.h>
#include <so/link/so_link_module_impl.h>
#include <types.h>

// Local view of soLinkModule. The shared header's virtual order does not reproduce the slot the
// fighter code calls (getParentModelLoupeNo at vtable +0x54), so the first 19 slots are placeholders (view offset includes two implicit slots).
class ftLinkModuleLoupeView {
public:
    virtual void slot0() = 0; virtual void slot1() = 0; virtual void slot2() = 0; virtual void slot3() = 0;
    virtual void slot4() = 0; virtual void slot5() = 0; virtual void slot6() = 0; virtual void slot7() = 0;
    virtual void slot8() = 0; virtual void slot9() = 0; virtual void slot10() = 0; virtual void slot11() = 0;
    virtual void slot12() = 0; virtual void slot13() = 0; virtual void slot14() = 0; virtual void slot15() = 0;
    virtual void slot16() = 0; virtual void slot17() = 0; virtual void slot18() = 0;
    virtual int getParentModelLoupeNo(u32 index) = 0;
};

// HYPOTHESIS: link event sent to the linked fighter; layout recovered from the stack frame.
struct ftDededeNSpitLinkEventArgs : public soLinkEventArgs {
    float m_param;
    int m_value;
    bool m_flag;
    inline ftDededeNSpitLinkEventArgs(int eventKind) : soLinkEventArgs(eventKind), m_param(0.0f), m_flag(false) {}
};

class ftDededeStatusUniqProcessSpecialNSpit : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialNSpit() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftDededeStatusUniqProcessSpecialNSpit g_ftDededeStatusUniqProcessSpecialNSpit;

void ftDededeStatusUniqProcessSpecialNSpit::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* workManage = &moduleAccesser->getWorkManageModule();
    workManage->setInt(reinterpret_cast<ftLinkModuleLoupeView*>(&moduleAccesser->getLinkModule())->getParentModelLoupeNo(0), 0x20000009);
    moduleAccesser->getWorkManageModule().setInt(0, 0x2000000a);
    moduleAccesser->getWorkManageModule().setInt(0, 0x2000000b);
}

void ftDededeStatusUniqProcessSpecialNSpit::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialNSpit::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000015)) {
        moduleAccesser->getWorkManageModule().onFlag(0x22000016);
        moduleAccesser->getWorkManageModule().offFlag(0x22000015);
        ftDededeNSpitLinkEventArgs args(0x17);
        args.m_value = moduleAccesser->getWorkManageModule().getInt(0x20000004);
        moduleAccesser->getLinkModule().sendEventNodes(0, args, 0);
    }
}

void ftDededeStatusUniqProcessSpecialNSpit::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (!moduleAccesser->getWorkManageModule().isFlag(0x22000016)) {
        if (moduleAccesser->getLinkModule().isLinked(0) == true) {
            static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->catchCut(false, true);
        }
    }
}
