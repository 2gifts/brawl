#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// MATCH-ONLY: the -O2,s fighter flags stop the implicit constructor inlining into the static initializer;
// the Matching neighbours build at the default -O4,p.
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_status_uniq_process_capture_pulled.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessCaptureWait : public ftStatusUniqProcessCapturePulled {
public:
    virtual ~ftStatusUniqProcessCaptureWait() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessCaptureWait g_ftStatusUniqProcessCaptureWait;

void ftStatusUniqProcessCaptureWait::initStatus(soModuleAccesser* moduleAccesser) {
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    // HYPOTHESIS: byte 5 is the soKineticEnergy suspend byte; bit 0x80 is an unnamed flag set here.
    *((u8*)&stop + 5) |= 0x80;
    int kind = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    // Capture-state ids 0x7e..0x80 fold to 0x7f, 0x81..0x83 fold to 0x82.
    if (kind >= 0x7e && kind <= 0x80) {
        kind = 0x7f;
    } else if (kind >= 0x81 && kind <= 0x83) {
        kind = 0x82;
    }
    moduleAccesser->getWorkManageModule().setInt(kind, 0x20000000);
    moduleAccesser->getGroundModule().setCorrectIgnoreSlope(1, 0);
    if (moduleAccesser->getStatusModule().getPrevStatusKind(0) == 0x3d) {
        moduleAccesser->getControllerModule().setRumble(4, 0, true, -1);
    }
}

void ftStatusUniqProcessCaptureWait::execStatus(soModuleAccesser* moduleAccesser) {
    execStatusCommon(moduleAccesser, 0x7f, 0x82, 0x20000000);
}

// TODO: execFixPos (300 bytes) is not decoded.
void ftStatusUniqProcessCaptureWait::execFixPos(soModuleAccesser*) {}
