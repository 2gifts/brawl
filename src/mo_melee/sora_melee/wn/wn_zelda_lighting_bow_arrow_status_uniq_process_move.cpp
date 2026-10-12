#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnZeldaLightingBowArrowStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnZeldaLightingBowArrowStatusUniqProcessMove() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnZeldaLightingBowArrowStatusUniqProcessMove g_wnZeldaLightingBowArrowStatusUniqProcessMove;

void wnZeldaLightingBowArrowStatusUniqProcessMove::initStatus(soModuleAccesser* moduleAccesser) { }
void wnZeldaLightingBowArrowStatusUniqProcessMove::execStatus(soModuleAccesser* moduleAccesser) { }
