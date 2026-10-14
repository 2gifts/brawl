#pragma once
#include <so/status/so_status_module_impl.h>

class wnGameWatchOctopusStatusUniqProcessCommon : public soStatusUniqProcess {
public:
    virtual ~wnGameWatchOctopusStatusUniqProcessCommon() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};
extern wnGameWatchOctopusStatusUniqProcessCommon g_wnGameWatchOctopusStatusUniqProcessCommon;
