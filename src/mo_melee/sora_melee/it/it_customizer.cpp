// The unit also instantiates the soArrayList<itFigureArchive*, 64> size/capacity members (weak copies).
#define SO_ARRAY_LIST_EXTERNAL_INDEX_MEMBERS
#define SO_ARRAY_LIST_EXTERNAL_isFull
#define SO_ARRAY_LIST_EXTERNAL_shiftFreeArrayIndex
#define SO_ARRAY_LIST_EXTERNAL_getArrayIndex
#define SO_ARRAY_LIST_EXTERNAL_insertSub
#define SO_ARRAY_LIST_EXTERNAL_eraseSub
#include <so/so_array.h>
#define IT_CUSTOMIZER_BOOL_HOOKS // the hooks below return values, see it/item.h
#include <it/item.h>
#include <it/kinetic/it_kinetic_module_impl.h>
#include <math.h>
#include <st/stage.h>
#include <st_homerun/st_homerun.h>
#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_photo.h>
#include <if/if_resetsan.h>
#include <it/it_figure_manager.h>
#include <ft/ft_external_value_accesser.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/event/so_gimmick_event_presenter.h>
#include <so/so_external_value_accesser.h>
#include <types.h>

// The item customizers (the per-item hooks of BaseItem): this unit holds their overrides and destructors; the hooks that
// are not decompiled yet are only declared (so the vtables have their slots). The names of the hooks the module map
// leaves unnamed come from their vtable slots.
// TODO: the remaining hooks, the non-virtual helpers and itPhotoCallBack with the static initializer.

class itResetsanCustomizer : public itCustomizer<itResetsanCustomizer> {
public:
    virtual ~itResetsanCustomizer();
    virtual void onConstruct(BaseItem* item);
    virtual void onReset(BaseItem* item);
    virtual bool onNotifyEventAnimCmd(BaseItem* item, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);
    virtual itParam::KineticFlag getKineticFlags(BaseItem* item);
};

class itBananaCustomizer : public itCustomizer<itBananaCustomizer> {
public:
    virtual ~itBananaCustomizer();
    virtual void onNotifyEventCollisionAttackCheck(BaseItem* item, int hitTaskId);
};

class itSmashBallCustomizer : public itCustomizer<itSmashBallCustomizer> {
public:
    virtual ~itSmashBallCustomizer();
    virtual bool onNotifyEventAnimCmd(BaseItem* item, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);
    virtual void onUpdateAiDir(BaseItem* item, Vec2f*, Vec2f*, Vec2f*, Vec2f*);
    virtual void onUpdateAiMove(BaseItem* item, Vec2f*, Vec2f*, Vec2f*);
    virtual bool onPreDamageCheck(BaseItem* item, int damageStatusKind, soDamageLog*);
    virtual bool isUseSpeedDamage(BaseItem* item);
    virtual itParam::KineticFlag getKineticFlags(BaseItem* item);
};

class itKatanaAnaCustomizer : public itCustomizer<itKatanaAnaCustomizer> {
public:
    virtual ~itKatanaAnaCustomizer();
    virtual bool onSearchTarget(BaseItem* item, int* taskId, Vec3f pos, int);
};

class itSonansCustomizer : public itCustomizer<itSonansCustomizer> {
public:
    virtual ~itSonansCustomizer();
    virtual bool onProcessFixPosition(BaseItem* item);
    virtual void onNotifyEventCollisionAttackCheck(BaseItem* item, int hitTaskId);
    virtual bool onDamage(BaseItem* item, int damageStatusKind, soDamageLog*);
    virtual bool isUseSpeedDamage(BaseItem* item);
    virtual itParam::KineticFlag getKineticFlags(BaseItem* item);
};

class itSandbagCustomizer : public itCustomizer<itSandbagCustomizer> {
public:
    virtual ~itSandbagCustomizer();
    virtual void onReset(BaseItem* item);
    virtual bool onProcessFixPosition(BaseItem* item);
    virtual void onNotifyEventLink(BaseItem* item, void* unk1, soModuleAccesser* moduleAccesser, StageObject*, int unk4);
    virtual bool onNotifyEventCaptureStatus(BaseItem* item);
    virtual bool onDamage(BaseItem* item, int damageStatusKind, soDamageLog*);
    virtual void onPreBound(BaseItem* item, Vec2f*, BaseItem::BoundMask);
    virtual bool isUseTurnDamage(BaseItem* item);
    virtual bool isUseSpeedDamage(BaseItem* item);
    virtual itParam::KineticFlag getKineticFlags(BaseItem* item);
};

class itSuperScopeCustomizer : public itCustomizer<itSuperScopeCustomizer> {
public:
    virtual ~itSuperScopeCustomizer();
    virtual u32 onShoot(BaseItem* item, u32, float lr, float);
    virtual u32 onShootBlanks(BaseItem* item, u32);
    virtual int onGetBullet(BaseItem* item);
};

class itFireFlowerCustomizer : public itCustomizer<itFireFlowerCustomizer> {
public:
    virtual ~itFireFlowerCustomizer();
    virtual u32 onShoot(BaseItem* item, u32, float lr, float);
    virtual u32 onShootBlanks(BaseItem* item, u32);
    virtual int onGetBullet(BaseItem* item);
    virtual void onAction(BaseItem* item, u32, float rate);
};

class itRayGunCustomizer : public itCustomizer<itRayGunCustomizer> {
public:
    virtual ~itRayGunCustomizer();
    virtual u32 onShoot(BaseItem* item, u32, float lr, float);
    virtual u32 onShootBlanks(BaseItem* item, u32);
    virtual int onGetBullet(BaseItem* item);
};

class itRayGunShotCustomizer : public itCustomizer<itRayGunShotCustomizer> {
public:
    virtual ~itRayGunShotCustomizer();
    virtual bool onProcessFixPosition(BaseItem* item);
};

class itClackerCustomizer : public itCustomizer<itClackerCustomizer> {
public:
    virtual ~itClackerCustomizer();
    virtual u32 onShoot(BaseItem* item, u32, float lr, float);
    virtual u32 onShootBlanks(BaseItem* item, u32);
    virtual int onGetBullet(BaseItem* item);
};

class itRipStickCustomizer : public itCustomizer<itRipStickCustomizer> {
public:
    virtual ~itRipStickCustomizer();
    virtual u32 onShoot(BaseItem* item, u32, float lr, float);
    virtual u32 onShootBlanks(BaseItem* item, u32);
    virtual int onGetBullet(BaseItem* item);
};

