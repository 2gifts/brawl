#pragma once

#include <ft/ft_status_uniq_process_air_lasso.h>
#include <types.h>

// Air lasso hang (the fighter hangs from the lasso). Derives from the lasso common process.
class ftStatusUniqProcessAirLassoHang : public ftStatusUniqProcessAirLassoCommon {
public:
    virtual ~ftStatusUniqProcessAirLassoHang() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

extern ftStatusUniqProcessAirLassoHang g_ftStatusUniqProcessAirLassoHang;
