#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessSquat : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessSquat() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessSquat g_ftStatusUniqProcessSquat;

// HYPOTHESIS: 0x20000000 is the squat step counter (-1 when not yet set); 0x11 is the status kind of the squat
// being checked.
void ftStatusUniqProcessSquat::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getStatusModule().getStatusKind();
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x11:
        moduleAccesser->getWorkManageModule().setInt(-1, 0x20000000);
        break;
    }
}

void ftStatusUniqProcessSquat::execStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x11: {
        int count = moduleAccesser->getWorkManageModule().getInt(0x20000000);
        if (count > 0) {
            moduleAccesser->getWorkManageModule().setInt(count - 1, 0x20000000);
        }
        if (count == -1) {
            if (moduleAccesser->getControllerModule().getStickY() <= soValueAccesser::getConstantFloat(moduleAccesser, 0xc73, 0)) {
                if ((u8)moduleAccesser->getControllerModule().getFlickY() < soValueAccesser::getConstantInt(moduleAccesser, 0x5a06, 0)) {
                    if (moduleAccesser->getGroundModule().isPassableGround(0) == true) {
                        float frames = soValueAccesser::getConstantFloat(moduleAccesser, 0xc75, 0);
                        moduleAccesser->getWorkManageModule().setInt((int)frames, 0x20000000);
                    }
                }
            }
        }
        break;
    }
    }
}
