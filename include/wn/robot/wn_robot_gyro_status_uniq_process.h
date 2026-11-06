#pragma once

#include <so/status/so_status_module_impl.h>

// HYPOTHESIS: Gyro status processes share the standard status-process base.
class wnRobotGyroStatusUniqProcessHold : public soStatusUniqProcess {
public:
    virtual ~wnRobotGyroStatusUniqProcessHold();
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

class wnRobotGyroStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnRobotGyroStatusUniqProcessMove();
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class wnRobotGyroStatusUniqProcessStop : public soStatusUniqProcess {
public:
    virtual ~wnRobotGyroStatusUniqProcessStop();
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};