class itStarRodCustomizer : public itCustomizer<itStarRodCustomizer> {
public:
    virtual ~itStarRodCustomizer();
    virtual u32 onShoot(BaseItem* item, u32, float lr, float);
    virtual u32 onShootBlanks(BaseItem* item, u32);
    virtual int onGetBullet(BaseItem* item);
    virtual void onAction(BaseItem* item, u32, float rate);
};

class itBeamSwordCustomizer : public itCustomizer<itBeamSwordCustomizer> {
public:
    virtual ~itBeamSwordCustomizer();
    virtual void onAction(BaseItem* item, u32, float rate);
};

class itFigureCustomizer : public itCustomizer<itFigureCustomizer> {
public:
    virtual ~itFigureCustomizer();
    virtual void onConstruct(BaseItem* item);
    virtual void onDestruct(BaseItem* item);
    virtual void onReset(BaseItem* item);
    virtual bool onProcessFixPosition(BaseItem* item);
    virtual void onRenderPre(BaseItem* item);
    virtual bool onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8 searchHitCollisionIndex, Vec3f* searchHitPos);
    virtual void onAction(BaseItem* item, u32, float rate);
};

class itNintendogsHiCustomizer : public itCustomizer<itNintendogsHiCustomizer> {
public:
    virtual ~itNintendogsHiCustomizer();
    virtual void onConstruct(BaseItem* item);
    virtual void onDestruct(BaseItem* item);
    virtual void onProcessFixCamera(BaseItem* item);
    virtual bool isUseScaleZ(BaseItem* item);
    virtual bool isSafe(BaseItem* item);
};

class itSpringCustomizer : public itCustomizer<itSpringCustomizer> {
public:
    virtual ~itSpringCustomizer();
    virtual void onDestruct(BaseItem* item);
    virtual bool onProcessFixPosition(BaseItem* item);
    virtual void onNotifyEventChangeStatus(BaseItem* item, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual bool onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8 searchHitCollisionIndex, Vec3f* searchHitPos);
    virtual void onNotifyEventGimmick(BaseItem* item, soGimmickEventArgs* eventInfo, int* taskId);
    virtual bool onHave(BaseItem* item, int);
};

class itWarioBikeCustomizer : public itCustomizer<itWarioBikeCustomizer> {
public:
    virtual ~itWarioBikeCustomizer();
    virtual void onNotifyEventChangeStatus(BaseItem* item, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void onNotifyEventLink(BaseItem* item, void* unk1, soModuleAccesser* moduleAccesser, StageObject*, int unk4);
    virtual bool onRemoveModelConstraint(BaseItem* item, int);
    virtual itParam::KineticFlag getKineticFlags(BaseItem* item);
};

class itChewingCustomizer : public itCustomizer<itChewingCustomizer> {
public:
    virtual ~itChewingCustomizer();
    virtual void onNotifyEventChangeStatus(BaseItem* item, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual bool onNotifyEventCollisionSearch(BaseItem* item, soCollisionLog* collisionLog);
    virtual bool onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8 searchHitCollisionIndex, Vec3f* searchHitPos);
    virtual bool onHave(BaseItem* item, int);
    virtual bool onReflect(BaseItem* item);
    virtual bool isReferenceControlerHave(BaseItem* item);
};

class itSmokeScreenCustomizer : public itCustomizer<itSmokeScreenCustomizer> {
public:
    virtual ~itSmokeScreenCustomizer();
    virtual void onReset(BaseItem* item);
    virtual void onNotifyEventChangeStatus(BaseItem* item, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual bool onNotifyEventCollisionSearch(BaseItem* item, soCollisionLog* collisionLog);
    virtual bool onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8 searchHitCollisionIndex, Vec3f* searchHitPos);
    virtual bool onReflect(BaseItem* item);
    virtual bool isReferenceControlerHave(BaseItem* item);
};

class itSmartBombCustomizer : public itCustomizer<itSmartBombCustomizer> {
public:
    virtual ~itSmartBombCustomizer();
    virtual void onReset(BaseItem* item);
};

class itMetroidCustomizer : public itCustomizer<itMetroidCustomizer> {
public:
    virtual ~itMetroidCustomizer();
    virtual bool onProcessFixPosition(BaseItem* item);
    virtual void onNotifyEventChangeStatus(BaseItem* item, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual bool onNotifyEventCollisionSearch(BaseItem* item, soCollisionLog* collisionLog);
    virtual bool onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8 searchHitCollisionIndex, Vec3f* searchHitPos);
    virtual bool onHave(BaseItem* item, int);
    virtual bool isReferenceControlerHave(BaseItem* item);
};

class itTuchCustomizer : public itCustomizer<itTuchCustomizer> {
public:
    virtual ~itTuchCustomizer();
    virtual bool onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8 searchHitCollisionIndex, Vec3f* searchHitPos);
};

class itThunderCustomizer : public itCustomizer<itThunderCustomizer> {
public:
    virtual ~itThunderCustomizer();
    virtual bool onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8 searchHitCollisionIndex, Vec3f* searchHitPos);
    virtual int onResult(BaseItem* item);
};

class itMetalBlockCustomizer : public itCustomizer<itMetalBlockCustomizer> {
public:
    virtual ~itMetalBlockCustomizer();
    virtual bool onDamage(BaseItem* item, int damageStatusKind, soDamageLog*);
};

class itRobotGyroCustomizer : public itCustomizer<itRobotGyroCustomizer> {
public:
    virtual ~itRobotGyroCustomizer();
    virtual bool onProcessFixPosition(BaseItem* item);
    virtual void onAction(BaseItem* item, u32, float rate);
    virtual itParam::KineticFlag getKineticFlags(BaseItem* item);
};

class itDragoonSetCustomizer : public itCustomizer<itDragoonSetCustomizer> {
public:
    virtual ~itDragoonSetCustomizer();
    virtual void onDestruct(BaseItem* item);
    virtual void onReset(BaseItem* item);
    virtual bool onProcessFixPosition(BaseItem* item);
    virtual void onProcessHit(BaseItem* item);
    virtual void onRenderDebug(BaseItem* item);
    virtual bool isUseScaleZ(BaseItem* item);
    virtual bool isConstraintHave(BaseItem* item);
    virtual bool isReferenceControlerHave(BaseItem* item);
    virtual itParam::KineticFlag getKineticFlags(BaseItem* item);
};

class itDragoonSightCustomizer : public itCustomizer<itDragoonSightCustomizer> {
public:
    virtual ~itDragoonSightCustomizer();
    virtual void onConstruct(BaseItem* item);
    virtual void onDestruct(BaseItem* item);
    virtual void onReset(BaseItem* item);
    virtual bool onProcessFixPosition(BaseItem* item);
    virtual void onNotifyEventLink(BaseItem* item, void* unk1, soModuleAccesser* moduleAccesser, StageObject*, int unk4);
    virtual int onGetLayerType(BaseItem* item);
};

