#pragma once

#include <so/status/so_status_module_impl.h>

class ftCaptainStatusUniqProcessFalconKick : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFalconKick() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    void setEventCollisionAttack(soModuleAccesser* moduleAccesser);
};
