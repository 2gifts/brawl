#pragma once

#include <so/status/so_status_module_impl.h>

class soModuleAccesser;

class wnIkeSwordStatusUniqProcess : public soStatusUniqProcess {
public:
    wnIkeSwordStatusUniqProcess();
    virtual ~wnIkeSwordStatusUniqProcess();
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
};
