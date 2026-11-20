#define IT_CUSTOMIZER_UNIT
// The unit also instantiates the soArrayList<itFigureArchive*, 64> size/capacity members (weak copies).
#define SO_ARRAY_LIST_EXTERNAL_INDEX_MEMBERS
#define SO_ARRAY_LIST_EXTERNAL_isFull
#define SO_ARRAY_LIST_EXTERNAL_shiftFreeArrayIndex
#define SO_ARRAY_LIST_EXTERNAL_getArrayIndex
#define SO_ARRAY_LIST_EXTERNAL_insertSub
#define SO_ARRAY_LIST_EXTERNAL_eraseSub
#include <so/so_array.h>
#include <it/item.h>
#include <types.h>
#include <it/it_manager.h>
#include <cm/cm_camera_controller.h>

// The item customizers (the per-item hooks of BaseItem): this unit holds their small overrides and destructors; the
// larger hooks stay in the module blob for now (declared so the vtables have their slots).
// TODO: the remaining hooks, the non-virtual helpers and itPhotoCallBack with the static initializer.

itResetsanCustomizer::~itResetsanCustomizer() { }

itParam::KineticFlag itResetsanCustomizer::getKineticFlags(BaseItem* item) { return static_cast<itParam::KineticFlag>(0x18); }

itBananaCustomizer::~itBananaCustomizer() { }

itSmashBallCustomizer::~itSmashBallCustomizer() { }

void itSmashBallCustomizer::onUpdateAiDir(BaseItem* item, Vec2f*, Vec2f*, Vec2f*, Vec2f*) { }

bool itSmashBallCustomizer::isUseSpeedDamage(BaseItem* item) { return false; }

itParam::KineticFlag itSmashBallCustomizer::getKineticFlags(BaseItem* item) { return static_cast<itParam::KineticFlag>(0x81); }

itKatanaAnaCustomizer::~itKatanaAnaCustomizer() { }

itSonansCustomizer::~itSonansCustomizer() { }

bool itSonansCustomizer::isUseSpeedDamage(BaseItem* item) { return false; }

itParam::KineticFlag itSonansCustomizer::getKineticFlags(BaseItem* item) { return static_cast<itParam::KineticFlag>(0x18); }

itSandbagCustomizer::~itSandbagCustomizer() { }

void itSandbagCustomizer::onReset(BaseItem* item) { }

void itSandbagCustomizer::onNotifyEventLink(BaseItem* item, void* unk1, soModuleAccesser* moduleAccesser, StageObject*, int unk4) { }

bool itSandbagCustomizer::onNotifyEventCaptureStatus(BaseItem* item) { return false; }

bool itSandbagCustomizer::isUseTurnDamage(BaseItem* item) { return true; }

itParam::KineticFlag itSandbagCustomizer::getKineticFlags(BaseItem* item) { return static_cast<itParam::KineticFlag>(0x2); }

itSuperScopeCustomizer::~itSuperScopeCustomizer() { }

itFireFlowerCustomizer::~itFireFlowerCustomizer() { }

itRayGunCustomizer::~itRayGunCustomizer() { }

itRayGunShotCustomizer::~itRayGunShotCustomizer() { }

itClackerCustomizer::~itClackerCustomizer() { }

itRipStickCustomizer::~itRipStickCustomizer() { }

itStarRodCustomizer::~itStarRodCustomizer() { }

itBeamSwordCustomizer::~itBeamSwordCustomizer() { }

itFigureCustomizer::~itFigureCustomizer() { }

void itFigureCustomizer::onConstruct(BaseItem* item) { }

itNintendogsHiCustomizer::~itNintendogsHiCustomizer() { }

bool itNintendogsHiCustomizer::isUseScaleZ(BaseItem* item) { return false; }

bool itNintendogsHiCustomizer::isSafe(BaseItem* item) { return false; }

itSpringCustomizer::~itSpringCustomizer() { }

itWarioBikeCustomizer::~itWarioBikeCustomizer() { }

bool itWarioBikeCustomizer::onRemoveModelConstraint(BaseItem* item, int) { return false; }

itParam::KineticFlag itWarioBikeCustomizer::getKineticFlags(BaseItem* item) { return static_cast<itParam::KineticFlag>(0x100); }

itChewingCustomizer::~itChewingCustomizer() { }

bool itChewingCustomizer::isReferenceControlerHave(BaseItem* item) { return true; }

itSmokeScreenCustomizer::~itSmokeScreenCustomizer() { }

bool itSmokeScreenCustomizer::isReferenceControlerHave(BaseItem* item) { return true; }

itSmartBombCustomizer::~itSmartBombCustomizer() { }

itMetroidCustomizer::~itMetroidCustomizer() { }

bool itMetroidCustomizer::onHave(BaseItem* item, int) { return false; }

bool itMetroidCustomizer::isReferenceControlerHave(BaseItem* item) { return true; }

itTuchCustomizer::~itTuchCustomizer() { }

itThunderCustomizer::~itThunderCustomizer() { }

itMetalBlockCustomizer::~itMetalBlockCustomizer() { }

itRobotGyroCustomizer::~itRobotGyroCustomizer() { }

