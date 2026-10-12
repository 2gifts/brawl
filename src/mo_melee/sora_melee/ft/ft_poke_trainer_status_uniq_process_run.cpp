#include <so/motion/so_motion_change_param.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftPokeTrainerStatusUniqProcessRun : public soStatusUniqProcess {
public:
    virtual ~ftPokeTrainerStatusUniqProcessRun() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftPokeTrainerStatusUniqProcessRun g_ftPokeTrainerStatusUniqProcessRun;

// Constants from module rodata (lbl_27_rodata_7C48).
static const float kRunTable[3] = { -1.0f, 1.0f, 0.0f };

void ftPokeTrainerStatusUniqProcessRun::initStatus(soModuleAccesser* moduleAccesser) {
    const float* table = kRunTable;
    float value = moduleAccesser->getWorkManageModule().getFloat(0x11000016);
    float choice = (value < table[2]) ? table[0] : table[1];
    moduleAccesser->getWorkManageModule().setFloat(choice, 0x11000019);

    soMotionChangeParam param;
    param.m_kind = (table[0] == choice) ? 0x1d7 : 0x1d6;
    param.m_frame = table[2];
    param.m_rate = table[1];
    param._12 = 0;
    param._13 = 0;
    param._14 = 0;
    param._15 = 0;
    moduleAccesser->getMotionModule().changeMotionRequest(&param);

    moduleAccesser->getWorkManageModule().setFloat(table[2], 0x11000018);
}

void ftPokeTrainerStatusUniqProcessRun::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftPokeTrainerStatusUniqProcessRun::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftPokeTrainerStatusUniqProcessRun::exitStatus(soModuleAccesser* moduleAccesser, int) {
}
