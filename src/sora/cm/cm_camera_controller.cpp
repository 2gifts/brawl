#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_adventure.h>
#include <cm/cm_controller_demo.h>
#include <cm/cm_controller_fig_disp1.h>
#include <cm/cm_controller_photo.h>
#include <cm/cm_controller_tech.h>
#include <gf/gf_camera.h>
#include <sr/sr_common.h>

extern "C" void fn_800A6BF0(gfCameraController* controller);
extern "C" void fn_800A6B78(gfCameraController* controller);
extern "C" void fn_800A6624(gfCameraController* controller);
extern "C" void fn_800A3EE8(gfCameraController* controller);
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
            camera.m_rotationOffset = zero;
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
        static_cast<cmAdventureController*>(m_adventureController)->initialize();
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
// MATCH-ONLY: changeInput calls this with bl instead of inlining it
#pragma dont_inline on
gfCameraController* CameraController::getCameraController(Kind kind)
{
    switch (kind) {
    case Animation:
    case AnimationLocked:
        return m_animationController;
    case AI:
        return m_aiController;
    case Adventure:
        return m_adventureController;
    case AdventureFree:
        return m_adventureFreeController;
    case FighterTarget:
        return m_fighterTargetController;
    case Photo:
        return m_photoController;
    case MenuPad:
        return m_menuPadController;
    case MenuFixed:
        return m_menuFixedController;
    case FigDisp1:
        return m_figDisp1Controller;
    case Tech:
        return m_techController;
    case MeleeFixed:
        return m_meleeFixedController;
    case Demo:
        return m_demoController;
    case Default:
        return m_defaultController;
    }
}
#pragma dont_inline reset

gfCamera* CameraController::getCameraInternal()
{
    switch (m_currentControllerKind) {
    case Adventure:
    case AdventureFree:
    case FighterTarget:
        return gfCameraManager::getManager()->getCamera(0);
    default:
        return gfCameraManager::getManager()->getCamera(1);
    }
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
// HYPOTHESIS: the CameraController instance pointer (the map leaves this .sbss pair unnamed).
CameraController* lbl_805A0278[2] __attribute__((aligned(4))) = { 0 };

CameraController* CameraController::create()
{
    if (!lbl_805A0278[0]) {
        lbl_805A0278[0] = new (Heaps::System) CameraController();
        lbl_805A0278[0]->init();
    }
    return lbl_805A0278[0];
}

cmPhotoController::~cmPhotoController()
{
}

cmFigDisp1Controller::~cmFigDisp1Controller()
{
}

bool CameraController::isForceEnableCamControll3D()
{
    if (m_currentControllerKind == AI || m_currentControllerKind == MeleeFixed) {
        return false;
    }
    if (m_currentControllerKind == Demo) {
        return static_cast<cmDemoController*>(m_demoController)->isForceCamControl3D();
    }
    return true;
}

unsigned int CameraController::isSubjectSettingFlags()
{
    unsigned int flags = 0;
    if (m_currentControllerKind == Demo && m_demoController != nullptr) {
        switch (static_cast<cmDemoController*>(m_demoController)->m_targetType) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            flags = 1;
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            flags = 4;
            break;
        case 9:
            flags = 3;
            break;
        }
    }
    return flags | 1;
}

void CameraController::clearDisablePlayerNo(int playerNo)
{
    if ((u32)playerNo < 7) {
        unk45 &= ~(u8)(1 << playerNo);
    }
}

void CameraController::startZoomIn(cmSubject* subject, u16 frame, const Vec2f& rotation, const Vec2f& targetRotation, float zoom, float distance)
{
    cmTechController* controller = static_cast<cmTechController*>(lbl_805A0278[0]->m_techController);
    controller->start(subject, frame, rotation, targetRotation, zoom, distance);
    controller->unk10 = m_currentControllerKind;
    changeInput(Tech);
}

void CameraController::setDisableZoomStart()
{
    if (unk188 == 0 && m_currentControllerKind == Demo && m_aiController != nullptr) {
        fn_8009FCF4(m_aiController, 10.0f);
    }
    ++unk188;
}

void CameraController::setDisableZoomEnd()
{
    if (unk188 > 0) {
        --unk188;
    }
}

bool CameraController::isDisableZoom()
{
    return unk188 > 0;
}

void CameraController::cancelZoomIn(cmSubject* subject)
{
    if (m_currentControllerKind == Tech) {
        cmTechController* controller = static_cast<cmTechController*>(lbl_805A0278[0]->m_techController);
        if (!subject || subject == controller->m_target) {
            controller->resetTarget();
            changeInput(static_cast<Kind>(controller->unk10));
        }
    }
}
