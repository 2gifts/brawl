#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnPokeLizardonRockStatusUniqProcessCrushed : public soStatusUniqProcess {
public:
    virtual ~wnPokeLizardonRockStatusUniqProcessCrushed() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnPokeLizardonRockStatusUniqProcessCrushed g_wnPokeLizardonRockStatusUniqProcessCrushed;

void wnPokeLizardonRockStatusUniqProcessCrushed::initStatus(soModuleAccesser* moduleAccesser) { }
void wnPokeLizardonRockStatusUniqProcessCrushed::execStatus(soModuleAccesser* moduleAccesser) { }
