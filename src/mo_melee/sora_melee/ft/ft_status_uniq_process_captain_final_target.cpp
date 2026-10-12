#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessCaptainFinalTarget : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptainFinalTarget() { }
};

class ftStatusUniqProcessCaptainFinalStart : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptainFinalStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftStatusUniqProcessCaptainFinalFuraFura : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptainFinalFuraFura() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

class ftStatusUniqProcessCaptainFinalClash : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptainFinalClash() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftStatusUniqProcessCaptainFinalEnd : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptainFinalEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessCaptainFinalTarget g_ftStatusUniqProcessCaptainFinalTarget;
ftStatusUniqProcessCaptainFinalStart g_ftStatusUniqProcessCaptainFinalStart;
ftStatusUniqProcessCaptainFinalFuraFura g_ftStatusUniqProcessCaptainFinalFuraFura;
ftStatusUniqProcessCaptainFinalClash g_ftStatusUniqProcessCaptainFinalClash;
ftStatusUniqProcessCaptainFinalEnd g_ftStatusUniqProcessCaptainFinalEnd;

void ftStatusUniqProcessCaptainFinalStart::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessCaptainFinalStart::execStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessCaptainFinalFuraFura::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessCaptainFinalFuraFura::execStatus(soModuleAccesser* moduleAccesser) { }
bool ftStatusUniqProcessCaptainFinalFuraFura::checkDamage(soModuleAccesser* moduleAccesser, void*) { return false; }
void ftStatusUniqProcessCaptainFinalClash::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessCaptainFinalClash::execStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessCaptainFinalEnd::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessCaptainFinalEnd::execStatus(soModuleAccesser* moduleAccesser) { }
