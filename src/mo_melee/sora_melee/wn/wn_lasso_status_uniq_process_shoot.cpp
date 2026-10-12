#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <mt/mt_vector.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <wn/wn_lasso_status_view.h>
#include <types.h>

class wnLassoStatusUniqProcessShoot : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessShoot() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnLassoStatusUniqProcessShoot g_wnLassoStatusUniqProcessShoot;

void wnLassoStatusUniqProcessShoot::initStatus(soModuleAccesser* moduleAccesser) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    soPostureModule* posture = &moduleAccesser->getPostureModule();
    // HYPOTHESIS: the shot offset is the posture's left/right direction scaled by the scale and param 0xFA0.
    float scale = posture->getScale();
    float param = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa0, 0);
    float rate = param * scale;
    float lr = posture->getLr();
    float offset = lr * rate;
    pos.m_x = offset;
    int lifeParam;
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000000) == 0) {
        lifeParam = soValueAccesser::getConstantInt(moduleAccesser, 0x5dc1, 0);
    } else {
        lifeParam = soValueAccesser::getConstantInt(moduleAccesser, 0x5dc0, 0);
    }
    wnLassoGetAltStatus(moduleAccesser)->unkB0VecInt(&pos, lifeParam, true);
    wnLassoGetAltStatus(moduleAccesser)->unkA4Int(0);
}

void wnLassoStatusUniqProcessShoot::execFixPos(soModuleAccesser* moduleAccesser) {
    if (wnLassoGetAltStatus(moduleAccesser)->unkA8Int() == 2) {
        moduleAccesser->getStatusModule().changeStatusForce(2, moduleAccesser);
        return;
    }
    int limit;
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000000) == 0) {
        limit = soValueAccesser::getConstantInt(moduleAccesser, 0x5dc1, 0);
    } else {
        limit = soValueAccesser::getConstantInt(moduleAccesser, 0x5dc0, 0);
    }
    if (wnLassoGetAltStatus(moduleAccesser)->unk88Int() < limit) {
        return;
    }
    moduleAccesser->getStatusModule().changeStatusForce(2, moduleAccesser);
}
