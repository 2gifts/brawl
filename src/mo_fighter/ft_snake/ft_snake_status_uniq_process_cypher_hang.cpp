#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftSnakeStatusUniqProcessCypherHang : public soStatusUniqProcess {
public:
    virtual ~ftSnakeStatusUniqProcessCypherHang() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

// The hang-state setup and per-frame cypher checks are not reconstructed yet.
void ftSnakeStatusUniqProcessCypherHang::initStatus(soModuleAccesser*) { }
void ftSnakeStatusUniqProcessCypherHang::execStatus(soModuleAccesser*) { }
void ftSnakeStatusUniqProcessCypherHang::exitStatus(soModuleAccesser*, int) { }

ftSnakeStatusUniqProcessCypherHang g_ftSnakeStatusUniqProcessCypherHang;
