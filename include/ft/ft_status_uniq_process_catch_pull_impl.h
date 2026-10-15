#pragma once

#include <so/status/so_status_module_impl.h>

// Full override set of the CatchPull status. ft_status_uniq_process_catch_pull.h declares only initStatus and
// checkDamage, which is what the special-move cling statuses derive from.
class ftStatusUniqProcessCatchPull : public soStatusUniqProcess {
public:
    virtual void initStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
    // Original CatchPull vtable imports this override, rather than the generic false callback.
    virtual bool checkDamage(soModuleAccesser*, void*);
};
