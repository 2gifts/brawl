#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_air_lasso.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

class ftLucasStatusUniqProcessAirLasso : public ftStatusUniqProcessAirLasso {
public:
    virtual ~ftLucasStatusUniqProcessAirLasso() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

void ftLucasStatusUniqProcessAirLasso::initStatus(soModuleAccesser* moduleAccesser) {
    ftStatusUniqProcessAirLasso::initStatus(moduleAccesser);
    soMotionModule& motion = moduleAccesser->getMotionModule();
    int kind = motion.getKind();
    float frame = motion.getFrame();
    soMotionChangeParam param;
    param.m_kind = 0xEA;
    // HYPOTHESIS: frame 0.0f and rate 1.0f from the rodata constants (values not verified).
    param.m_frame = 0.0f;
    param.m_rate = 1.0f;
    param._12 = 0;
    param._13 = 0;
    param._14 = 0;
    param._15 = 0;
    motion.changeMotionRequest(&param);
    if (kind == 0x19 || kind == 0x1A) {
        // HYPOTHESIS: the second animation is the lasso reach/hang motion with rate 1.0f.
        motion.add2ndAnimChr(frame, 1.0f, kind, false);
        motion.setWeight(1.0f);
        moduleAccesser->getKineticModule().changeKinetic(0x4F, moduleAccesser);
    } else {
        moduleAccesser->getKineticModule().changeKinetic(0x13, moduleAccesser);
    }
}

void ftLucasStatusUniqProcessAirLasso::execFixPos(soModuleAccesser* moduleAccesser) {
    soMotionModule& motion = moduleAccesser->getMotionModule();
    if (motion.is2ndEnd()) {
        moduleAccesser->getKineticModule().changeKinetic(0x13, moduleAccesser);
        motion.remove2ndAnimChr();
    }
}

ftLucasStatusUniqProcessAirLasso g_ftLucasStatusUniqProcessAirLasso;
