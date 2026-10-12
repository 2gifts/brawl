#pragma once

#include <ft/ft_status_uniq_process_air_lasso.h>
#include <types.h>

// Air lasso rewind (the lasso is pulled back to the fighter). Derives from the lasso common process.
class ftStatusUniqProcessAirLassoRewind : public ftStatusUniqProcessAirLassoCommon {
public:
    virtual ~ftStatusUniqProcessAirLassoRewind() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

extern ftStatusUniqProcessAirLassoRewind g_ftStatusUniqProcessAirLassoRewind;
