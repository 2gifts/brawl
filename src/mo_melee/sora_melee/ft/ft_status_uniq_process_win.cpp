#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessWin : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessWin() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);

    void generateArticleWin(soModuleAccesser* moduleAccesser);
    void removeArticleWin(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessWin g_ftStatusUniqProcessWin;

void ftStatusUniqProcessWin::initStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessWin::execStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessWin::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) { }

void ftStatusUniqProcessWin::generateArticleWin(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessWin::removeArticleWin(soModuleAccesser* moduleAccesser) { }
