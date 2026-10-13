#pragma once

#include <so/status/so_status_module_impl.h>

class ftCaptainStatusUniqProcessSpecialHiCling : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessSpecialHiCling() { }
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* data);
    virtual bool onChangeLr(soModuleAccesser* moduleAccesser, float, float);
};
