#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

class soModuleAccesser;

class ftKoopaStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftKoopaStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser*);
};

class ftKoopaStatusUniqProcessSpecialSSquat : public soStatusUniqProcess {
public:
    virtual ~ftKoopaStatusUniqProcessSpecialSSquat() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
    virtual bool checkDamage(soModuleAccesser*, void*);
};

class ftKoopaStatusUniqProcessSpecialSJump : public soStatusUniqProcess {
public:
    virtual ~ftKoopaStatusUniqProcessSpecialSJump() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
    virtual bool checkDamage(soModuleAccesser*, void*);
};

class ftKoopaStatusUniqProcessSpecialSFall : public soStatusUniqProcess {
public:
    virtual ~ftKoopaStatusUniqProcessSpecialSFall() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
    virtual bool checkDamage(soModuleAccesser*, void*);
};

class ftKoopaStatusUniqProcessSpecialSLanding : public soStatusUniqProcess {
public:
    virtual ~ftKoopaStatusUniqProcessSpecialSLanding() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
    virtual bool checkDamage(soModuleAccesser*, void*);
};

void ftKoopaStatusUniqProcessSpecialS::initStatus(soModuleAccesser*) { }

void ftKoopaStatusUniqProcessSpecialSSquat::initStatus(soModuleAccesser*) { }
void ftKoopaStatusUniqProcessSpecialSSquat::exitStatus(soModuleAccesser*, int) { }
bool ftKoopaStatusUniqProcessSpecialSSquat::checkDamage(soModuleAccesser*, void*) { return false; }

void ftKoopaStatusUniqProcessSpecialSJump::initStatus(soModuleAccesser*) { }
void ftKoopaStatusUniqProcessSpecialSJump::execStatus(soModuleAccesser*) { }
void ftKoopaStatusUniqProcessSpecialSJump::exitStatus(soModuleAccesser*, int) { }
bool ftKoopaStatusUniqProcessSpecialSJump::checkDamage(soModuleAccesser*, void*) { return false; }

void ftKoopaStatusUniqProcessSpecialSFall::initStatus(soModuleAccesser*) { }
void ftKoopaStatusUniqProcessSpecialSFall::exitStatus(soModuleAccesser*, int) { }
bool ftKoopaStatusUniqProcessSpecialSFall::checkDamage(soModuleAccesser*, void*) { return false; }

void ftKoopaStatusUniqProcessSpecialSLanding::initStatus(soModuleAccesser*) { }
void ftKoopaStatusUniqProcessSpecialSLanding::execStatus(soModuleAccesser*) { }
void ftKoopaStatusUniqProcessSpecialSLanding::exitStatus(soModuleAccesser*, int) { }
bool ftKoopaStatusUniqProcessSpecialSLanding::checkDamage(soModuleAccesser*, void*) { return false; }

ftKoopaStatusUniqProcessSpecialS g_ftKoopaStatusUniqProcessSpecialS;
ftKoopaStatusUniqProcessSpecialSSquat g_ftKoopaStatusUniqProcessSpecialSSquat;
ftKoopaStatusUniqProcessSpecialSJump g_ftKoopaStatusUniqProcessSpecialSJump;
ftKoopaStatusUniqProcessSpecialSFall g_ftKoopaStatusUniqProcessSpecialSFall;
ftKoopaStatusUniqProcessSpecialSLanding g_ftKoopaStatusUniqProcessSpecialSLanding;
