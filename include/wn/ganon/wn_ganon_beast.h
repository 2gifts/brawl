#pragma once

#include <so/status/so_status_module_impl.h>

class soModuleAccesser;

class wnGanonBeastStatusUniqProcessStart : public soStatusUniqProcess {
public:
    wnGanonBeastStatusUniqProcessStart();
    virtual ~wnGanonBeastStatusUniqProcessStart();
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

class wnGanonBeastStatusUniqProcessAttack : public soStatusUniqProcess {
public:
    virtual ~wnGanonBeastStatusUniqProcessAttack();
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};
