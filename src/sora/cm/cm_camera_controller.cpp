#include <types.h>
#include <cm/cm_camera_controller.h>
#include <gf/gf_camera.h>

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
