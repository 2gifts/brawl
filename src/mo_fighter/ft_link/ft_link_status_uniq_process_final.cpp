#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

class soModuleAccesser;

class ftLinkStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftLinkStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execFixPos(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
};

class ftLinkStatusUniqProcessFinalDash : public soStatusUniqProcess {
public:
    virtual ~ftLinkStatusUniqProcessFinalDash() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void execFixPos(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
};

class ftLinkStatusUniqProcessFinalCombo : public soStatusUniqProcess {
public:
    virtual ~ftLinkStatusUniqProcessFinalCombo() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execFixPos(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
};

void ftLinkStatusUniqProcessFinal::initStatus(soModuleAccesser*) { }
void ftLinkStatusUniqProcessFinal::execFixPos(soModuleAccesser*) { }
void ftLinkStatusUniqProcessFinal::exitStatus(soModuleAccesser*, int) { }

void ftLinkStatusUniqProcessFinalDash::initStatus(soModuleAccesser*) { }
void ftLinkStatusUniqProcessFinalDash::execStatus(soModuleAccesser*) { }
void ftLinkStatusUniqProcessFinalDash::execFixPos(soModuleAccesser*) { }
void ftLinkStatusUniqProcessFinalDash::exitStatus(soModuleAccesser*, int) { }

void ftLinkStatusUniqProcessFinalCombo::initStatus(soModuleAccesser*) { }
void ftLinkStatusUniqProcessFinalCombo::execFixPos(soModuleAccesser*) { }
void ftLinkStatusUniqProcessFinalCombo::exitStatus(soModuleAccesser*, int) { }

ftLinkStatusUniqProcessFinal g_ftLinkStatusUniqProcessFinal;
ftLinkStatusUniqProcessFinalDash g_ftLinkStatusUniqProcessFinalDash;
ftLinkStatusUniqProcessFinalCombo g_ftLinkStatusUniqProcessFinalCombo;
