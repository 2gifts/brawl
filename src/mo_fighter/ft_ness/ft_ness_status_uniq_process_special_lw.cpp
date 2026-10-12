#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

// Ness's PK Thunder (special lw) start status. The body (Vec2f speed setup on the kinetic energy and the
// stop/gravity energies) is not reconstructed yet; only the class shape is in place.
class ftNessStatusUniqProcessSpecialLw : public soStatusUniqProcess {
public:
    virtual ~ftNessStatusUniqProcessSpecialLw() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

ftNessStatusUniqProcessSpecialLw g_ftNessStatusUniqProcessSpecialLw;

void ftNessStatusUniqProcessSpecialLw::initStatus(soModuleAccesser*) {}
