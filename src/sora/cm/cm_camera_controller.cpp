#include <cm/cm_camera_controller.h>

extern "C" void fn_800A6BF0(gfCameraController* controller);
extern "C" void fn_800A6B78(gfCameraController* controller);
extern "C" void fn_800A6624(gfCameraController* controller);
extern "C" void fn_800A3EE8(gfCameraController* controller);
extern "C" void fn_800A59C4(gfCameraController* controller);
extern "C" void fn_8009FCF4(gfCameraController* controller, float value);

class cmMenuFixedController : public gfCameraController {
public:
    void init();
};

void CameraController::changeInput(Kind cameraControllerKind)
{
    if (m_currentControllerKind == AnimationLocked && *(int*)((char*)m_animationController + 0x20) == 0) {
        return;
    }
    if (m_currentControllerKind == Photo && cameraControllerKind != Photo) {
        fn_800A6BF0(m_photoController);
    }

    bool flag = true;
    if (m_currentControllerKind == Demo || cameraControllerKind == Demo) {
        if (*(int*)&unk168[0x20] > 0) {
            flag = false;
        }
    }
    if (flag && cameraControllerKind != Tech && cameraControllerKind != Demo) {
        gfCameraManager* manager = gfCameraManager::getManager();
        if (manager != nullptr) {
            gfCamera& camera = manager->m_cameras[0];
            Vec3f zero(0.0f, 0.0f, 0.0f);
            camera.m_0x9C = zero;
            *(Vec3f*)&camera._0xA8[12] = zero;
        }
    }
    if (flag && (cameraControllerKind == AI || cameraControllerKind == Demo)) {
        if (m_currentControllerKind == Demo || m_currentControllerKind == Tech) {
            fn_8009FCF4(m_aiController, 10.0f);
        }
    }

    m_currentControllerKind = cameraControllerKind;
    *(float*)&unk168[0x1C] = 0.0f;
    m_currentController = getCameraController(cameraControllerKind);

    // MATCH-ONLY: empty cases list every kind so the compiler emits the 0-14 jump table
    switch (m_currentControllerKind) {
    case None:
    case Animation:
    case AI:
        break;
    case Adventure:
        fn_800A59C4(m_adventureController);
        break;
    case AdventureFree:
    case AnimationLocked:
        break;
    case FighterTarget:
        fn_800A3EE8(m_fighterTargetController);
        break;
    case MenuPad:
        fn_800A6624(m_menuPadController);
        break;
    case MenuFixed:
        static_cast<cmMenuFixedController*>(m_menuFixedController)->init();
        break;
    case Photo:
        fn_800A6B78(m_photoController);
        break;
    case Tech:
    case MeleeFixed:
    case Demo:
    case Default:
        break;
    }
}
