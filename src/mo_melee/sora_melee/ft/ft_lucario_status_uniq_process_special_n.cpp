#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftLucarioStatusUniqProcessSpecialN : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialN() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftLucarioStatusUniqProcessSpecialN g_ftLucarioStatusUniqProcessSpecialN;

void ftLucarioStatusUniqProcessSpecialN::exitStatus(soModuleAccesser*, int) { }
