#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSnakeC4StatusUniqProcessEstablishGround : public soStatusUniqProcess {
public:
    virtual ~wnSnakeC4StatusUniqProcessEstablishGround() { }
    virtual void execFixCamera(soModuleAccesser* moduleAccesser);
};

wnSnakeC4StatusUniqProcessEstablishGround g_wnSnakeC4StatusUniqProcessEstablishGround;

void wnSnakeC4StatusUniqProcessEstablishGround::execFixCamera(soModuleAccesser* moduleAccesser) { }
