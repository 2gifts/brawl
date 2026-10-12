// Shared Final Smash camera distance and restoration of the fighter render state.
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/snake/ft_snake_status_uniq_process_final.h>
#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_ai.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <gf/gf_camera.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_photo_call_back.h>
#include <so/so_value_accesser.h>
#include <so/so_world.h>
#include <math.h>

extern char lbl_122_data_774[], lbl_122_data_8C98[];
extern float lbl_27_rodata_EF8; // Shared 30-degree reference camera angle.
// HYPOTHESIS: the native call ABI sign-extends the final argument from a short.
extern "C" void* __dynamic_cast(void*, int, void*, void*, short);

void ftSnakeStatusUniqProcessFinalCommon::setCameraOffsetZ(
    soModuleAccesser* acc, float scale) {
    cmAIController* ai = g_cmAIController;
    gfCamera* camera = CameraController::getInstance()->getCameraInternal();
    if (ai || camera) {
        scale *= soValueAccesser::getConstantFloat(acc, 0xFB1, 0);
        float reference = static_cast<float>(tan(0.017453292f * lbl_27_rodata_EF8));
        float tangent = static_cast<float>(tan(camera->unkD0));
        float correction = tangent <= 0.0f ? 1.0f : reference / tangent;
        scale *= correction;
        ai->setUserDistMin(scale);
    }
}

void ftSnakeStatusUniqProcessFinalCommon::exitStatusCommon(
    soModuleAccesser* acc, int next) {
    if (next < 0x133 && next >= 0x131) return;
    nw4r::g3d::ScnObj* node = acc->getModelModule().getScnMdl(false);
    if (node)
        node->SetPriorityDrawOpa(acc->getWorkManageModule().getInt(0x10000041));
    int ownerId = acc->getWorkManageModule().getInt(0x10000000);
    ftOwner* owner = ftManager::getInstance()->getOwner(ownerId);
    bool spycloak = owner->getSpycloak();
    soModelModule& model = acc->getModelModule();
    model.resetLayerType(spycloak == true
        ? gfSceneRoot::Layer_Effect_Fighter : gfSceneRoot::Layer_Ground_And_Chara);
    void* snake = __dynamic_cast(&acc->getStageObject(), 0x3C,
        lbl_122_data_774, lbl_122_data_8C98, 0);
    if (snake) {
        // Native Snake owns its Final Smash photo callback at this offset.
        reinterpret_cast<soPhotoCallBack*>(
            reinterpret_cast<unsigned char*>(snake) + 0x2D020)->removeCallBack();
        float scaleZ = g_soWorld->m_8;
        acc->getModelModule().setScaleZ(scaleZ);
        acc->getEffectModule().ignoreScaleZ(false);
    }
}
