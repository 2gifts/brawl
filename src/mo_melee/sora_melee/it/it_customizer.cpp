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

// The item customizers (the per-item hooks of BaseItem): this unit holds their small overrides and destructors; the
// larger hooks stay in the module blob for now (declared so the vtables have their slots).
// TODO: the remaining hooks, the non-virtual helpers and itPhotoCallBack with the static initializer.

class itResetsanCustomizer : public itCustomizer<itResetsanCustomizer> {
public:
    virtual ~itResetsanCustomizer();
    virtual void onConstruct(BaseItem* item);
    virtual void onReset(BaseItem* item);
    virtual void onNotifyEventAnimCmd(BaseItem* item, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);
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
    virtual void onNotifyEventAnimCmd(BaseItem* item, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);
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
    virtual void onProcessFixPosition(BaseItem* item);
    virtual void onNotifyEventCollisionAttackCheck(BaseItem* item, int hitTaskId);
    virtual bool onDamage(BaseItem* item, int damageStatusKind, soDamageLog*);
    virtual bool isUseSpeedDamage(BaseItem* item);
    virtual itParam::KineticFlag getKineticFlags(BaseItem* item);
};

class itSandbagCustomizer : public itCustomizer<itSandbagCustomizer> {
public:
    virtual ~itSandbagCustomizer();
    virtual void onReset(BaseItem* item);
    virtual void onProcessFixPosition(BaseItem* item);
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
    virtual void onProcessFixPosition(BaseItem* item);
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
    virtual void onProcessFixPosition(BaseItem* item);
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
    virtual void onProcessFixPosition(BaseItem* item);
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
    virtual void onProcessFixPosition(BaseItem* item);
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
    virtual void onProcessFixPosition(BaseItem* item);
    virtual void onAction(BaseItem* item, u32, float rate);
    virtual itParam::KineticFlag getKineticFlags(BaseItem* item);
};

class itDragoonSetCustomizer : public itCustomizer<itDragoonSetCustomizer> {
public:
    virtual ~itDragoonSetCustomizer();
    virtual void onDestruct(BaseItem* item);
    virtual void onReset(BaseItem* item);
    virtual void onProcessFixPosition(BaseItem* item);
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
    virtual void onProcessFixPosition(BaseItem* item);
    virtual void onNotifyEventLink(BaseItem* item, void* unk1, soModuleAccesser* moduleAccesser, StageObject*, int unk4);
    virtual int onGetLayerType(BaseItem* item);
};

class itWarpstarCustomizer : public itCustomizer<itWarpstarCustomizer> {
public:
    virtual ~itWarpstarCustomizer();
    virtual void onProcessFixPosition(BaseItem* item);
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
    virtual void onProcessFixPosition(BaseItem* item);
    virtual void onNotifyEventLink(BaseItem* item, void* unk1, soModuleAccesser* moduleAccesser, StageObject*, int unk4);
    virtual bool onReflect(BaseItem* item);
    virtual bool isTreadPassive(BaseItem* item);
    virtual bool isUseShake(BaseItem* item);
};

class itBombheiCustomizer : public itCustomizer<itBombheiCustomizer> {
public:
    virtual ~itBombheiCustomizer();
    virtual void onProcessFixPosition(BaseItem* item);
};

class itLinkBombCustomizer : public itCustomizer<itLinkBombCustomizer> {
public:
    virtual ~itLinkBombCustomizer();
    virtual void onProcessFixPosition(BaseItem* item);
    virtual bool onNotifyEventCollisionSearch(BaseItem* item, soCollisionLog* collisionLog);
    virtual bool onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8 searchHitCollisionIndex, Vec3f* searchHitPos);
};

class itDevilCustomizer : public itCustomizer<itDevilCustomizer> {
public:
    virtual ~itDevilCustomizer();
    virtual void onConstruct(BaseItem* item);
    virtual void onDestruct(BaseItem* item);
    virtual void onProcessGameProc(BaseItem* item);
    virtual void onNotifyEventAnimCmd(BaseItem* item, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);
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
    virtual void onProcessFixPosition(BaseItem* item);
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
    virtual void onProcessFixPosition(BaseItem* item);
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
    virtual void onProcessFixPosition(BaseItem* item);
};

class itJeffCustomizer : public itCustomizer<itJeffCustomizer> {
public:
    virtual ~itJeffCustomizer();
    virtual void onProcessFixPosition(BaseItem* item);
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
    virtual void onNotifyEventAnimCmd(BaseItem* item, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);
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
