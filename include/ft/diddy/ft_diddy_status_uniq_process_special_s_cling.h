#pragma once

#include <so/status/so_status_module_impl.h>

// Diddy's side-special cling; the throw from the cling (ftDiddyStatusUniqProcessSpecialSThrow) derives from it.
class ftDiddyStatusUniqProcessSpecialSCling : public soStatusUniqProcess {
public:
    virtual ~ftDiddyStatusUniqProcessSpecialSCling() { }
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* data);
};
