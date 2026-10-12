#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

class soModuleAccesser;

// HYPOTHESIS: the camera and rotate helpers are soModuleAccesser members in the map (fn_118_11410 / fn_118_1117C).
extern "C" void fn_118_11410(soModuleAccesser*);
extern "C" void fn_118_1117C(soModuleAccesser*);

class ftLucarioStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
};

class ftLucarioStatusUniqProcessFinalEntry : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessFinalEntry() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
};

class ftLucarioStatusUniqProcessFinalHold : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessFinalHold() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
};

class ftLucarioStatusUniqProcessFinalAttack : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessFinalAttack() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
};

class ftLucarioStatusUniqProcessFinalEnd : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessFinalEnd() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
};

void ftLucarioStatusUniqProcessFinal::initStatus(soModuleAccesser*) { }
void ftLucarioStatusUniqProcessFinal::execStatus(soModuleAccesser*) { }

void ftLucarioStatusUniqProcessFinalEntry::initStatus(soModuleAccesser* moduleAccesser) { fn_118_11410(moduleAccesser); }
void ftLucarioStatusUniqProcessFinalEntry::execStatus(soModuleAccesser*) { }

void ftLucarioStatusUniqProcessFinalHold::initStatus(soModuleAccesser* moduleAccesser) { fn_118_11410(moduleAccesser); }
void ftLucarioStatusUniqProcessFinalHold::execStatus(soModuleAccesser* moduleAccesser) { fn_118_1117C(moduleAccesser); }

void ftLucarioStatusUniqProcessFinalAttack::initStatus(soModuleAccesser*) { }
void ftLucarioStatusUniqProcessFinalAttack::execStatus(soModuleAccesser*) { }

void ftLucarioStatusUniqProcessFinalEnd::initStatus(soModuleAccesser*) { }
void ftLucarioStatusUniqProcessFinalEnd::execStatus(soModuleAccesser*) { }

ftLucarioStatusUniqProcessFinal g_ftLucarioStatusUniqProcessFinal;
ftLucarioStatusUniqProcessFinalEntry g_ftLucarioStatusUniqProcessFinalEntry;
ftLucarioStatusUniqProcessFinalHold g_ftLucarioStatusUniqProcessFinalHold;
ftLucarioStatusUniqProcessFinalAttack g_ftLucarioStatusUniqProcessFinalAttack;
ftLucarioStatusUniqProcessFinalEnd g_ftLucarioStatusUniqProcessFinalEnd;