class itWarpstarCustomizer : public itCustomizer<itWarpstarCustomizer> {
public:
    virtual ~itWarpstarCustomizer();
    virtual bool onProcessFixPosition(BaseItem* item);
    virtual bool isConstraintHave(BaseItem* item);
    virtual bool isReferenceControlerHave(BaseItem* item);
    virtual itParam::KineticFlag getKineticFlags(BaseItem* item);
};

class itHarisenCustomizer : public itCustomizer<itHarisenCustomizer> {
public:
    virtual ~itHarisenCustomizer();
    virtual void onAction(BaseItem* item, u32, float rate);
};

class itCurryShotCustomizer : public itCustomizer<itCurryShotCustomizer> {
public:
    virtual ~itCurryShotCustomizer();
    virtual void onReset(BaseItem* item);
    virtual void onUpdateKinetic(BaseItem* item);
    virtual void onThrowAttack(BaseItem* item, float, float* lr, Vec3f*, Vec3f*, float* powerMulStatus, bool*);
};

class itHammerCustomizer : public itCustomizer<itHammerCustomizer> {
public:
    virtual ~itHammerCustomizer();
    virtual void onAction(BaseItem* item, u32, float rate);
};

class itGoldenHammerCustomizer : public itCustomizer<itGoldenHammerCustomizer> {
public:
    virtual ~itGoldenHammerCustomizer();
    virtual void onNotifyEventCollisionAttackCheck(BaseItem* item, int hitTaskId);
    virtual void onAction(BaseItem* item, u32, float rate);
    virtual bool onHave(BaseItem* item, int);
};

class itPasaranCustomizer : public itCustomizer<itPasaranCustomizer> {
public:
    virtual ~itPasaranCustomizer();
    virtual void onReset(BaseItem* item);
    virtual bool onReflect(BaseItem* item);
    virtual itParam::KineticFlag getKineticFlags(BaseItem* item);
};

class itGreenShellCustomizer : public itCustomizer<itGreenShellCustomizer> {
public:
    virtual ~itGreenShellCustomizer();
    virtual bool onProcessFixPosition(BaseItem* item);
    virtual void onNotifyEventLink(BaseItem* item, void* unk1, soModuleAccesser* moduleAccesser, StageObject*, int unk4);
    virtual bool onReflect(BaseItem* item);
    virtual bool isTreadPassive(BaseItem* item);
    virtual bool isUseShake(BaseItem* item);
};

class itBombheiCustomizer : public itCustomizer<itBombheiCustomizer> {
public:
    virtual ~itBombheiCustomizer();
    virtual bool onProcessFixPosition(BaseItem* item);
};

class itLinkBombCustomizer : public itCustomizer<itLinkBombCustomizer> {
public:
    virtual ~itLinkBombCustomizer();
    virtual bool onProcessFixPosition(BaseItem* item);
    virtual bool onNotifyEventCollisionSearch(BaseItem* item, soCollisionLog* collisionLog);
    virtual bool onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8 searchHitCollisionIndex, Vec3f* searchHitPos);
};

class itDevilCustomizer : public itCustomizer<itDevilCustomizer> {
public:
    virtual ~itDevilCustomizer();
    virtual void onConstruct(BaseItem* item);
    virtual void onDestruct(BaseItem* item);
    virtual void onProcessGameProc(BaseItem* item);
    virtual bool onNotifyEventAnimCmd(BaseItem* item, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);
};

class itDoseisanCustomizer : public itCustomizer<itDoseisanCustomizer> {
public:
    virtual ~itDoseisanCustomizer();
    virtual void onNotifyEventCollisionAttackCheck(BaseItem* item, int hitTaskId);
    virtual bool isUseSpeedDamage(BaseItem* item);
};

class itTeamHealingCustomizer : public itCustomizer<itTeamHealingCustomizer> {
public:
    virtual ~itTeamHealingCustomizer();
    virtual bool onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8 searchHitCollisionIndex, Vec3f* searchHitPos);
};

class itGokulinCustomizer : public itCustomizer<itGokulinCustomizer> {
public:
    virtual ~itGokulinCustomizer();
    virtual void onDestruct(BaseItem* item);
    virtual bool onProcessFixPosition(BaseItem* item);
    virtual void onNotifyEventChangeStatus(BaseItem* item, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void onNotifyEventLink(BaseItem* item, void* unk1, soModuleAccesser* moduleAccesser, StageObject*, int unk4);
    virtual bool onEnumFighter(BaseItem* item, Fighter* fighter);
};

class itHeririnCustomizer : public itCustomizer<itHeririnCustomizer> {
public:
    virtual ~itHeririnCustomizer();
    virtual void onProcessUpdate(BaseItem* item);
};

class itMoneyCustomizer : public itCustomizer<itMoneyCustomizer> {
public:
    virtual ~itMoneyCustomizer();
    virtual void onReset(BaseItem* item);
    virtual bool onNotifyEventCollisionSearch(BaseItem* item, soCollisionLog* collisionLog);
    virtual bool onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8 searchHitCollisionIndex, Vec3f* searchHitPos);
    virtual void onAction(BaseItem* item, u32, float rate);
    virtual bool isSafe(BaseItem* item);
};

class itTingleCustomizer : public itCustomizer<itTingleCustomizer> {
public:
    virtual ~itTingleCustomizer();
    virtual void onDestruct(BaseItem* item);
    virtual bool onProcessFixPosition(BaseItem* item);
    virtual bool onEnumFighter(BaseItem* item, Fighter* fighter);
};

class itCBoxCustomizer : public itCustomizer<itCBoxCustomizer> {
public:
    virtual ~itCBoxCustomizer();
    virtual void onAction(BaseItem* item, u32, float rate);
    virtual bool onHave(BaseItem* item, int);
    virtual bool onRemoveModelConstraint(BaseItem* item, int);
    virtual bool isUseScaleZ(BaseItem* item);
    virtual bool isUseThrowRotParam(BaseItem* item);
    virtual itParam::KineticFlag getKineticFlags(BaseItem* item);
};

class itManaphyCustomizer : public itCustomizer<itManaphyCustomizer> {
public:
    virtual ~itManaphyCustomizer();
    virtual void onDestruct(BaseItem* item);
    virtual bool onProcessFixPosition(BaseItem* item);
};

