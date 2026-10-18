#pragma once

#include <so/status/so_status_module_impl.h>

class ftStatusUniqProcessEscape : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessEscape() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};
