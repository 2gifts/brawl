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

class ftStatusUniqProcessIkeFinalDamageFly : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessIkeFinalDamageFly() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void execFixCamera(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* unk);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};

ftStatusUniqProcessIkeFinalDamageFly g_ftStatusUniqProcessIkeFinalDamageFly;

void ftStatusUniqProcessIkeFinalDamageFly::initStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessIkeFinalDamageFly::execStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessIkeFinalDamageFly::execFixPos(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessIkeFinalDamageFly::execFixCamera(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().chkLinkStop() == false) {
        reinterpret_cast<ftStopModuleView*>(moduleAccesser->m_enumerationStart->m_stopModule)->unk7();
        reinterpret_cast<ftStopModuleView*>(moduleAccesser->m_enumerationStart->m_stopModule)->unk9();
    }
}

bool ftStatusUniqProcessIkeFinalDamageFly::checkDamage(soModuleAccesser* moduleAccesser, void* unk) { return true; }

void ftStatusUniqProcessIkeFinalDamageFly::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    if (nextStatusKind != 0xf8) {
        if (moduleAccesser->getLinkModule().isLink(4)) {
            moduleAccesser->getLinkModule().unlink(4);
        }
    }
}