class itJeffCustomizer : public itCustomizer<itJeffCustomizer> {
public:
    virtual ~itJeffCustomizer();
    virtual bool onProcessFixPosition(BaseItem* item);
};

class itJeffPencilCustomizer : public itCustomizer<itJeffPencilCustomizer> {
public:
    virtual ~itJeffPencilCustomizer();
    virtual bool onReflect(BaseItem* item);
};

class itUsagiHatCustomizer : public itCustomizer<itUsagiHatCustomizer> {
public:
    virtual ~itUsagiHatCustomizer();
    virtual void onAction(BaseItem* item, u32, float rate);
    virtual bool isUsePhysics(BaseItem* item);
};

class itBadgeCustomizer : public itCustomizer<itBadgeCustomizer> {
public:
    virtual ~itBadgeCustomizer();
    virtual void onAction(BaseItem* item, u32, float rate);
};

class itDiddyPeanutsCustomizer : public itCustomizer<itDiddyPeanutsCustomizer> {
public:
    virtual ~itDiddyPeanutsCustomizer();
    virtual void onAction(BaseItem* item, u32, float rate);
    virtual bool isUseSpeedDamage(BaseItem* item);
};

class itLittlemacCustomizer : public itCustomizer<itLittlemacCustomizer> {
public:
    virtual ~itLittlemacCustomizer();
    virtual void onNotifyEventCollisionAttackCheck(BaseItem* item, int hitTaskId);
};

class itSlowCustomizer : public itCustomizer<itSlowCustomizer> {
public:
    virtual ~itSlowCustomizer();
    virtual int onResult(BaseItem* item);
};

class itShadowCustomizer : public itCustomizer<itShadowCustomizer> {
public:
    virtual ~itShadowCustomizer();
    virtual void onNotifyEventChangeStatus(BaseItem* item, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual bool onNotifyEventAnimCmd(BaseItem* item, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);
};

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

// Hooks that read the item's work variables; the bullet / result counts are int work 0x10000000.
static inline soWorkManageModule& itWork(BaseItem* item) {
    return item->m_moduleAccesser->getWorkManageModule();
}

int itSuperScopeCustomizer::onGetBullet(BaseItem* item) { return itWork(item).getInt(0x10000000); }

int itFireFlowerCustomizer::onGetBullet(BaseItem* item) { return itWork(item).getInt(0x10000000); }

int itRayGunCustomizer::onGetBullet(BaseItem* item) { return itWork(item).getInt(0x10000000); }

int itClackerCustomizer::onGetBullet(BaseItem* item) { return itWork(item).getInt(0x10000000); }

int itRipStickCustomizer::onGetBullet(BaseItem* item) { return itWork(item).getInt(0x10000000); }

int itStarRodCustomizer::onGetBullet(BaseItem* item) { return itWork(item).getInt(0x10000000); }

int itThunderCustomizer::onResult(BaseItem* item) { return itWork(item).getInt(0x10000000); }

int itSlowCustomizer::onResult(BaseItem* item) { return itWork(item).getInt(0x10000000); }

// TODO: clean up - the sync-visibility byte at 0x3D27 of BaseItem is written by hand (it_item.cpp reads it the same way).
void itSmokeScreenCustomizer::onReset(BaseItem* item) { *(reinterpret_cast<u8*>(item) + 0x3D27) = 0; }

void itSmartBombCustomizer::onReset(BaseItem* item) { *(reinterpret_cast<u8*>(item) + 0x3D27) = 0; }

bool itCBoxCustomizer::isUseThrowRotParam(BaseItem* item) { return !itWork(item).isFlag(0x12000000); }

bool itGreenShellCustomizer::isTreadPassive(BaseItem* item) { return itWork(item).getInt(0x1000000b) == 0; }

bool itMoneyCustomizer::isSafe(BaseItem* item) { return item->isVanishMode(); }

bool itSandbagCustomizer::isUseSpeedDamage(BaseItem* item) { return item->m_kind != 0x4f; }

// The gimmick event (kind 0x34) the spring sends on pickup and removal.
struct itSpringGimmickEventArgs : public soGimmickEventArgs {
    itSpringGimmickEventArgs() : soGimmickEventArgs(static_cast<Gimmick::EventKind>(0x34), 0) { }
};

bool itSpringCustomizer::onHave(BaseItem* item, int) {
    itSpringGimmickEventArgs args;
    item->presentEventGimmick(&args, -1);
    return true;
}

void itSpringCustomizer::onDestruct(BaseItem* item) {
    itSpringGimmickEventArgs args;
    item->presentEventGimmick(&args, -1);
}

// Shown again when the status changes to 2 (except from statuses 2 and 6).
void itChewingCustomizer::onNotifyEventChangeStatus(BaseItem* item, int prevStatus, int status, soStatusData*, soModuleAccesser*) {
    if (prevStatus == 2) {
        return;
    }
    if (prevStatus == 6) {
        return;
    }
    if (status != 2) {
        return;
    }
    item->setVisibilityWhole((u8)1);
}

void itSmokeScreenCustomizer::onNotifyEventChangeStatus(BaseItem* item, int prevStatus, int status, soStatusData*, soModuleAccesser*) {
    if (prevStatus == 2) {
        return;
    }
    if (prevStatus == 6) {
        return;
    }
    if (status != 2) {
        return;
    }
    item->setVisibilityWhole((u8)1);
}

bool itChewingCustomizer::onHave(BaseItem* item, int) { return !itWork(item).isFlag(0x12000009); }

bool itChewingCustomizer::onReflect(BaseItem* item) {
    itWork(item).setInt(-1, 0x10000000);
    return false;
}

bool itSmokeScreenCustomizer::onReflect(BaseItem* item) {
    itWork(item).setInt(-1, 0x10000000);
    return false;
}

// TODO: clean up - MATCH-ONLY: the unit's literal pool is not split, so the touch radius is read through its pool label.
extern "C" const float lbl_27_rodata_46DC[];

bool itTuchCustomizer::onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8, Vec3f* searchHitPos) {
    item->sendTouchMessage(searchHitTaskId, searchHitPos, lbl_27_rodata_46DC[0]);
    return false;
}

bool itThunderCustomizer::onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8, Vec3f* searchHitPos) {
    item->sendTouchMessage(searchHitTaskId, searchHitPos, lbl_27_rodata_46DC[0]);
    return false;
}

bool itMoneyCustomizer::onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8, Vec3f* searchHitPos) {
    item->sendTouchMessage(searchHitTaskId, searchHitPos, lbl_27_rodata_46DC[0]);
    return false;
}

