#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/effect/so_effect_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <ec/ec_mgr.h>
#include <wn/samus/wn_samus_cshot.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSamusCShotStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnSamusCShotStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnSamusCShotStatusUniqProcess g_wnSamusCShotStatusUniqProcess;

void wnSamusCShotStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) { }

// Kills the effect whose id is held in work int 0x10000006, unless the next status is in 1..8.
void wnSamusCShotStatusUniqProcess::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus >= 9 || nextStatus < 1) {
        int effectId = moduleAccesser->getWorkManageModule().getInt(0x10000006);
        moduleAccesser->getEffectModule().kill(effectId, true, true);
    }
}

void wnSamusCShotStatusUniqProcess::execFixPos(soModuleAccesser* moduleAccesser) {
    int status = moduleAccesser->getStatusModule().getStatusKind();
    ecMgr* effectManager;
    u32 effectId = moduleAccesser->getWorkManageModule().getInt(0x10000006);
    int kind = moduleAccesser->getWorkManageModule().getInt(0x10000007);
    Vec3f offset;
    if (kind == 3) {
        offset = Vec3f(status == 0 ? -8.0f : 0.0f, 0.0f, 0.0f);
    } else {
        offset = Vec3f(status == 0 ? 0.0f : 0.0f, 0.0f, 0.0f);
    }
    effectManager = g_ecMgr;
    effectManager->setPos(effectId, &moduleAccesser->getModelModule().getNodeGlobalPosition(0, &offset, false, false));
    const float* parameters = dynamic_cast<wnSamusCShot&>(moduleAccesser->getStageObject()).getData()->unk34;
    float charge = moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    float scale = parameters[6] + charge * (parameters[7] - parameters[6]);
    scale *= moduleAccesser->getPostureModule().getScale();
    g_ecMgr->setScl(effectId, &Vec3f(scale, scale, scale));
    switch (status) {
    case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:
        moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
    }
}
