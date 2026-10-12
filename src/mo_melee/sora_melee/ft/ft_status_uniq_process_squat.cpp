#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_posture_view.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessSquat : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessSquat() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessSquat g_ftStatusUniqProcessSquat;

// MATCH-ONLY: the ground module query at vtable offset 0x1bc is not declared in the shared header; the call is
// made through the vtable slot directly (same call sequence as the target).
typedef int (*ftSquatGroundQueryFn)(void* self, u32 arg);
static bool squatGroundQuery(soModuleAccesser* moduleAccesser) {
    void* ground = moduleAccesser->m_enumerationStart->m_groundModule;
    ftSquatGroundQueryFn fn = *reinterpret_cast<ftSquatGroundQueryFn*>(reinterpret_cast<char*>(*static_cast<void**>(ground)) + 0x1bc);
    return fn(ground, 0) == 1;
}

// HYPOTHESIS: 0x20000000 is the squat step counter (-1 when not yet set); 0x11 is the status kind of the squat
// being checked. The helper below is defined in this unit and used by the squat walk.
extern "C" float soModuleAccesser__SquatWalk_GetStickXRate(soModuleAccesser* moduleAccesser);

// HYPOTHESIS: constants from lbl_27_rodata_20C0 (not verified): [0] 0.0f, [4] and [8] end rates, [12] a width.
static const float s_squatRate[4] = {0.0f, 1.0f, 0.0f, 1.0f};

void ftStatusUniqProcessSquat::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getStatusModule().getStatusKind();
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x11) {
        moduleAccesser->getWorkManageModule().setInt(-1, 0x20000000);
    }
}

void ftStatusUniqProcessSquat::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getStatusKind() != 0x11) {
        return;
    }
    int count = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    if (count > 0) {
        moduleAccesser->getWorkManageModule().setInt(count - 1, 0x20000000);
    }
    if (count == -1) {
        float stickLimit = soValueAccesser::getConstantFloat(moduleAccesser, 0xc73, 0);
        if (moduleAccesser->getControllerModule().getStickY() > stickLimit) {
            int flickLimit = soValueAccesser::getConstantInt(moduleAccesser, 0x5a06, 0);
            if ((u8)moduleAccesser->getControllerModule().getFlickY() < flickLimit) {
                if (squatGroundQuery(moduleAccesser) == true) {
                    float frames = soValueAccesser::getConstantFloat(moduleAccesser, 0xc75, 0);
                    moduleAccesser->getWorkManageModule().setInt((int)frames, 0x20000000);
                }
            }
        }
    }
}

// Squat-walk stick-rate helper (defined in this unit): picks the stick rate for the current stick value.
extern "C" float soModuleAccesser__SquatWalk_GetStickXRate(soModuleAccesser* moduleAccesser) {
    float speed = reinterpret_cast<ftStatusPostureView&>(moduleAccesser->getPostureModule()).unk2c();
    float value = speed * moduleAccesser->getControllerModule().getStickX();
    float lo = s_squatRate[0];
    float hi = s_squatRate[0];
    int kind = moduleAccesser->getStatusModule().getStatusKind();
    if (kind == 0x14) {
        lo = soValueAccesser::getConstantFloat(moduleAccesser, 0xc40, 0);
        hi = soValueAccesser::getConstantFloat(moduleAccesser, 0xc41, 0);
        if (value > lo) {
            return s_squatRate[1];
        }
        if (value < hi) {
            return s_squatRate[2];
        }
    } else if (kind == 0x13) {
        lo = soValueAccesser::getConstantFloat(moduleAccesser, 0xc3e, 0);
        hi = soValueAccesser::getConstantFloat(moduleAccesser, 0xc3f, 0);
        if (value < lo) {
            return s_squatRate[1];
        }
        if (value > hi) {
            return s_squatRate[2];
        }
    }
    float span = hi - lo;
    if ((float)__fabs(span) > s_squatRate[3]) {
        return s_squatRate[1] + s_squatRate[1] * ((value - lo) / span);
    }
    return s_squatRate[2];
}
