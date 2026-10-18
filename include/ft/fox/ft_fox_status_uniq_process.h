#pragma once

#include <so/status/so_status_module_impl.h>

// The special-move status processes of Fox that Falco shares (their code is in sora_melee; Wolf has its own reflector turn).
// HYPOTHESIS: class shapes from the module map; see src/mo_melee/sora_melee/ft/ft_fox_status_uniq_process_*.cpp.
class ftFoxStatusUniqProcessBlaster : public soStatusUniqProcess {
public:
    virtual ~ftFoxStatusUniqProcessBlaster() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftFoxStatusUniqProcessIllusion : public soStatusUniqProcess {
public:
    virtual ~ftFoxStatusUniqProcessIllusion() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    void stepChanged(soModuleAccesser* moduleAccesser);
};

class ftFoxStatusUniqProcessFire : public soStatusUniqProcess {
public:
    virtual ~ftFoxStatusUniqProcessFire() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

class ftFoxStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftFoxStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

extern ftFoxStatusUniqProcessBlaster g_ftFoxStatusUniqProcessBlaster;
extern ftFoxStatusUniqProcessIllusion g_ftFoxStatusUniqProcessIllusion;
extern ftFoxStatusUniqProcessFire g_ftFoxStatusUniqProcessFire;
extern ftFoxStatusUniqProcessFinal g_ftFoxStatusUniqProcessFinal;
