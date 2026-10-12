#pragma once

#include <so/status/so_status_module_impl.h>
#include <types.h>

// Status process of the koopa dived (Koopa dived / Bowser-style dive) unit (names from the module map).
class ftStatusUniqProcessKoopaDived : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessKoopaDived() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

extern ftStatusUniqProcessKoopaDived g_ftStatusUniqProcessKoopaDived;
