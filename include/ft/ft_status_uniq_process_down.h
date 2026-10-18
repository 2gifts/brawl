#pragma once

#include <so/status/so_status_module_impl.h>

// HYPOTHESIS: class shape from the shared Down status (kind 0x4A). Empty execStatus and exitStatus overrides (map names).
class ftStatusUniqProcessDown : public soStatusUniqProcess {
public:
    ftStatusUniqProcessDown() { }
    virtual ~ftStatusUniqProcessDown() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
};
