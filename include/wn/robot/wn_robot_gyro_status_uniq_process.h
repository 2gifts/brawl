#pragma once

#include <so/status/so_status_module_impl.h>

// HYPOTHESIS: Gyro status processes share the standard status-process base.
class wnRobotGyroStatusUniqProcessHold : public soStatusUniqProcess {
public:
    virtual ~wnRobotGyroStatusUniqProcessHold();
};

class wnRobotGyroStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnRobotGyroStatusUniqProcessMove();
};

class wnRobotGyroStatusUniqProcessStop : public soStatusUniqProcess {
public:
    virtual ~wnRobotGyroStatusUniqProcessStop();
};
