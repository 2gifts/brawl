#pragma once
#include <so/status/so_status_module_impl.h>

// The native full RTTI chain is FinalSwim -> soStatusUniqProcess.
// FinalSwim also supplies the inherited position-fix callback.
class ftGameWatchStatusUniqProcessFinalSwim : public soStatusUniqProcess {
public:
    virtual ~ftGameWatchStatusUniqProcessFinalSwim() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};