itParam::KineticFlag itRobotGyroCustomizer::getKineticFlags(BaseItem* item) { return static_cast<itParam::KineticFlag>(0x4); }

itDragoonSetCustomizer::~itDragoonSetCustomizer() { }

bool itDragoonSetCustomizer::isUseScaleZ(BaseItem* item) { return false; }

bool itDragoonSetCustomizer::isConstraintHave(BaseItem* item) { return false; }

bool itDragoonSetCustomizer::isReferenceControlerHave(BaseItem* item) { return true; }

itParam::KineticFlag itDragoonSetCustomizer::getKineticFlags(BaseItem* item) { return static_cast<itParam::KineticFlag>(0x40); }

itDragoonSightCustomizer::~itDragoonSightCustomizer() { }

int itDragoonSightCustomizer::onGetLayerType(BaseItem* item) { return 0x7; }

itWarpstarCustomizer::~itWarpstarCustomizer() { }

bool itWarpstarCustomizer::isConstraintHave(BaseItem* item) { return false; }

bool itWarpstarCustomizer::isReferenceControlerHave(BaseItem* item) { return true; }

itParam::KineticFlag itWarpstarCustomizer::getKineticFlags(BaseItem* item) { return static_cast<itParam::KineticFlag>(0x20); }

itHarisenCustomizer::~itHarisenCustomizer() { }

itCurryShotCustomizer::~itCurryShotCustomizer() { }

void itCurryShotCustomizer::onUpdateKinetic(BaseItem* item) { }

itHammerCustomizer::~itHammerCustomizer() { }

itGoldenHammerCustomizer::~itGoldenHammerCustomizer() { }

itPasaranCustomizer::~itPasaranCustomizer() { }

void itPasaranCustomizer::onReset(BaseItem* item) { }

itParam::KineticFlag itPasaranCustomizer::getKineticFlags(BaseItem* item) { return static_cast<itParam::KineticFlag>(0x18); }

itGreenShellCustomizer::~itGreenShellCustomizer() { }

bool itGreenShellCustomizer::isUseShake(BaseItem* item) { return false; }

itBombheiCustomizer::~itBombheiCustomizer() { }

itLinkBombCustomizer::~itLinkBombCustomizer() { }

bool itLinkBombCustomizer::onNotifyEventCollisionSearch(BaseItem* item, soCollisionLog* collisionLog) { return true; }

itDevilCustomizer::~itDevilCustomizer() { }

itDoseisanCustomizer::~itDoseisanCustomizer() { }

bool itDoseisanCustomizer::isUseSpeedDamage(BaseItem* item) { return false; }

itTeamHealingCustomizer::~itTeamHealingCustomizer() { }

itGokulinCustomizer::~itGokulinCustomizer() { }

itHeririnCustomizer::~itHeririnCustomizer() { }

void itHeririnCustomizer::onProcessUpdate(BaseItem* item) { }

itMoneyCustomizer::~itMoneyCustomizer() { }

itTingleCustomizer::~itTingleCustomizer() { }

itCBoxCustomizer::~itCBoxCustomizer() { }

bool itCBoxCustomizer::isUseScaleZ(BaseItem* item) { return false; }

itParam::KineticFlag itCBoxCustomizer::getKineticFlags(BaseItem* item) { return static_cast<itParam::KineticFlag>(0x300); }

itManaphyCustomizer::~itManaphyCustomizer() { }

void itManaphyCustomizer::onDestruct(BaseItem* item) { }

itJeffCustomizer::~itJeffCustomizer() { }

itJeffPencilCustomizer::~itJeffPencilCustomizer() { }

bool itJeffPencilCustomizer::onReflect(BaseItem* item) { return false; }

itUsagiHatCustomizer::~itUsagiHatCustomizer() { }

bool itUsagiHatCustomizer::isUsePhysics(BaseItem* item) { return true; }

itBadgeCustomizer::~itBadgeCustomizer() { }

itDiddyPeanutsCustomizer::~itDiddyPeanutsCustomizer() { }

bool itDiddyPeanutsCustomizer::isUseSpeedDamage(BaseItem* item) { return false; }

itLittlemacCustomizer::~itLittlemacCustomizer() { }

itSlowCustomizer::~itSlowCustomizer() { }

itShadowCustomizer::~itShadowCustomizer() { }

class itFigureArchive;

template s32 soArrayList<itFigureArchive*, 64>::size() const;
template s32 soArrayList<itFigureArchive*, 64>::capacity() const;

// HYPOTHESIS: the default layer type depends on the item kind; a few kinds defer to a mode value inside itManager.
int itCustomizerInterface::onGetLayerType(BaseItem* item) {
    switch (item->m_kind) {
    case 0x0:
    case 0x4:
    case 0xC:
    case 0x17:
    case 0x1A:
    case 0x28:
    case 0x32:
    case 0x3A:
    case 0x3F:
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x45:
    case 0x46:
    case 0x47:
    case 0x49:
        return 3;
    case 0xA0:
        return 5;
    case 0x4B:
    case 0x4C:
        return 0;
    case 0x6E:
    case 0x76:
    case 0x77:
    case 0x86:
    case 0x87:
        {
            int* mgr = reinterpret_cast<int*>(reinterpret_cast<u8*>(itManager::getInstance()));
            int layer = 2;
            if (mgr[0x54 / 4] == 0x24) {
                layer = 0;
            }
            return layer;
        }
    default:
        return 2;
    }
}

