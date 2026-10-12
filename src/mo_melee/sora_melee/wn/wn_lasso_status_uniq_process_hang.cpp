#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnLassoStatusUniqProcessHang : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessHang() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnLassoStatusUniqProcessHang g_wnLassoStatusUniqProcessHang;

void wnLassoStatusUniqProcessHang::initStatus(soModuleAccesser* moduleAccesser) { }
void wnLassoStatusUniqProcessHang::execStatus(soModuleAccesser* moduleAccesser) { }