void itRobotGyroCustomizer::onAction(BaseItem* item, u32 action, float rate) {
    if (action == 0) {
        itWork(item).setFloat(rate, 0x11000000);
    }
}

void itDragoonSetCustomizer::onReset(BaseItem* item) {
    if (CameraController::getInstance()) {
        CameraController::getInstance()->setDisableZoomStart();
    }
}

void itDragoonSetCustomizer::onDestruct(BaseItem* item) {
    if (CameraController::getInstance()) {
        CameraController::getInstance()->setDisableZoomEnd();
    }
}

// Lost when the link event 0x3D arrives with argument 7.
void itDragoonSightCustomizer::onNotifyEventLink(BaseItem* item, void* unk1, soModuleAccesser* moduleAccesser, StageObject*, int unk4) {
    if (*static_cast<int*>(unk1) == 0x3d && unk4 == 7) {
        item->requestLost();
    }
}

bool itTingleCustomizer::onEnumFighter(BaseItem* item, Fighter* fighter) {
    return soExternalValueAccesser::getWorkInt(reinterpret_cast<StageObject*>(fighter), 0x12000023) == 0;
}

void itCBoxCustomizer::onAction(BaseItem* item, u32 action, float rate) {
    soWorkManageModule& work = itWork(item);
    switch ((int)action) {
    case 0:
        work.onFlag(0x12000000);
        break;
    case 1:
        item->setPickUpAreaRequest(true);
        break;
    }
}

void itDiddyPeanutsCustomizer::onAction(BaseItem* item, u32 action, float rate) {
    if (action == 0) {
        itWork(item).onFlag(0x12000000);
    }
}

// Firing without a shot uses up one bullet (int work 0x10000000) and returns what is left.
u32 itRayGunCustomizer::onShootBlanks(BaseItem* item, u32) {
    soWorkManageModule& work = itWork(item);
    int bullets = work.getInt(0x10000000);
    if (bullets > 0) {
        bullets--;
        work.setInt(bullets, 0x10000000);
    }
    return bullets;
}

u32 itClackerCustomizer::onShootBlanks(BaseItem* item, u32) {
    soWorkManageModule& work = itWork(item);
    int bullets = work.getInt(0x10000000);
    if (bullets > 0) {
        bullets--;
        work.setInt(bullets, 0x10000000);
    }
    return bullets;
}

u32 itRipStickCustomizer::onShootBlanks(BaseItem* item, u32) {
    soWorkManageModule& work = itWork(item);
    int bullets = work.getInt(0x10000000);
    if (bullets > 0) {
        bullets--;
        work.setInt(bullets, 0x10000000);
    }
    return bullets;
}

void itUsagiHatCustomizer::onAction(BaseItem* item, u32 action, float rate) {
    if (action == 0) {
        soWorkManageModule& work = itWork(item);
        work.onFlag(0x12000000);
        work.setInt(-1, 0x1000000b);
    }
}

void itBadgeCustomizer::onAction(BaseItem* item, u32 action, float rate) {
    if (action == 0) {
        soWorkManageModule& work = itWork(item);
        work.onFlag(0x12000000);
        work.setInt(-1, 0x1000000b);
    }
}

void itDevilCustomizer::onConstruct(BaseItem* item) {
    g_Stage->enableDevil();
}

void itDevilCustomizer::onDestruct(BaseItem* item) {
    if (g_Stage) {
        g_Stage->setDevilScrool(lbl_27_rodata_46DC[0], lbl_27_rodata_46DC[0]);
        g_Stage->disableDevil();
    }
}

// TODO: clean up - MATCH-ONLY: a second literal of the unit's pool (the speed decay), read through its pool label.
extern "C" const float lbl_27_rodata_46D8;

// Each attack check slows the spin (float work 0x11000000) down by a factor until it is zero.
void itSonansCustomizer::onNotifyEventCollisionAttackCheck(BaseItem* item, int hitTaskId) {
    soWorkManageModule& work = itWork(item);
    float speed = work.getFloat(0x11000000);
    if (!((float)fabs(speed) <= lbl_27_rodata_46DC[0])) {
        speed *= lbl_27_rodata_46D8;
        work.setFloat(speed, 0x11000000);
    }
}


// Bouncing off the ground in motions 11..16 drops the vertical speed.
void itSandbagCustomizer::onPreBound(BaseItem* item, Vec2f* speed, BaseItem::BoundMask mask) {
    if (mask & 1) {
        int motion = item->m_moduleAccesser->getMotionModule().getKind();
        if ((motion >= 12 && motion <= 16) || motion == 11) {
            speed->m_y = lbl_27_rodata_46DC[0];
        }
    }
}

// The swing hit sound plays while the hammer is in motion 10.
void itGoldenHammerCustomizer::onNotifyEventCollisionAttackCheck(BaseItem* item, int hitTaskId) {
    if (item->m_moduleAccesser->getMotionModule().getKind() == 10) {
        item->m_moduleAccesser->getSoundModule().playSE((SndID)0x1f52, 0, 0, 0);
    }
}

// Link event 0x840 (HYPOTHESIS: the bike is broken up) takes parameter 2 as its float argument and removes the bike.
void itWarioBikeCustomizer::onNotifyEventLink(BaseItem* item, void* unk1, soModuleAccesser* moduleAccesser, StageObject*, int unk4) {
    switch (*static_cast<int*>(unk1)) {
    case 0x840:
        static_cast<float*>(unk1)[2] = item->getParamFloat(2);
        item->remove();
        break;
    }
}

// Swing actions while held: 0x64 plays the swing motion, 0x65 the dash swing.
void itStarRodCustomizer::onAction(BaseItem* item, u32 action, float rate) {
    if (item->getEmbeddedModuleAccesser().getStatusModule().getStatusKind() == 2) {
        switch (action) {
        case 0x64:
            item->changeMotion(7, false);
            break;
        case 0x65:
            item->changeMotion(1, false);
            break;
        }
    }
}

void itBeamSwordCustomizer::onAction(BaseItem* item, u32 action, float rate) {
    if (item->getEmbeddedModuleAccesser().getStatusModule().getStatusKind() == 2) {
        switch (action) {
        case 0x64:
            item->changeMotion(2, false);
            break;
        case 0x65:
            item->changeMotion(1, false);
            break;
        }
    }
}

// Actions 1 and 2 wake and sleep the hammer's attack collision while it is held.
void itGoldenHammerCustomizer::onAction(BaseItem* item, u32 action, float rate) {
    if (item->getEmbeddedModuleAccesser().getStatusModule().getStatusKind() == 2) {
        switch (action) {
        case 1:
            item->m_moduleAccesser->getCollisionAttackModule().sleep(false);
            break;
        case 2:
            item->m_moduleAccesser->getCollisionAttackModule().sleep(true);
            break;
        }
    }
}

