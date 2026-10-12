#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Local view of the stop (hit-stop) module; no header in this tree. Slots 7 and 9 are called without arguments.
class ftStopModuleView {
public:
    virtual ~ftStopModuleView() { }
    virtual void unk3();
    virtual void unk4();
    virtual void unk5();
    virtual void unk6();
    virtual void unk7();
    virtual void unk8();
    virtual void unk9();
};

class ftStatusUniqProcessIkeFinalDamageFall : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessIkeFinalDamageFall() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void execFixCamera(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* unk);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};

ftStatusUniqProcessIkeFinalDamageFall g_ftStatusUniqProcessIkeFinalDamageFall;

void ftStatusUniqProcessIkeFinalDamageFall::initStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessIkeFinalDamageFall::execStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessIkeFinalDamageFall::execFixPos(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessIkeFinalDamageFall::execFixCamera(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().chkLinkStop() == false) {
        reinterpret_cast<ftStopModuleView*>(moduleAccesser->m_enumerationStart->m_stopModule)->unk7();
        reinterpret_cast<ftStopModuleView*>(moduleAccesser->m_enumerationStart->m_stopModule)->unk9();
    }
}

bool ftStatusUniqProcessIkeFinalDamageFall::checkDamage(soModuleAccesser* moduleAccesser, void* unk) {
    // HYPOTHESIS: damage is only checked while work flag 0x22000010 is off.
    return moduleAccesser->getWorkManageModule().isFlag(0x22000010) == false;
}

void ftStatusUniqProcessIkeFinalDamageFall::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    if (moduleAccesser->getLinkModule().isLink(4)) {
        moduleAccesser->getLinkModule().unlink(4);
    }
}
