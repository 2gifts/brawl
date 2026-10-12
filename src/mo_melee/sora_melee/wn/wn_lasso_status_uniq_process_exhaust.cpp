#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnLassoStatusUniqProcessExhaust : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessExhaust() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnLassoStatusUniqProcessExhaust g_wnLassoStatusUniqProcessExhaust;

void wnLassoStatusUniqProcessExhaust::initStatus(soModuleAccesser* moduleAccesser) { }
void wnLassoStatusUniqProcessExhaust::execFixPos(soModuleAccesser* moduleAccesser) { }
