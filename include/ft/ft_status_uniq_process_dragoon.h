#pragma once

#include <so/status/so_status_module_impl.h>
#include <types.h>

// Status process of the dragoon unit (names from the module map).
class ftStatusUniqProcessDragoon : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessDragoon() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

extern ftStatusUniqProcessDragoon g_ftStatusUniqProcessDragoon;
