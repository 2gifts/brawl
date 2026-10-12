#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class wnLucasPKFlashStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnLucasPKFlashStatusUniqProcessMove() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

wnLucasPKFlashStatusUniqProcessMove g_wnLucasPKFlashStatusUniqProcessMove;

void wnLucasPKFlashStatusUniqProcessMove::initStatus(soModuleAccesser* moduleAccesser) {
}

void wnLucasPKFlashStatusUniqProcessMove::execStatus(soModuleAccesser* moduleAccesser) {
}

void wnLucasPKFlashStatusUniqProcessMove::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
}
