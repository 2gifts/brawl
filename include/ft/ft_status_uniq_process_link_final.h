#pragma once

#include <so/status/so_status_module_impl.h>
#include <types.h>

// Link final status (the fighter is linked to the final smash target). Names from the module map.
class ftStatusUniqProcessLinkFinal : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessLinkFinal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

extern ftStatusUniqProcessLinkFinal g_ftStatusUniqProcessLinkFinal;
