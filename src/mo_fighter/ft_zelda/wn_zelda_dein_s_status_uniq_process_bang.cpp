#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnZeldaDeinSStatusUniqProcessBang : public soStatusUniqProcess {
public:
    virtual ~wnZeldaDeinSStatusUniqProcessBang() { }
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnZeldaDeinSStatusUniqProcessBang g_wnZeldaDeinSStatusUniqProcessBang;

void wnZeldaDeinSStatusUniqProcessBang::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
