#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/turn/so_turn_module.h>
#include <types.h>

// Local view of the posture module: the shared header does not declare the virtual used here (vtable 0x2c).
class ftFlyPostureView {
public:
    virtual ~ftFlyPostureView() { }
    virtual void slot02();
    virtual void slot03();
    virtual void slot04();
    virtual void slot05();
    virtual void slot06();
    virtual void slot07();
    virtual void slot08();
    virtual void slot09();
    virtual float slot0a();
};

class ftStatusUniqProcessFly : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessFly() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessFly g_ftStatusUniqProcessFly;

// HYPOTHESIS: the sound id and work flag names are guesses from how they are used.
void ftStatusUniqProcessFly::initStatus(soModuleAccesser* moduleAccesser) {
    float posture = reinterpret_cast<ftFlyPostureView&>(moduleAccesser->getPostureModule()).slot0a();
    float stick = moduleAccesser->getControllerModule().getStickX();
    float threshold = soValueAccesser::getConstantFloat(moduleAccesser, 0xc31, 0);
    if (stick * posture < threshold) {
        void* data = soValueAccesser::getConstantIndefinite(moduleAccesser, 0xa80a, 0);
        soTurnModule* turn = static_cast<soTurnModule*>(moduleAccesser->m_enumerationStart->m_turnModule);
        turn->set(reinterpret_cast<const soTurnData*>(static_cast<u8*>(data) + 0x14), true, false, posture);
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000013) == true) {
        moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1fac), false, false, 0);
    }
}

void ftStatusUniqProcessFly::execStatus(soModuleAccesser*) {}
