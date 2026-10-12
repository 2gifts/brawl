#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnNessYoyoHeadStatusUniqProcessShot : public soStatusUniqProcess {
public:
    virtual ~wnNessYoyoHeadStatusUniqProcessShot() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnNessYoyoHeadStatusUniqProcessShot g_wnNessYoyoHeadStatusUniqProcessShot;

void wnNessYoyoHeadStatusUniqProcessShot::execStatus(soModuleAccesser* moduleAccesser) { }
void wnNessYoyoHeadStatusUniqProcessShot::execFixPos(soModuleAccesser* moduleAccesser) { }
