#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnNessPKFlashStatusUniqProcessBang : public soStatusUniqProcess {
public:
    virtual ~wnNessPKFlashStatusUniqProcessBang() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnNessPKFlashStatusUniqProcessBang g_wnNessPKFlashStatusUniqProcessBang;

void wnNessPKFlashStatusUniqProcessBang::initStatus(soModuleAccesser* moduleAccesser) { }
void wnNessPKFlashStatusUniqProcessBang::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