bool itGoldenHammerCustomizer::onHave(BaseItem* item, int) {
    if (item->getEmbeddedModuleAccesser().getStatusModule().getStatusKind() != 2) {
        return true;
    }
    item->changeMotion(9, false);
    return true;
}

// A direct hit (outside status 6) sends the touch message to the attacker's owner.
bool itMetalBlockCustomizer::onDamage(BaseItem* item, int damageStatusKind, soDamageLog* damageLog) {
    if (item->getEmbeddedModuleAccesser().getStatusModule().getStatusKind() != 6 && damageLog->m_isAttackDirect) {
        Vec3f pos = item->m_moduleAccesser->getPostureModule().getPos();
        item->sendTouchMessage(damageLog->m_attackerTeamOwnerId, &pos, lbl_27_rodata_46DC[0]);
    }
    return true;
}

// TODO: clean up - two bytes of itKineticModuleImpl (0x304, 0x305; HYPOTHESIS: a ground mode and its kind) are written
// by hand; name them once the item kinetic module header has the fields.
void itCurryShotCustomizer::onReset(BaseItem* item) {
    itKineticModuleImpl& kinetic = dynamic_cast<itKineticModuleImpl&>(item->m_moduleAccesser->getKineticModule());
    reinterpret_cast<u8*>(&kinetic)[0x304] = 0;
    reinterpret_cast<u8*>(&kinetic)[0x305] = 2;
}

void itMoneyCustomizer::onReset(BaseItem* item) {
    item->m_moduleAccesser->getCollisionSearchModule().setCheckInterval(6);
    if (item->m_kind == 0xf) {
        itKineticModuleImpl& kinetic = dynamic_cast<itKineticModuleImpl&>(item->m_moduleAccesser->getKineticModule());
        reinterpret_cast<u8*>(&kinetic)[0x304] = 0;
        reinterpret_cast<u8*>(&kinetic)[0x305] = 5;
    }
}

// The last shot plays the empty motion.
u32 itFireFlowerCustomizer::onShootBlanks(BaseItem* item, u32) {
    soWorkManageModule& work = itWork(item);
    int bullets = work.getInt(0x10000000);
    if (bullets > 0) {
        if (--bullets <= 0) {
            bullets = 0;
            item->changeMotion(10, false);
        }
        work.setInt(bullets, 0x10000000);
    }
    return bullets;
}

// Action 0 makes the coin visible again and lets it be picked up.
void itMoneyCustomizer::onAction(BaseItem* item, u32 action, float rate) {
    soWorkManageModule& work = itWork(item);
    switch (action) {
    case 0:
        item->setVanishMode(false);
        work.offFlag(0x12000008);
        item->m_moduleAccesser->getAreaModule().setWhole(0);
        item->m_moduleAccesser->getStatusModule().changeStatusRequest(5, item->m_moduleAccesser);
        break;
    }
}

void itFireFlowerCustomizer::onAction(BaseItem* item, u32 action, float rate) {
    if (item->getEmbeddedModuleAccesser().getStatusModule().getStatusKind() == 2) {
        switch (action) {
        case 0:
            item->changeMotion(8, false);
            break;
        case 1:
            item->changeMotion(9, false);
            break;
        }
        item->m_moduleAccesser->getStatusModule().changeStatusRequest(2, item->m_moduleAccesser);
    }
}


// itManager (it/it_manager.h pulls in the shared customizer header, whose classes this unit declares itself).
class itManager {
public:
    // TODO: clean up - only the trophy manager member is placed (it/it_manager.h has the full layout).
    char _0[0x14B8];
    itFigureManager* m_figureManager;

    static itManager* getInstance();
};

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

void itResetsanCustomizer::onConstruct(BaseItem* item) {
    IfResetsanTask* task = IfResetsanTask::create(item, item->m_itArchive, 0x80);
    if (task != NULL) {
        // TODO: clean up - the window's task id is kept in a BaseItem field that is not declared yet (0x3D2C).
        *reinterpret_cast<u32*>(reinterpret_cast<u8*>(item) + 0x3D2C) = task->m_taskId;
    }
}

bool itSandbagCustomizer::onProcessFixPosition(BaseItem* item) { return true; }

bool itRobotGyroCustomizer::onProcessFixPosition(BaseItem* item) { return true; }

// TODO: clean up - placeholder name of the sora_melee helper the fighter check goes through (ft_external_value_accesser.cpp).
extern "C" bool fn_27_14D74C(Fighter* fighter);

bool itGokulinCustomizer::onEnumFighter(BaseItem* item, Fighter* fighter) { return fn_27_14D74C(fighter); }

bool itPasaranCustomizer::onReflect(BaseItem* item) {
    return item->m_moduleAccesser->getStatusModule().changeStatusRequest(5, item->m_moduleAccesser);
}

void itDragoonSetCustomizer::onRenderDebug(BaseItem* item) {
    int kind = item->m_moduleAccesser->getMotionModule().getKind();
    if (kind == 1) {
        return;
    }
}

// TODO: clean up - the photo callbacks of the Nintendogs and Dragoon sight items are statics of the unit (with its
// static initializer, not decompiled yet); they are reached through their bss labels.
extern "C" utListNode lbl_27_bss_6460;
extern "C" utListNode lbl_27_bss_6480;

static inline cmPhotoController* itGetPhotoController() {
    gfCameraController* cm = !CameraController::getInstance() ? 0 :
        CameraController::getInstance()->getCameraController(CameraController::Photo);
    return static_cast<cmPhotoController*>(cm);
}

void itNintendogsHiCustomizer::onConstruct(BaseItem* item) {
    cmPhotoController* photo = itGetPhotoController();
    if (photo) {
        photo->addCallBack(&lbl_27_bss_6460);
    }
}

void itNintendogsHiCustomizer::onDestruct(BaseItem* item) {
    cmPhotoController* photo = itGetPhotoController();
    if (photo) {
        photo->removeCallBack(&lbl_27_bss_6460);
    }
}

void itDragoonSightCustomizer::onConstruct(BaseItem* item) {
    cmPhotoController* photo = itGetPhotoController();
    if (photo) {
        photo->addCallBack(&lbl_27_bss_6480);
    }
}

void itDragoonSightCustomizer::onDestruct(BaseItem* item) {
    item->m_moduleAccesser->getEffectModule().killAll(4, false, false);
    cmPhotoController* photo = itGetPhotoController();
    if (photo) {
        photo->removeCallBack(&lbl_27_bss_6480);
    }
}

