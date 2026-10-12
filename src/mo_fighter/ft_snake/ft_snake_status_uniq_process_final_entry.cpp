// Save normal draw priority and activate the Final Smash photo and depth-scale policy.
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/snake/ft_snake_status_uniq_process_final.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <so/so_module_accesser.h>
#include <so/so_photo_call_back.h>
#include <so/so_value_accesser.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
extern char lbl_122_data_774[], lbl_122_data_8C98[];
// HYPOTHESIS: the native call ABI sign-extends the final argument from a short.
extern "C" void* __dynamic_cast(void*, int, void*, void*, short);

ftSnakeStatusUniqProcessFinalEntry g_ftSnakeStatusUniqProcessFinalEntry;
void ftSnakeStatusUniqProcessFinalEntry::initStatus(soModuleAccesser* acc) {
    acc->getPostureModule().setScale(1.0f);
    void* snake = __dynamic_cast(&acc->getStageObject(), 0x3C,
        lbl_122_data_774, lbl_122_data_8C98, 0);
    if (snake) {
        reinterpret_cast<soPhotoCallBack*>(
            reinterpret_cast<unsigned char*>(snake) + 0x2D020)->addCallback();
        acc->getModelModule().setScaleZ(1.0f);
        acc->getEffectModule().ignoreScaleZ(true);
        nw4r::g3d::ScnObj* node =
            acc->getModelModule().getScnMdl(false);
        if (node) {
            const u8* savedPriority = reinterpret_cast<const u8*>(
                reinterpret_cast<unsigned char*>(node) + 0xD0);
            acc->getWorkManageModule().setInt(*savedPriority, 0x10000041);
            node->SetPriorityDrawOpa(0xFF);
        }
        acc->getModelModule().resetLayerType(gfSceneRoot::Layer_Effect_Fighter);
    }
}
void ftSnakeStatusUniqProcessFinalEntry::execFixPosCounter(soModuleAccesser* acc) {
    acc->getWorkManageModule().incInt(0x20000002);
}
void ftSnakeStatusUniqProcessFinalEntry::execFixPos(soModuleAccesser* acc) {
    soWorkManageModule& entryWork = acc->getWorkManageModule();
    int maximum = soValueAccesser::getConstantInt(acc, 24000, 0);
    int count = entryWork.getInt(0x20000002);
    if (count <= maximum) {
        soWorkManageModule& work = acc->getWorkManageModule();
        float duration = static_cast<float>(soValueAccesser::getConstantInt(acc, 24000, 0));
        float elapsed = static_cast<float>(work.getInt(0x20000002));
        float cameraScale = elapsed / duration;
        ftSnakeStatusUniqProcessFinalCommon::setCameraOffsetZ(acc, cameraScale);
    }
}
void ftSnakeStatusUniqProcessFinalEntry::exitStatus(soModuleAccesser* acc, int next) {
    ftSnakeStatusUniqProcessFinalCommon::exitStatusCommon(acc, next);
}
