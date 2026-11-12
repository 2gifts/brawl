#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_photo.h>
#include <it/it_manager.h>
#include <so/collision/so_collision_attack_part.h>
#include <types.h>

// The scene work of Captain Falcon's Final Smash (target links, equipment, camera, stage layers, items and weapons
// hidden during the cut-in, the attack on the targets).
// TODO: the routines other than lock_photo_camera; only the unit's statics and that routine are reconstructed.
class ftCaptainFinalSubRoutine {
public:
    static void lock_photo_camera(void* lock);
};

// HYPOTHESIS: the two item generation sheet kinds that other units define the same way (no user in this unit).
static itGenSheetKind s_genSheetKindLevel0(Stages::srStageKind(0xff), 0);
static itGenSheetKind s_genSheetKindLevel1(Stages::srStageKind(0xff), 1);

// HYPOTHESIS: the attack data of the Final Smash hit on the targets (setup_abs_attack).
static soCollisionAttackData s_absAttackData;

// The photo camera keeps the frame while the cut-in plays.
void ftCaptainFinalSubRoutine::lock_photo_camera(void* lock) {
    static_cast<cmPhotoController*>(CameraController::getInstance()->getCameraController(CameraController::Photo))
        ->setFreezeMode(lock);
}
