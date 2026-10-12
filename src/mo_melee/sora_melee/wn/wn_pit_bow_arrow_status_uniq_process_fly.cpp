#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class wnPitBowArrowStatusUniqProcessFly : public soStatusUniqProcess {
public:
    virtual ~wnPitBowArrowStatusUniqProcessFly() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnPitBowArrowStatusUniqProcessFly g_wnPitBowArrowStatusUniqProcessFly;

void wnPitBowArrowStatusUniqProcessFly::initStatus(soModuleAccesser* moduleAccesser) {
}

void wnPitBowArrowStatusUniqProcessFly::execStatus(soModuleAccesser* moduleAccesser) {
}
