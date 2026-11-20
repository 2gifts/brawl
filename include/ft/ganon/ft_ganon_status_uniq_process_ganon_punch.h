#pragma once

#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

// Ganondorf's side special (Warlock Punch). The class names are the game's own RTTI strings.
class ftGanonStatusUniqProcessGanonPunch : public soStatusUniqProcess {
public:
    virtual ~ftGanonStatusUniqProcessGanonPunch() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

// The turning variant of the punch.
class ftGanonStatusUniqProcessGanonPunchTurn : public soStatusUniqProcess {
public:
    virtual ~ftGanonStatusUniqProcessGanonPunchTurn() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};
