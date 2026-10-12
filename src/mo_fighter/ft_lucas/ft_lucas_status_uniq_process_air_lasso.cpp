#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

class soModuleAccesser;

class ftLucasStatusUniqProcessAirLasso : public soStatusUniqProcess {
public:
    virtual ~ftLucasStatusUniqProcessAirLasso() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

void ftLucasStatusUniqProcessAirLasso::initStatus(soModuleAccesser*) { }

void ftLucasStatusUniqProcessAirLasso::execFixPos(soModuleAccesser*) { }

ftLucasStatusUniqProcessAirLasso g_ftLucasStatusUniqProcessAirLasso;
