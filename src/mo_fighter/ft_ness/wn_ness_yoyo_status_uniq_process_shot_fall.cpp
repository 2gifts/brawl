#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnNessYoyoStatusUniqProcessShotFall : public soStatusUniqProcess {
public:
    virtual ~wnNessYoyoStatusUniqProcessShotFall() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnNessYoyoStatusUniqProcessShotFall g_wnNessYoyoStatusUniqProcessShotFall;

void wnNessYoyoStatusUniqProcessShotFall::initStatus(soModuleAccesser* moduleAccesser) { }
void wnNessYoyoStatusUniqProcessShotFall::execStatus(soModuleAccesser* moduleAccesser) { }
void wnNessYoyoStatusUniqProcessShotFall::execFixPos(soModuleAccesser* moduleAccesser) { }
