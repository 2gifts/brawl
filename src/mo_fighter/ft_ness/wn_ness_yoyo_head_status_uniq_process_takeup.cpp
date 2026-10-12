#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnNessYoyoHeadStatusUniqProcessTakeup : public soStatusUniqProcess {
public:
    virtual ~wnNessYoyoHeadStatusUniqProcessTakeup() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnNessYoyoHeadStatusUniqProcessTakeup g_wnNessYoyoHeadStatusUniqProcessTakeup;

void wnNessYoyoHeadStatusUniqProcessTakeup::initStatus(soModuleAccesser* moduleAccesser) { }
void wnNessYoyoHeadStatusUniqProcessTakeup::execStatus(soModuleAccesser* moduleAccesser) { }
void wnNessYoyoHeadStatusUniqProcessTakeup::execFixPos(soModuleAccesser* moduleAccesser) { }
