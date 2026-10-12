#pragma once

#include <so/status/so_status_module_impl.h>
#include <types.h>

// Hammer status process (hammer swing / hammer hold). Names from the module map.
class ftStatusUniqProcessHammer : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessHammer() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

extern ftStatusUniqProcessHammer g_ftStatusUniqProcessHammer;
