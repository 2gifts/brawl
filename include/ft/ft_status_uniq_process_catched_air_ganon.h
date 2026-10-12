#pragma once

#include <so/status/so_status_module_impl.h>
#include <types.h>

// Ganondorf-captured fighter in the air (CatchedAirGanon, CatchedAirFallGanon, CatchedAirEndGanon).
// Derivation is HYPOTHESIS: the three processes are independent overrides of the same status interface.
class ftStatusUniqProcessCatchedAirGanon : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCatchedAirGanon() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

class ftStatusUniqProcessCatchedAirFallGanon : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCatchedAirFallGanon() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

class ftStatusUniqProcessCatchedAirEndGanon : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCatchedAirEndGanon() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

extern ftStatusUniqProcessCatchedAirGanon g_ftStatusUniqProcessCatchedAirGanon;
extern ftStatusUniqProcessCatchedAirFallGanon g_ftStatusUniqProcessCatchedAirFallGanon;
extern ftStatusUniqProcessCatchedAirEndGanon g_ftStatusUniqProcessCatchedAirEndGanon;
