#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class wnToonLinkBowArrowStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnToonLinkBowArrowStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnToonLinkBowArrowStatusUniqProcess g_wnToonLinkBowArrowStatusUniqProcess;

void wnToonLinkBowArrowStatusUniqProcess::initStatus(soModuleAccesser*) { }

void wnToonLinkBowArrowStatusUniqProcess::execStatus(soModuleAccesser*) { }
