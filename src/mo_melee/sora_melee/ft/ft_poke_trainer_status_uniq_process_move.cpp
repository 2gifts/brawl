#include <mt/mt_prng.h>
#include <so/motion/so_motion_change_param.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Pokemon Trainer: Move (the trainer's move-to-target status between the two Pokemon).
class ftPokeTrainerStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~ftPokeTrainerStatusUniqProcessMove() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftPokeTrainerStatusUniqProcessMove g_ftPokeTrainerStatusUniqProcessMove;

// Motion-blend constants from module rodata (lbl_27_rodata_7C58).
static const float kMoveTable[3] = { -1.0f, 1.0f, 0.0f };

void ftPokeTrainerStatusUniqProcessMove::initStatus(soModuleAccesser* moduleAccesser) {
    const float* table = kMoveTable;
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    float threshold = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa5, 0);
    float a = workManage.getFloat(0x1100001e);
    float choice;
    if (a < threshold) {
        float x = workManage.getFloat(0x1100001f);
        choice = (table[0] == x) ? table[0] : table[1];
    } else {
        float x = workManage.getFloat(0x1100001f);
        if (table[0] == x) {
            choice = (randi(3) >= 2) ? table[1] : table[0];
        } else {
            choice = (randi(3) >= 2) ? table[0] : table[1];
        }
    }
    if (choice == table[1]) {
        soMotionChangeParam param;
        param.m_kind = 0x1d4;
        param.m_frame = table[2];
        param.m_rate = table[1];
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
    } else {
        soMotionChangeParam param;
        param.m_kind = 0x1d5;
        param.m_frame = table[2];
        param.m_rate = table[1];
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
    }
}

void ftPokeTrainerStatusUniqProcessMove::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftPokeTrainerStatusUniqProcessMove::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftPokeTrainerStatusUniqProcessMove::exitStatus(soModuleAccesser* moduleAccesser, int) {
}
