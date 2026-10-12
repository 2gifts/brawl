#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftSZerosuitStatusUniqProcessSpecialLwStep : public soStatusUniqProcess {
public:
    virtual ~ftSZerosuitStatusUniqProcessSpecialLwStep() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

// The step start (task lookup, article cast and link/hit-box setup) is not reconstructed yet.
void ftSZerosuitStatusUniqProcessSpecialLwStep::initStatus(soModuleAccesser*) { }

// The link-module teardown that runs before the flip/kick check (link queries on the link module at
// enum+0x54) is not reconstructed yet; only the common flag reset is written here.
void ftSZerosuitStatusUniqProcessSpecialLwStep::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x115 && static_cast<u32>(nextStatus - 0x11b) > 4u) {
        soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
        soPostureModule& posture = moduleAccesser->getPostureModule();
        if (workManage.isFlag(0x22000017)) {
            posture.reverseLr();
            posture.updateRotYLr();
        }
    }
}

ftSZerosuitStatusUniqProcessSpecialLwStep g_ftSZerosuitStatusUniqProcessSpecialLwStep;
