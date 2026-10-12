#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftWarioStatusUniqProcessSpecialNOpen : public soStatusUniqProcess {
public:
    virtual ~ftWarioStatusUniqProcessSpecialNOpen() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftWarioStatusUniqProcessSpecialNOpen g_ftWarioStatusUniqProcessSpecialNOpen;

void ftWarioStatusUniqProcessSpecialNOpen::initStatus(soModuleAccesser* moduleAccesser) { }
void ftWarioStatusUniqProcessSpecialNOpen::execFixPos(soModuleAccesser* moduleAccesser) { }
void ftWarioStatusUniqProcessSpecialNOpen::exitStatus(soModuleAccesser* moduleAccesser, int) { }
