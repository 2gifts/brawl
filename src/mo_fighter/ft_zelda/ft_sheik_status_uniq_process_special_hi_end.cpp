#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftSheikStatusUniqProcessSpecialHiEnd : public soStatusUniqProcess {
public:
    virtual ~ftSheikStatusUniqProcessSpecialHiEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

// The end-of-move setup (article/status casts and the Vec2f offset reset) is not reconstructed yet.
void ftSheikStatusUniqProcessSpecialHiEnd::initStatus(soModuleAccesser*) { }
// The per-frame landing/position correction is not reconstructed yet.
void ftSheikStatusUniqProcessSpecialHiEnd::execFixPosCounter(soModuleAccesser*) { }

ftSheikStatusUniqProcessSpecialHiEnd g_ftSheikStatusUniqProcessSpecialHiEnd;
