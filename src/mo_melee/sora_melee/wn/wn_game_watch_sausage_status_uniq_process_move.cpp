#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

class wnGameWatchSausageStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnGameWatchSausageStatusUniqProcessMove() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnGameWatchSausageStatusUniqProcessMove g_wnGameWatchSausageStatusUniqProcessMove;

void wnGameWatchSausageStatusUniqProcessMove::execStatus(soModuleAccesser* moduleAccesser) { }
