#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Partial shell: only the virtual status hooks are declared. The collision-search, catch and
// target-position helpers of this unit are not reconstructed yet.
class ftToonLinkStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftToonLinkStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

class ftToonLinkStatusUniqProcessFinalDash : public soStatusUniqProcess {
public:
    virtual ~ftToonLinkStatusUniqProcessFinalDash() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

class ftToonLinkStatusUniqProcessFinalCombo : public soStatusUniqProcess {
public:
    virtual ~ftToonLinkStatusUniqProcessFinalCombo() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

void ftToonLinkStatusUniqProcessFinal::initStatus(soModuleAccesser*) { }
void ftToonLinkStatusUniqProcessFinal::exitStatus(soModuleAccesser*, int) { }
void ftToonLinkStatusUniqProcessFinal::execFixPos(soModuleAccesser*) { }

void ftToonLinkStatusUniqProcessFinalDash::initStatus(soModuleAccesser*) { }
void ftToonLinkStatusUniqProcessFinalDash::exitStatus(soModuleAccesser*, int) { }
void ftToonLinkStatusUniqProcessFinalDash::execStatus(soModuleAccesser*) { }
void ftToonLinkStatusUniqProcessFinalDash::execFixPos(soModuleAccesser*) { }

void ftToonLinkStatusUniqProcessFinalCombo::initStatus(soModuleAccesser*) { }
void ftToonLinkStatusUniqProcessFinalCombo::exitStatus(soModuleAccesser*, int) { }
void ftToonLinkStatusUniqProcessFinalCombo::execFixPos(soModuleAccesser*) { }

ftToonLinkStatusUniqProcessFinal g_ftToonLinkStatusUniqProcessFinal;
ftToonLinkStatusUniqProcessFinalDash g_ftToonLinkStatusUniqProcessFinalDash;
ftToonLinkStatusUniqProcessFinalCombo g_ftToonLinkStatusUniqProcessFinalCombo;
