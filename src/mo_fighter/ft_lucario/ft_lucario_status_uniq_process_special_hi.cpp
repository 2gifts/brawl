#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

class soModuleAccesser;

class ftLucarioStatusUniqProcessSpecialHi : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialHi() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
};

class ftLucarioStatusUniqProcessSpecialHiRush : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialHiRush() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
    virtual void execFixPos(soModuleAccesser*);
};

class ftLucarioStatusUniqProcessSpecialHiRushEnd : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialHiRushEnd() { }
};

class ftLucarioStatusUniqProcessSpecialHiBound : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialHiBound() { }
};

void ftLucarioStatusUniqProcessSpecialHi::initStatus(soModuleAccesser*) { }
void ftLucarioStatusUniqProcessSpecialHi::execStatus(soModuleAccesser*) { }

void ftLucarioStatusUniqProcessSpecialHiRush::initStatus(soModuleAccesser*) { }
void ftLucarioStatusUniqProcessSpecialHiRush::execStatus(soModuleAccesser*) { }
void ftLucarioStatusUniqProcessSpecialHiRush::exitStatus(soModuleAccesser*, int) { }
void ftLucarioStatusUniqProcessSpecialHiRush::execFixPos(soModuleAccesser*) { }

ftLucarioStatusUniqProcessSpecialHi g_ftLucarioStatusUniqProcessSpecialHi;
ftLucarioStatusUniqProcessSpecialHiRush g_ftLucarioStatusUniqProcessSpecialHiRush;
ftLucarioStatusUniqProcessSpecialHiRushEnd g_ftLucarioStatusUniqProcessSpecialHiRushEnd;
ftLucarioStatusUniqProcessSpecialHiBound g_ftLucarioStatusUniqProcessSpecialHiBound;
