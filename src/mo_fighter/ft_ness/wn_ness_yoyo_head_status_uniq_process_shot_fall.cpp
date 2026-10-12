#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnNessYoyoHeadStatusUniqProcessShotFall : public soStatusUniqProcess {
public:
    virtual ~wnNessYoyoHeadStatusUniqProcessShotFall() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnNessYoyoHeadStatusUniqProcessShotFall g_wnNessYoyoHeadStatusUniqProcessShotFall;

void wnNessYoyoHeadStatusUniqProcessShotFall::initStatus(soModuleAccesser* moduleAccesser) { }
void wnNessYoyoHeadStatusUniqProcessShotFall::execStatus(soModuleAccesser* moduleAccesser) { }
void wnNessYoyoHeadStatusUniqProcessShotFall::execFixPos(soModuleAccesser* moduleAccesser) { }
