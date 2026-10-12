#pragma once

#include <ft/ft_status_uniq_process_air_lasso.h>
#include <types.h>

// Air lasso reach (the lasso is extended toward the target). Derives from the lasso common process.
class ftStatusUniqProcessAirLassoReach : public ftStatusUniqProcessAirLassoCommon {
public:
    virtual ~ftStatusUniqProcessAirLassoReach() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

extern ftStatusUniqProcessAirLassoReach g_ftStatusUniqProcessAirLassoReach;
