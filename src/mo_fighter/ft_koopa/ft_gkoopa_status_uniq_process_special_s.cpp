#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

class soModuleAccesser;

class ftGKoopaStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftGKoopaStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser*);
};

class ftGKoopaStatusUniqProcessSpecialSSquat : public soStatusUniqProcess {
public:
    virtual ~ftGKoopaStatusUniqProcessSpecialSSquat() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
    virtual bool checkDamage(soModuleAccesser*, void*);
};

class ftGKoopaStatusUniqProcessSpecialSJump : public soStatusUniqProcess {
public:
    virtual ~ftGKoopaStatusUniqProcessSpecialSJump() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
    virtual bool checkDamage(soModuleAccesser*, void*);
};

class ftGKoopaStatusUniqProcessSpecialSFall : public soStatusUniqProcess {
public:
    virtual ~ftGKoopaStatusUniqProcessSpecialSFall() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
    virtual bool checkDamage(soModuleAccesser*, void*);
};

class ftGKoopaStatusUniqProcessSpecialSLanding : public soStatusUniqProcess {
public:
    virtual ~ftGKoopaStatusUniqProcessSpecialSLanding() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
    virtual bool checkDamage(soModuleAccesser*, void*);
};

void ftGKoopaStatusUniqProcessSpecialS::initStatus(soModuleAccesser*) { }

void ftGKoopaStatusUniqProcessSpecialSSquat::initStatus(soModuleAccesser*) { }
void ftGKoopaStatusUniqProcessSpecialSSquat::exitStatus(soModuleAccesser*, int) { }
bool ftGKoopaStatusUniqProcessSpecialSSquat::checkDamage(soModuleAccesser*, void*) { return false; }

void ftGKoopaStatusUniqProcessSpecialSJump::initStatus(soModuleAccesser*) { }
void ftGKoopaStatusUniqProcessSpecialSJump::execStatus(soModuleAccesser*) { }
void ftGKoopaStatusUniqProcessSpecialSJump::exitStatus(soModuleAccesser*, int) { }
bool ftGKoopaStatusUniqProcessSpecialSJump::checkDamage(soModuleAccesser*, void*) { return false; }

void ftGKoopaStatusUniqProcessSpecialSFall::initStatus(soModuleAccesser*) { }
void ftGKoopaStatusUniqProcessSpecialSFall::exitStatus(soModuleAccesser*, int) { }
bool ftGKoopaStatusUniqProcessSpecialSFall::checkDamage(soModuleAccesser*, void*) { return false; }

void ftGKoopaStatusUniqProcessSpecialSLanding::initStatus(soModuleAccesser*) { }
void ftGKoopaStatusUniqProcessSpecialSLanding::execStatus(soModuleAccesser*) { }
void ftGKoopaStatusUniqProcessSpecialSLanding::exitStatus(soModuleAccesser*, int) { }
bool ftGKoopaStatusUniqProcessSpecialSLanding::checkDamage(soModuleAccesser*, void*) { return false; }

ftGKoopaStatusUniqProcessSpecialS g_ftGKoopaStatusUniqProcessSpecialS;
ftGKoopaStatusUniqProcessSpecialSSquat g_ftGKoopaStatusUniqProcessSpecialSSquat;
ftGKoopaStatusUniqProcessSpecialSJump g_ftGKoopaStatusUniqProcessSpecialSJump;
ftGKoopaStatusUniqProcessSpecialSFall g_ftGKoopaStatusUniqProcessSpecialSFall;
ftGKoopaStatusUniqProcessSpecialSLanding g_ftGKoopaStatusUniqProcessSpecialSLanding;