// HYPOTHESIS: the byte at item + 0x3D27 is a flag cleared on reset (the tail of the unnamed region before m_genId).
void itSmokeScreenCustomizer::onReset(BaseItem* item) { *(reinterpret_cast<u8*>(item) + 0x3D27) = 0; }

void itSmartBombCustomizer::onReset(BaseItem* item) { *(reinterpret_cast<u8*>(item) + 0x3D27) = 0; }

bool itSandbagCustomizer::isUseSpeedDamage(BaseItem* item) { return item->m_kind != 0x4F; }

// HYPOTHESIS: the touch message's float argument is 1.0f.
bool itTuchCustomizer::onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8 searchHitCollisionIndex, Vec3f* searchHitPos) {
    item->sendTouchMessage(searchHitTaskId, searchHitPos, 1.0f);
    return false;
}

bool itThunderCustomizer::onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8 searchHitCollisionIndex, Vec3f* searchHitPos) {
    item->sendTouchMessage(searchHitTaskId, searchHitPos, 1.0f);
    return false;
}

// HYPOTHESIS: the bullet count is the item's work int 0x10000000.
int itSuperScopeCustomizer::onGetBullet(BaseItem* item) { return item->getModuleAccesser()->getWorkManageModule().getInt(0x10000000); }

int itThunderCustomizer::onResult(BaseItem* item) { return item->getModuleAccesser()->getWorkManageModule().getInt(0x10000000); }
int itFireFlowerCustomizer::onGetBullet(BaseItem* item) { return item->getModuleAccesser()->getWorkManageModule().getInt(0x10000000); }
int itRayGunCustomizer::onGetBullet(BaseItem* item) { return item->getModuleAccesser()->getWorkManageModule().getInt(0x10000000); }
int itClackerCustomizer::onGetBullet(BaseItem* item) { return item->getModuleAccesser()->getWorkManageModule().getInt(0x10000000); }
int itRipStickCustomizer::onGetBullet(BaseItem* item) { return item->getModuleAccesser()->getWorkManageModule().getInt(0x10000000); }
int itStarRodCustomizer::onGetBullet(BaseItem* item) { return item->getModuleAccesser()->getWorkManageModule().getInt(0x10000000); }

// HYPOTHESIS: onAction stores the rate in the work float 0x11000000 when the action id is 0.
void itRobotGyroCustomizer::onAction(BaseItem* item, u32 action, float rate) {
    if (action == 0) {
        item->getModuleAccesser()->getWorkManageModule().setFloat(rate, 0x11000000);
    }
}

// HYPOTHESIS: the motion changes follow the action ids 0x64 (begin) and 0x65 (end) while the item's status kind is 2.
void itBeamSwordCustomizer::onAction(BaseItem* item, u32 action, float rate) {
    if (item->getEmbeddedModuleAccesser().getStatusModule().getStatusKind() == 2) {
        switch (static_cast<int>(action)) {
        case 0x64:
            item->changeMotion(2, false);
            break;
        case 0x65:
            item->changeMotion(1, false);
            break;
        }
    }
}

void itStarRodCustomizer::onAction(BaseItem* item, u32 action, float rate) {
    if (item->getEmbeddedModuleAccesser().getStatusModule().getStatusKind() == 2) {
        switch (static_cast<int>(action)) {
        case 0x64:
            item->changeMotion(7, false);
            break;
        case 0x65:
            item->changeMotion(1, false);
            break;
        }
    }
}

// HYPOTHESIS: leaving the held status (2) toggles the whole-model visibility back on.
void itChewingCustomizer::onNotifyEventChangeStatus(BaseItem* item, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    if (statusKind != 2 && statusKind != 6 && prevStatusKind == 2) {
        item->setVisibilityWhole(true);
    }
}

void itSmokeScreenCustomizer::onNotifyEventChangeStatus(BaseItem* item, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    if (statusKind != 2 && statusKind != 6 && prevStatusKind == 2) {
        item->setVisibilityWhole(true);
    }
}

void itDragoonSetCustomizer::onReset(BaseItem* item) {
    if (CameraController::getInstance() != nullptr) {
        CameraController::getInstance()->setDisableZoomStart();
    }
}

void itDragoonSetCustomizer::onDestruct(BaseItem* item) {
    if (CameraController::getInstance() != nullptr) {
        CameraController::getInstance()->setDisableZoomEnd();
    }
}

// HYPOTHESIS: reflecting clears the item's work int 0x10000000.
bool itChewingCustomizer::onReflect(BaseItem* item) {
    item->getModuleAccesser()->getWorkManageModule().setInt(-1, 0x10000000);
    return false;
}

bool itSmokeScreenCustomizer::onReflect(BaseItem* item) {
    item->getModuleAccesser()->getWorkManageModule().setInt(-1, 0x10000000);
    return false;
}