// The owner's own coin search is ignored while int work 0x1000000B is positive.
bool itMoneyCustomizer::onNotifyEventCollisionSearch(BaseItem* item, soCollisionLog* collisionLog) {
    if (collisionLog->m_taskId == static_cast<u32>(item->m_createOwnerTaskId)) {
        if (itWork(item).getInt(0x1000000b) > 0) {
            return false;
        }
    }
    return true;
}

extern "C" const float lbl_27_rodata_46C8[];

// Status 0xB plays the 2D effect 0xCC0003.
void itShadowCustomizer::onNotifyEventChangeStatus(BaseItem* item, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    if (statusKind == 0xB) {
        Vec3f pos;
        Vec3f rot;
        rot.m_x = lbl_27_rodata_46DC[0];
        rot.m_y = lbl_27_rodata_46DC[0];
        rot.m_z = lbl_27_rodata_46DC[0];
        pos.m_x = lbl_27_rodata_46DC[0];
        pos.m_y = lbl_27_rodata_46DC[0];
        pos.m_z = lbl_27_rodata_46DC[0];
        item->m_moduleAccesser->getEffectModule().req2D(static_cast<EfID>(0xCC0003), &pos, &rot, lbl_27_rodata_46C8[0], 6);
    }
}

// Entering status 2 while linked to node 3 sets the link attribute 7 off.
void itMetroidCustomizer::onNotifyEventChangeStatus(BaseItem* item, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    if (statusKind == 2 && item->m_moduleAccesser->getLinkModule().isLink(3)) {
        item->m_moduleAccesser->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(7), false);
    }
}

// The trophy id is the item's variation (1 when it is not a valid trophy).
static inline u32 itGetFigureId(BaseItem* item) {
    u32 id = item->m_variation;
    return itManager::getInstance()->m_figureManager->isValidFigureId(id) ? id : 1;
}

void itFigureCustomizer::onReset(BaseItem* item) {
    itFigureManager* manager = itManager::getInstance()->m_figureManager;
    if (manager->preloadFigureTex(itGetFigureId(item), true)) {
        itWork(item).onFlag(0x12000003);
    }
}

void itFigureCustomizer::onDestruct(BaseItem* item) {
    itFigureManager* manager = itManager::getInstance()->m_figureManager;
    u32 id = itGetFigureId(item);
    if (itWork(item).isFlag(0x12000003)) {
        manager->unloadFigureTex(id, true);
    }
}

// A box held by someone else is unlinked from its previous holder.
bool itCBoxCustomizer::onHave(BaseItem* item, int taskId) {
    if (item->m_moduleAccesser->getLinkModule().isLink(1) &&
        static_cast<u32>(taskId) != static_cast<u32>(item->m_moduleAccesser->getLinkModule().getParentTaskId(1))) {
        item->m_moduleAccesser->getLinkModule().unlink(1);
    }
    return true;
}

// TODO: clean up - the original falls off the end in the handled cases (the result register keeps the last call's value).
bool itTeamHealingCustomizer::onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8, Vec3f* searchHitPos) {
    switch (item->sendTouchMessage(searchHitTaskId, searchHitPos, lbl_27_rodata_46DC[0])) {
    case 1:
        item->requestLost();
        break;
    case 2:
        itWork(item).setInt(searchHitTaskId, 0x10000000);
        item->m_moduleAccesser->getStatusModule().changeStatusRequest(7, item->m_moduleAccesser);
        break;
    default:
        return false;
    }
}

// TODO: clean up - MATCH-ONLY: the minimum height literal is read through the unit's pool label.
extern "C" const float lbl_27_rodata_479C[];

// Keeps the item at or above the height of param 0x403.
bool itJeffCustomizer::onProcessFixPosition(BaseItem* item) {
    float minY = item->getParamFloat(0x403);
    if (minY > lbl_27_rodata_479C[0]) {
        soPostureModule* posture = &item->m_moduleAccesser->getPostureModule();
        Vec3f pos = posture->getPos();
        if (pos.m_z < minY) {
            pos.m_z = minY;
        }
        posture->setPos(&pos);
        item->updateNodeSRT();
    }
    return true;
}

// A trophy that touches the Home-Run Contest sandbag is entered on the stage and removed.
// TODO: clean up - the original falls off the end after removing the trophy (no return value on that path).
bool itFigureCustomizer::onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8, Vec3f* searchHitPos) {
    gfTask* task = gfTask::getTask(searchHitTaskId);
    if (task == NULL) {
        return false;
    }
    if (task->m_taskCategory != gfTask::Category_Item) {
        return false;
    }
    BaseItem* hit = dynamic_cast<BaseItem*>(task);
    if (hit == NULL || hit->m_kind != 0x4F) {
        return false;
    }
    if (g_Stage != NULL && g_Stage->m_stageKind == Stages::HomeRunContest) {
        static_cast<stHomerun*>(g_Stage)->entryFigure(item->m_instanceId, item->m_kind, item->m_variation);
    }
    item->requestLost();
}

// A fighter caught by the Metroid is held at its Metroid node (any other target at no node).
bool itMetroidCustomizer::onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8, Vec3f* searchHitPos) {
    gfTask* task = gfTask::getTask(searchHitTaskId);
    if (task == NULL) {
        return false;
    }
    int nodeId = -1;
    if (dynamic_cast<Fighter*>(task) != NULL) {
        nodeId = ftExternalValueAccesser::getMetroidNodeId();
    }
    itWork(item).onFlag(0x12000009);
    item->have(searchHitTaskId, nodeId, -1);
    return true;
}

// Only fighters that are not in status 0x102 can be caught; otherwise the touch flag is cleared.
bool itMetroidCustomizer::onNotifyEventCollisionSearch(BaseItem* item, soCollisionLog* collisionLog) {
    int taskId = collisionLog->m_taskId;
    soWorkManageModule* work = &itWork(item);
    gfTask* task = gfTask::getTask(taskId);
    if (task == NULL || task->m_taskCategory != gfTask::Category_Fighter) {
        work->offFlag(0x22000005);
        return false;
    }
    Fighter* fighter = dynamic_cast<Fighter*>(task);
    if (fighter == NULL || soExternalValueAccesser::getStatusKind(fighter) == 0x102) {
        work->offFlag(0x22000005);
        return false;
    }
    return true;
}

