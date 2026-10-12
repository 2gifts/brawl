#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

class soModuleAccesser;

class ftLinkStatusUniqProcessSpecialBomb : public soStatusUniqProcess {
public:
    virtual ~ftLinkStatusUniqProcessSpecialBomb() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

void ftLinkStatusUniqProcessSpecialBomb::initStatus(soModuleAccesser*) { }

void ftLinkStatusUniqProcessSpecialBomb::execFixPos(soModuleAccesser*) { }

ftLinkStatusUniqProcessSpecialBomb g_ftLinkStatusUniqProcessSpecialBomb;
