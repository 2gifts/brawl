#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>
#include <ft/ft_common_data_accesser.h>

// Ness's PSI Magnet attack (special hi attack). Only the class shape is in place; the status bodies are not
// reconstructed yet.
class ftNessStatusUniqProcessSpecialHiAttack : public soStatusUniqProcess {
public:
    virtual ~ftNessStatusUniqProcessSpecialHiAttack() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

ftNessStatusUniqProcessSpecialHiAttack g_ftNessStatusUniqProcessSpecialHiAttack;

void ftNessStatusUniqProcessSpecialHiAttack::initStatus(soModuleAccesser*) {}

void ftNessStatusUniqProcessSpecialHiAttack::execStatus(soModuleAccesser*) {}

void ftNessStatusUniqProcessSpecialHiAttack::execStop(soModuleAccesser* moduleAccesser) {
    execStatus(moduleAccesser);
}

void ftNessStatusUniqProcessSpecialHiAttack::execFixPosCounter(soModuleAccesser*) {}

void ftNessStatusUniqProcessSpecialHiAttack::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    moduleAccesser->getModelModule().setNodeRotateZ(5, 0.0f);
    soPostureModule& posture = moduleAccesser->getPostureModule();
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    posture.setRot(&rotation, 0);
    if (nextStatus == 0x19) {
        const float* parameters = static_cast<const float*>(g_ftCommonDataAccesser.getData(Fighter_Ness)->extendParam[2]);
        float duration = parameters[11];
        if (duration > 0.0f) {
            moduleAccesser->getWorkManageModule().setFloat(duration, 0x11000000);
        }
    }
}

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
float fn_101_129F0(float* v) { return v[0] * v[0] + v[1] * v[1]; }
} // extern "C"