// The bomb bounces off whatever its search hits while the bounce flag is set.
bool itLinkBombCustomizer::onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8, Vec3f* searchHitPos) {
    soWorkManageModule* work = &itWork(item);
    if (work->isFlag(0x12000000)) {
        work->offFlag(0x12000000);
        work->offFlag(0x22000005);
        dynamic_cast<itKineticModuleImpl&>(item->m_moduleAccesser->getKineticModule()).characterBound(item->m_moduleAccesser, true);
    }
    return false;
}

void itWarioBikeCustomizer::onNotifyEventChangeStatus(BaseItem* item, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    soGroundModule* ground = &item->m_moduleAccesser->getGroundModule();
    ground->setShapeFlag(static_cast<soGroundShapeImpl::ShapeFlagId>(8), true, 0);
    if (prevStatusKind == 2 && statusKind != 2 && statusKind != 6) {
        ground->update(1);
        ground->testGround(0);
        item->updateNodeSRT();
    }
}

// The reflected shell keeps the reflect frames of its kind's parameter.
bool itGreenShellCustomizer::onReflect(BaseItem* item) {
    soWorkManageModule* work = &itWork(item);
    switch (item->m_kind) {
    case 0x1C:
        work->setInt(item->getParamInt(0x6338), 0x1000000c);
        break;
    case 0x51:
        work->setInt(item->getParamInt(0x6D92), 0x1000000c);
        break;
    case 0x52:
        work->setInt(item->getParamInt(0x6DC4), 0x1000000c);
        break;
    }
    return false;
}

void itFigureCustomizer::onAction(BaseItem* item, u32 action, float rate) {
    soWorkManageModule* work = &itWork(item);
    switch (action) {
    case 0:
        work->onFlag(0x12000000);
        item->m_moduleAccesser->getStatusModule().changeStatusRequest(3, item->m_moduleAccesser);
        break;
    case 1:
        work->onFlag(0x12000001);
        work->offFlag(0x12000008);
        item->m_moduleAccesser->getStatusModule().changeStatusRequest(8, item->m_moduleAccesser);
        break;
    }
}

// The spring events whose top position (and shoot parameters) are filled in after construction.
struct itSpringTopEventArgs : public soGimmickSpringEventArgs {
    itSpringTopEventArgs(Gimmick::EventKind kind) : soGimmickSpringEventArgs(kind, 0) { }
};

// While the spring is in status 8 or 0xB it tells the gimmicks where its top (node 4) is.
bool itSpringCustomizer::onProcessFixPosition(BaseItem* item) {
    if (item->getEmbeddedModuleAccesser().getStatusModule().getStatusKind() == 8 ||
        item->getEmbeddedModuleAccesser().getStatusModule().getStatusKind() == 0xB) {
        itSpringTopEventArgs args(Gimmick::Spring_Event_Pos);
        args.m_topPos = item->m_moduleAccesser->getModelModule().getNodeGlobalPosition(4, false);
        item->presentEventGimmick(&args, -1);
    }
    return true;
}

// Entering status 9 or 0xC, or status 6 from 8 or 0xB, shoots whoever stands on the spring.
void itSpringCustomizer::onNotifyEventChangeStatus(BaseItem* item, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    if (statusKind == 9 || statusKind == 0xC || (statusKind == 6 && (prevStatusKind == 8 || prevStatusKind == 0xB))) {
        itSpringTopEventArgs args(Gimmick::Spring_Event_Shoot);
        Vec3f rot = item->m_moduleAccesser->getPostureModule().getRot(0);
        args.m_topPos = moduleAccesser->getModelModule().getNodeGlobalPosition(4, false);
        args.m_bounce = item->getParamFloat(0x1AC2);
        args.m_rot = rot.m_z;
        item->presentEventGimmick(&args, -1);
    }
}

// The spring is pressed (status 8 on the ground, 0xB in the air) and reports its top position.
void itSpringCustomizer::onNotifyEventGimmick(BaseItem* item, soGimmickEventArgs* eventInfo, int* taskId) {
    switch (eventInfo->m_kind) {
    case Gimmick::Spring_Event_On:
        if (item->getEmbeddedModuleAccesser().getStatusModule().getStatusKind() != 6 &&
            (item->getEmbeddedModuleAccesser().getStatusModule().getStatusKind() != 8 ||
             item->getEmbeddedModuleAccesser().getStatusModule().getStatusKind() != 0xB)) {
            int status = 8;
            if (item->m_moduleAccesser->getSituationModule().getKind() == 0 &&
                item->m_moduleAccesser->getGroundModule().isTouch(static_cast<grCollStatus::TouchMask>(8), 0)) {
                status = 0xB;
            }
            item->m_moduleAccesser->getStatusModule().changeStatusRequest(status, item->m_moduleAccesser);
        }
    case Gimmick::Spring_Event_Shoot:
    case Gimmick::Spring_Event_Pos:
        static_cast<soGimmickSpringEventArgs*>(eventInfo)->m_topPos =
            item->m_moduleAccesser->getModelModule().getNodeGlobalPosition(4, false);
        break;
    case 0x33:
        break;
    }
}

// The spring turns to face what it hits (while flag 0x12000001 is set) and presses when the touch is accepted.
// TODO: clean up - MATCH-ONLY: the literals are read through the unit's pool label (46C8: 1, 46D8: -1, 46DC: 0,
// 4740: -90).
bool itSpringCustomizer::onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8, Vec3f* searchHitPos) {
    if (item->getEmbeddedModuleAccesser().getStatusModule().getStatusKind() == 6) {
        return false;
    }
    if (itWork(item).isFlag(0x12000001)) {
        const float* pool = lbl_27_rodata_46C8;
        gfTask* task = gfTask::getTask(searchHitTaskId);
        StageObject* target = task != NULL ? dynamic_cast<StageObject*>(task) : NULL;
        if (target != NULL) {
            Vec3f targetPos = soExternalValueAccesser::getPos(target);
            Vec3f pos = item->getPos();
            float lr = targetPos.m_x >= pos.m_x ? pool[0] : pool[4];
            item->m_moduleAccesser->getPostureModule().setLr(lr);
            item->m_moduleAccesser->getPostureModule().updateRotYLr();
            Vec3f rot(pool[5], pool[5], pool[30] * lr);
            item->m_moduleAccesser->getPostureModule().setRot(&rot, 0);
        }
    }
    if (item->sendTouchMessage(searchHitTaskId, searchHitPos, item->getParamFloat(0x1AC3))) {
        int status = 8;
        if (item->m_moduleAccesser->getSituationModule().getKind() == 0) {
            status = 0xB;
        }
        item->m_moduleAccesser->getStatusModule().changeStatusRequest(status, item->m_moduleAccesser);
    }
    return false;
}
