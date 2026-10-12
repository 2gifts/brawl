#include <ft/fighter.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: this translation unit holds Ganondorf's special-S and air special-S status processes. Only the class
// shapes, the vtable order and the static instances are reconstructed; the bodies are not reconstructed yet.

class ftGanonStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftGanonStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

class ftGanonStatusUniqProcessSpecialSCatch : public soStatusUniqProcess {
public:
    virtual ~ftGanonStatusUniqProcessSpecialSCatch() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* damage);
};

class ftGanonStatusUniqProcessSpecialAirSCatch : public soStatusUniqProcess {
public:
    virtual ~ftGanonStatusUniqProcessSpecialAirSCatch() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* damage);
};

class ftGanonStatusUniqProcessSpecialAirSFall : public soStatusUniqProcess {
public:
    virtual ~ftGanonStatusUniqProcessSpecialAirSFall() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

class ftGanonStatusUniqProcessSpecialAirSEnd : public soStatusUniqProcess {
public:
    virtual ~ftGanonStatusUniqProcessSpecialAirSEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftGanonStatusUniqProcessSpecialS::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftGanonStatusUniqProcessSpecialS::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftGanonStatusUniqProcessSpecialS::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
}

void ftGanonStatusUniqProcessSpecialSCatch::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftGanonStatusUniqProcessSpecialSCatch::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftGanonStatusUniqProcessSpecialSCatch::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftGanonStatusUniqProcessSpecialSCatch::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
}

bool ftGanonStatusUniqProcessSpecialSCatch::checkDamage(soModuleAccesser* moduleAccesser, void* damage) {
    return false;
}

void ftGanonStatusUniqProcessSpecialAirSCatch::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftGanonStatusUniqProcessSpecialAirSCatch::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftGanonStatusUniqProcessSpecialAirSCatch::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
}

bool ftGanonStatusUniqProcessSpecialAirSCatch::checkDamage(soModuleAccesser* moduleAccesser, void* damage) {
    return false;
}

void ftGanonStatusUniqProcessSpecialAirSFall::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftGanonStatusUniqProcessSpecialAirSFall::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftGanonStatusUniqProcessSpecialAirSFall::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
}

void ftGanonStatusUniqProcessSpecialAirSEnd::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftGanonStatusUniqProcessSpecialAirSEnd::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
}

ftGanonStatusUniqProcessSpecialS g_ftGanonStatusUniqProcessSpecialS;
ftGanonStatusUniqProcessSpecialSCatch g_ftGanonStatusUniqProcessSpecialSCatch;
ftGanonStatusUniqProcessSpecialAirSCatch g_ftGanonStatusUniqProcessSpecialAirSCatch;
ftGanonStatusUniqProcessSpecialAirSFall g_ftGanonStatusUniqProcessSpecialAirSFall;
ftGanonStatusUniqProcessSpecialAirSEnd g_ftGanonStatusUniqProcessSpecialAirSEnd;
