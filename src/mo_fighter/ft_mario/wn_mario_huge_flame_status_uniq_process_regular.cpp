#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnMarioHugeFlameStatusUniqProcessRegular : public soStatusUniqProcess {
public:
    virtual ~wnMarioHugeFlameStatusUniqProcessRegular() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnMarioHugeFlameStatusUniqProcessRegular g_wnMarioHugeFlameStatusUniqProcessRegular;

void wnMarioHugeFlameStatusUniqProcessRegular::initStatus(soModuleAccesser* moduleAccesser) { }
void wnMarioHugeFlameStatusUniqProcessRegular::execFixPos(soModuleAccesser* moduleAccesser) { }
