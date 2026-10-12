#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftMetaknightStatusUniqProcessSpecialNSpin : public soStatusUniqProcess {
public:
    virtual ~ftMetaknightStatusUniqProcessSpecialNSpin() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftMetaknightStatusUniqProcessSpecialNSpin g_ftMetaknightStatusUniqProcessSpecialNSpin;

void ftMetaknightStatusUniqProcessSpecialNSpin::execStatus(soModuleAccesser* moduleAccesser) {
}
