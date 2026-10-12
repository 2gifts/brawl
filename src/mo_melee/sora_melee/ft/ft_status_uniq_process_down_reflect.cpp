#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessDownReflect : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessDownReflect() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessDownReflect g_ftStatusUniqProcessDownReflect;

void ftStatusUniqProcessDownReflect::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessDownReflect::execStatus(soModuleAccesser* moduleAccesser) { }
