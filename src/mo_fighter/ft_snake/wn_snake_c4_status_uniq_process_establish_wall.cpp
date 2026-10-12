#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSnakeC4StatusUniqProcessEstablishWall : public soStatusUniqProcess {
public:
    virtual ~wnSnakeC4StatusUniqProcessEstablishWall() { }
    virtual void execFixCamera(soModuleAccesser* moduleAccesser);
};

wnSnakeC4StatusUniqProcessEstablishWall g_wnSnakeC4StatusUniqProcessEstablishWall;

void wnSnakeC4StatusUniqProcessEstablishWall::execFixCamera(soModuleAccesser* moduleAccesser) { }
