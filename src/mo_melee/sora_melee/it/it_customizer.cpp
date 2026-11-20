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
#include <gf/gf_task_scheduler.h>
#include <ft/ft_manager.h>
#include <so/anim/so_anim_cmd_arg_list.h>
#include <so/team/so_team.h>
#include <so/so_slow.h>
#include <cm/cm_controller_ai.h>
#include <gm/gm_global.h>
#include <mt/mt_trig.h>
#include <gm/gm_common.h>
#include <so/catch/so_catch_module.h>
#include <mt/mt_prng.h>
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
    // HYPOTHESIS: name and argument types (a new slot of this class, 0x297658): picks the two names the messages fill in
    // (kinds, full-width and UTF-16 strings).
    virtual void getMessageNames(int* kind1, const char** str1, const wchar_t** wstr1, int* kind2, const char** str2,
                                 const wchar_t** wstr2);
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
    BaseItem* createItem(Vec3f* safePos, Vec3f* pos, float lr, itKind kind, u32 variation = 0, int createOwnerTaskId = -1,
                         soResourceModule* resourceModule = NULL, u8 groupNo = 0, int brresId = 0xffff,
                         int texResIndex = 0, int texResId = 0xffff);
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

// The ball remembers its last attacker (as the attacking fighter's owner when the attack came from an article).
bool itSmashBallCustomizer::onPreDamageCheck(BaseItem* item, int damageStatusKind, soDamageLog* damageLog) {
    // TODO: clean up - the attacker's task category is read as a plain byte (soDamageLog declares a signed bit field).
    u8 category = reinterpret_cast<u8*>(damageLog)[0x44];
    if (category == 6) {
        return false;
    }
    u32 taskId = damageLog->m_attackerTaskId;
    if (category != gfTask::Category_Fighter) {
        gfTask* task = gfTaskScheduler::getInstance()->getTaskById(static_cast<gfTask::Category>(category), taskId);
        if (task != NULL) {
            taskId = soExternalValueAccesser::getTeamOwnerId(&dynamic_cast<StageObject&>(*task));
            category = gfTask::Category_Fighter;
        }
    }
    soWorkManageModule* work = &itWork(item);
    work->setInt(taskId, 0x10000000);
    work->setInt(category, 0x10000001);
    return true;
}

// TODO: clean up - placeholder name of BaseItem's target search (it_item.cpp has not decompiled it yet).
extern "C" bool fn_27_287890(BaseItem* item, int* taskId, Vec3f* pos, int);

// Search kind 8 aims at the point mirrored through the owner item.
bool itKatanaAnaCustomizer::onSearchTarget(BaseItem* item, int* taskId, Vec3f pos, int kind) {
    if (kind == 8) {
        gfTask* task = gfTask::getTask(item->m_createOwnerTaskId);
        BaseItem* owner = task != NULL ? dynamic_cast<BaseItem*>(task) : NULL;
        if (owner == NULL) {
            return false;
        }
        if (fn_27_287890(owner, taskId, &pos, 0)) {
            Vec3f ownerPos = soExternalValueAccesser::getPos(owner);
            pos.m_x = pos.m_x - (ownerPos.m_x - pos.m_x);
            pos.m_y = ownerPos.m_y;
            pos.m_z = ownerPos.m_z;
            return true;
        }
    }
    return false;
}

// The last star plays the empty motion (other anim 8) frozen on its first frame.
u32 itStarRodCustomizer::onShootBlanks(BaseItem* item, u32) {
    soWorkManageModule& work = itWork(item);
    int bullets = work.getInt(0x10000000);
    if (bullets > 0) {
        if (--bullets <= 0) {
            item->m_moduleAccesser->getMotionModule().addOtherAnim(0, 8, false, lbl_27_rodata_46DC[0], lbl_27_rodata_46C8[0]);
            item->m_moduleAccesser->getMotionModule().setOtherAnimRate(0, lbl_27_rodata_46DC[0]);
            item->m_moduleAccesser->getMotionModule().setOtherAnimFrame(lbl_27_rodata_46C8[0], 0);
        }
        work.setInt(bullets, 0x10000000);
    }
    return bullets;
}

// A blank shot still uses up ammunition by how long it was charged (params 0x69AB-0x69B6).
u32 itSuperScopeCustomizer::onShootBlanks(BaseItem* item, u32 charge) {
    soWorkManageModule& work = itWork(item);
    int bullets = work.getInt(0x10000000);
    if (bullets > 0) {
        int level;
        if (static_cast<int>(charge) <= item->getParamInt(0x69AB)) {
            level = 0;
        } else if (static_cast<int>(charge) <= item->getParamInt(0x69AC) * 8) {
            level = static_cast<int>(charge) / item->getParamInt(0x69AC) + 1;
        } else {
            level = 9;
        }
        if (level >= 0 && level <= 9) {
            bullets -= item->getParamInt(0x69AD + level);
            if (bullets < 0) {
                bullets = 0;
            }
            work.setInt(bullets, 0x10000000);
        }
    }
    return bullets;
}

// TODO: clean up - MATCH-ONLY: this unit's soAnimCmdArgList holds a non-const soArrayContractibleTable<acCmdArgConv>
// (its vtable is the one referenced), so a local copy of the list is built and handed to the getters.
struct itAnimCmdArgList {
    soArrayContractibleTable<acCmdArgConv> m_argList;
    soModuleAccesser* m_moduleAccesser;
    u8 m_errorOnValueFetch;

    itAnimCmdArgList(soModuleAccesser* moduleAccesser, const soArrayContractibleTable<const acCmdArgConv>& argList)
        : m_argList(reinterpret_cast<const soArrayContractibleTable<acCmdArgConv>&>(argList)), m_moduleAccesser(moduleAccesser),
          m_errorOnValueFetch(0) { }
    int getInt(u32 index) { return reinterpret_cast<soAnimCmdArgList*>(this)->getInt(index); }
};

// Anim cmd type 0x16 with two arguments hands the Final Smash to the fighter they name.
bool itSmashBallCustomizer::onNotifyEventAnimCmd(BaseItem* item, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3) {
    switch (acmd->getType()) {
    case 0x16:
        if (acmd->getArgNum() == 2) {
            itAnimCmdArgList args(moduleAccesser, acmd->getArgList());
            u8 category = args.getInt(0);
            int taskId = args.getInt(1);
            if (!args.m_errorOnValueFetch) {
                g_ftManager->setFinalTask(category, taskId);
                return true;
            }
        }
        break;
    }
    return false;
}

// The damage's knockback speed goes into the float work 0x11000000 and the attack power grows with the damage taken.
bool itSonansCustomizer::onDamage(BaseItem* item, int damageStatusKind, soDamageLog* damageLog) {
    soWorkManageModule* work = &itWork(item);
    work->getFloat(0x11000000);
    float add = damageLog->m_damageAdd * damageLog->m_lr * item->getParamFloat(0x2937);
    // TODO: clean up - the original also compares |add| with |speed| (an fcmpo whose result is unused); not reproduced.
    work->setFloat(add, 0x11000000);
    float powerMul = item->getParamFloat(0x2936);
    float powerMax = item->getParamFloat(0x293C);
    float power = damageLog->m_damageAdd * powerMul;
    item->m_moduleAccesser->getCollisionAttackModule().setPowerMul(power > powerMax ? powerMax : power);
    return true;
}

// In motion 3 the camera is forced onto the point given by the float work 0x11000007 / 0x11000008.
void itDragoonSetCustomizer::onProcessHit(BaseItem* item) {
    if (item->m_moduleAccesser->getMotionModule().getKind() == 3) {
        soWorkManageModule* work = &itWork(item);
        float y = work->getFloat(0x11000008);
        float x = work->getFloat(0x11000007);
        Vec3f target(x, y, lbl_27_rodata_46DC[0]);
        Vec3f center = target;
        Vec3f pos = target;
        item->m_moduleAccesser->getCameraModule().updateForce(0, &pos, &center);
    }
}

// TODO: clean up - MATCH-ONLY: the scale bounds are read through the unit's pool labels.
extern "C" const float lbl_27_rodata_4704[];
extern "C" const float lbl_27_rodata_4708[];

// The shot grows by a seventh of its speed every frame, between 0.001 and param 0x186A.
bool itRayGunShotCustomizer::onProcessFixPosition(BaseItem* item) {
    soWorkManageModule* work = &itWork(item);
    float scale = work->getFloat(0x11000000);
    scale += lbl_27_rodata_4704[0] * (float)fabs(item->getParamFloat(0x3EF));
    float max = item->getParamFloat(0x186A);
    if (scale > max) {
        scale = max;
    } else if (scale < lbl_27_rodata_4708[0]) {
        scale = lbl_27_rodata_4708[0];
    }
    Vec3f nodeScale(scale, item->m_moduleAccesser->getPostureModule().getOwnerScale(), scale);
    item->m_moduleAccesser->getModelModule().setNodeScale(0, &nodeScale);
    work->setFloat(scale, 0x11000000);
    return true;
}

template <class T>
static inline T* itCastTask(u32 taskId) {
    if (taskId == 0xFFFFFFFF) {
        return NULL;
    }
    gfTask* task = gfTask::getTask(taskId);
    if (task != NULL) {
        return dynamic_cast<T*>(task);
    }
    return NULL;
}

// The item ignores its thrower (int work 0x10000000) and the thrower's second team.
static inline bool itIsOtherTeam(BaseItem* item, soCollisionLog* collisionLog) {
    u32 throwerId = itWork(item).getInt(0x10000000);
    if (collisionLog->m_taskId == throwerId) {
        return false;
    }
    if (throwerId != 0xFFFFFFFF) {
        gfTask* task = gfTask::getTask(throwerId);
        StageObject* thrower = task != NULL ? dynamic_cast<StageObject*>(task) : NULL;
        if (thrower != NULL) {
            int team = soExternalValueAccesser::getTeam2ndNo(thrower);
            if (team != -1) {
                u32 targetId = collisionLog->m_taskId;
                if (targetId != 0xFFFFFFFF) {
                    // The original tests the thrower's task here instead of the target's (copy-paste slip).
                    gfTask* targetTask = gfTask::getTask(targetId);
                    StageObject* target = task != NULL ? dynamic_cast<StageObject*>(targetTask) : NULL;
                    if (target != NULL) {
                        int targetTeam = soExternalValueAccesser::getTeam2ndNo(target);
                        if (targetTeam != -1 && targetTeam == team) {
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool itChewingCustomizer::onNotifyEventCollisionSearch(BaseItem* item, soCollisionLog* collisionLog) {
    return itIsOtherTeam(item, collisionLog);
}

bool itSmokeScreenCustomizer::onNotifyEventCollisionSearch(BaseItem* item, soCollisionLog* collisionLog) {
    return itIsOtherTeam(item, collisionLog);
}

// A fighter caught in the smoke holds the item at the node it was hit; the item takes the team of its holder.
bool itSmokeScreenCustomizer::onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8 searchHitCollisionIndex, Vec3f* searchHitPos) {
    soWorkManageModule* work = &itWork(item);
    gfTask* task = gfTask::getTask(searchHitTaskId);
    if (task == NULL || task->m_taskCategory != gfTask::Category_Fighter) {
        work->offFlag(0x22000005);
        return false;
    }
    work->onFlag(0x12000009);
    item->have(searchHitTaskId, ftExternalValueAccesser::getNodeIdFromHitNo(searchHitCollisionIndex), -1);
    if (item->m_moduleAccesser->getLinkModule().isLink(3)) {
        u32 ownerId = item->m_moduleAccesser->getLinkModule().getParentTaskId(3);
        if (ownerId != 0xFFFFFFFF) {
            item->setTeamOwnerTaskId(ownerId);
        }
    }
    return true;
}

// Each punch motion plays its own hit sound.
void itLittlemacCustomizer::onNotifyEventCollisionAttackCheck(BaseItem* item, int hitTaskId) {
    switch (item->m_moduleAccesser->getMotionModule().getKind()) {
    case 0:
        item->m_moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1E68), false, false, 0);
        break;
    case 1:
        item->m_moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1E69), false, false, 0);
        break;
    case 2:
        item->m_moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1E6A), false, false, 0);
        break;
    case 3:
        item->m_moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1E68), false, false, 0);
        break;
    case 4:
        item->m_moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1E69), false, false, 0);
        break;
    }
}

// Anim cmd type 0x19 with two arguments slows everyone but the item's team (strength, frames).
bool itShadowCustomizer::onNotifyEventAnimCmd(BaseItem* item, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3) {
    switch (acmd->getType()) {
    case 0x19:
        if (acmd->getArgNum() == 2) {
            itAnimCmdArgList args(moduleAccesser, acmd->getArgList());
            int strength = args.getInt(0);
            int frames = args.getInt(1);
            if (!args.m_errorOnValueFetch) {
                g_ftManager->setSlow(item->m_moduleAccesser->getTeamModule().getTeam()->getNo(), true, strength, frames);
                return true;
            }
        }
        break;
    }
    return false;
}

// The box is tilted back when it is let go (and set down a bit further when flag 0x12000000 is on).
// TODO: clean up - MATCH-ONLY: the angles and offsets are read through the unit's pool label (46C8 + 0xCC: 7,
// + 0xD0: 15, + 0x74: 90, + 0: 1, + 4: 0.5).
bool itCBoxCustomizer::onRemoveModelConstraint(BaseItem* item, int) {
    const float* pool = lbl_27_rodata_46C8;
    Vec3f rot = item->m_moduleAccesser->getPostureModule().getRot(0);
    rot.m_x -= pool[0xCC / 4];
    rot.m_y += pool[0x74 / 4] * item->m_moduleAccesser->getPostureModule().getLr();
    if (itWork(item).isFlag(0x12000000)) {
        rot.m_x -= pool[0xD0 / 4];
        Vec3f pos = item->m_moduleAccesser->getPostureModule().getPos();
        pos.m_x += pool[0];
        pos.m_z += pool[1];
        item->m_moduleAccesser->getPostureModule().setPos(&pos);
    }
    item->m_moduleAccesser->getPostureModule().setRot(&rot, 0);
    return false;
}

// While the global slow motion lets objects move, a Metroid latched onto a fighter in status 0x102 drops off.
bool itMetroidCustomizer::onProcessFixPosition(BaseItem* item) {
    if (!soSlow::getInstance()->isAdjust()) {
        return true;
    }
    if (item->getEmbeddedModuleAccesser().getStatusModule().getStatusKind() != 2) {
        return true;
    }
    if (item->m_moduleAccesser->getLinkModule().isLink(3)) {
        u32 parentId = item->m_moduleAccesser->getLinkModule().getParentTaskId(3);
        gfTask* task = parentId != 0xFFFFFFFF ? gfTask::getTask(parentId) : NULL;
        Fighter* fighter = task != NULL ? dynamic_cast<Fighter*>(task) : NULL;
        if (fighter != NULL && soExternalValueAccesser::getStatusKind(fighter) == 0x102) {
            Vec3f speed(lbl_27_rodata_46DC[0], lbl_27_rodata_46DC[0], lbl_27_rodata_46DC[0]);
            item->fall(&speed, lbl_27_rodata_46C8[0]);
        }
    }
    return true;
}

// While the Devil is out (status 6) the camera offset it pushes decays (and stops below a length of 1).
// TODO: clean up - MATCH-ONLY: Vec2f::length is spelled out so its literals come from the unit's pool label (46C8 + 0x18:
// FLT_MIN, + 0x14: 0, + 0: 1, + 0xB4: 0.95).
void itDevilCustomizer::onProcessGameProc(BaseItem* item) {
    const float* pool = lbl_27_rodata_46C8;
    if (item->getEmbeddedModuleAccesser().getStatusModule().getStatusKind() == 6) {
        cmAIController* controller = g_cmAIController;
        soWorkManageModule* work = &itWork(item);
        if (controller != NULL) {
            Vec2f offset = controller->m_devilOffset;
            float lengthSquared = offset.m_x * offset.m_x + offset.m_y * offset.m_y;
            float length;
            double absolute = __fabs(lengthSquared);
            float magnitude = absolute;
            if (magnitude <= pool[0x18 / 4]) {
                length = pool[0x14 / 4];
            } else {
                length = lengthSquared * rsqrtf(lengthSquared);
            }
            if (length < pool[0]) {
                work->onFlag(0x12000000);
                offset.m_x *= pool[0x14 / 4];
                offset.m_y *= pool[0x14 / 4];
            } else {
                offset.m_x *= pool[0xB4 / 4];
                offset.m_y *= pool[0xB4 / 4];
            }
            controller->m_devilOffset = offset;
        }
    }
}

// The bomb swells and shakes as its fuse (int work 0x1000000B, out of 480 frames) runs down.
// TODO: clean up - MATCH-ONLY: the literals are read through the unit's pool label (46C8 + 0: 1, + 0x14: 0, + 0x98: 480,
// + 0xA8: -2.8, + 0xAC: -9, + 0xB0: 6.3); the int-to-float constant is the compiler's own.
bool itLinkBombCustomizer::onProcessFixPosition(BaseItem* item) {
    const float* pool = lbl_27_rodata_46C8;
    soModelModule* model = &item->m_moduleAccesser->getModelModule();
    float rate = pool[0] - itWork(item).getInt(0x1000000b) / pool[0x98 / 4];
    if (rate < pool[0x14 / 4]) {
        rate = pool[0x14 / 4];
    }
    float offset = pool[0xA8 / 4] * rate * item->getParam(static_cast<itValueAccesser::ParamFloat>(0xD71));
    float scale = item->getParam(static_cast<itValueAccesser::ParamFloat>(0xD71));
    Vec3f translate(pool[0x14 / 4], pool[0xB0 / 4] + offset, pool[0x14 / 4]);
    float angle = pool[0xAC / 4] * rate * scale;
    model->setNodeTranslate(4, &translate);
    Vec3f rotate(pool[0x14 / 4], angle, pool[0x14 / 4]);
    model->setNodeRotate(4, &rotate);
    return true;
}

// The Bob-omb swells and shakes as its fuse runs down (fully once flag 0x12000000 is set).
// TODO: clean up - MATCH-ONLY: the literals are read through the unit's pool label (46C8 + 0: 1, + 0x14: 0, + 0x98: 480,
// + 0x9C: -0.8, + 0xA0: -15, + 0xA4: 3); the int-to-float constant is the compiler's own.
bool itBombheiCustomizer::onProcessFixPosition(BaseItem* item) {
    const float* pool = lbl_27_rodata_46C8;
    soWorkManageModule* work = &itWork(item);
    soModelModule* model = &item->m_moduleAccesser->getModelModule();
    float rate;
    if (work->isFlag(0x12000000)) {
        rate = pool[0];
    } else {
        rate = pool[0] - work->getInt(0x1000000b) / pool[0x98 / 4];
    }
    if (rate < pool[0x14 / 4]) {
        rate = pool[0x14 / 4];
    }
    float offset = pool[0x9C / 4] * rate * item->getParam(static_cast<itValueAccesser::ParamFloat>(0xD71));
    float scale = item->getParam(static_cast<itValueAccesser::ParamFloat>(0xD71));
    Vec3f translate(pool[0x14 / 4], pool[0xA4 / 4] + offset, pool[0x14 / 4]);
    float angle = pool[0xA0 / 4] * rate * scale;
    model->setNodeTranslate(0x12, &translate);
    Vec3f rotate(pool[0x14 / 4], angle, pool[0x14 / 4]);
    model->setNodeRotate(0x12, &rotate);
    return true;
}

// A gum hit on a fighter (not in status 0x102) sticks to it at the hit node and takes its holder's team.
bool itChewingCustomizer::onNotifyEventCollisionSearchCheck(BaseItem* item, int searchHitTaskId, u8 searchHitCollisionIndex, Vec3f* searchHitPos) {
    soWorkManageModule* work = &itWork(item);
    gfTask* task = gfTask::getTask(searchHitTaskId);
    if (task == NULL || task->m_taskCategory != gfTask::Category_Fighter) {
        work->offFlag(0x22000005);
        return false;
    }
    Fighter* fighter = dynamic_cast<Fighter*>(task);
    if (fighter == NULL || soExternalValueAccesser::getStatusKind(fighter) == 0x102) {
        work->offFlag(0x22000005);
        return false;
    }
    work->onFlag(0x12000009);
    u32 ownerId = 0xFFFFFFFF;
    if (item->m_moduleAccesser->getLinkModule().isLink(3)) {
        ownerId = item->m_moduleAccesser->getLinkModule().getParentTaskId(3);
    }
    item->have(searchHitTaskId, ftExternalValueAccesser::getNodeIdFromHitNo(searchHitCollisionIndex), -1);
    if (ownerId != 0xFFFFFFFF) {
        item->setTeamOwnerTaskId(ownerId);
    }
    return true;
}

// Actions 0-3 play the swing motions (0-2 at the given rate); 4 and 5 grow and restore the fan.
void itHarisenCustomizer::onAction(BaseItem* item, u32 action, float rate) {
    switch (action) {
    case 0:
        item->changeMotion(7, false);
        item->m_moduleAccesser->getMotionModule().setRate(rate);
        break;
    case 1:
        item->changeMotion(8, false);
        item->m_moduleAccesser->getMotionModule().setRate(rate);
        break;
    case 2:
        item->changeMotion(9, false);
        item->m_moduleAccesser->getMotionModule().setRate(rate);
        break;
    case 3:
        item->changeMotion(10, false);
        break;
    case 4:
        {
            float scaleMin = item->getParamFloat(0x15AE);
            item->setScaleAnimation(lbl_27_rodata_46C8[0], scaleMin, 3, item->getParamInt(0x63CE));
        }
        break;
    case 5:
        item->setScaleAnimation(lbl_27_rodata_46C8[0], lbl_27_rodata_46D8, 3, 1);
        break;
    }
}

// The sight starts with its two frame animations stopped (the second one on frame 1 in widescreen) and its 2D effect.
void itDragoonSightCustomizer::onReset(BaseItem* item) {
    soMotionModule* motion = &item->m_moduleAccesser->getMotionModule();
    motion->addOtherAnim(0, 1, false, lbl_27_rodata_46DC[0], lbl_27_rodata_46C8[0]);
    motion->setOtherAnimRate(0, lbl_27_rodata_46DC[0]);
    motion->setOtherAnimFrame(lbl_27_rodata_46DC[0], 0);
    motion->addOtherAnim(1, 2, false, lbl_27_rodata_46DC[0], lbl_27_rodata_46C8[0]);
    motion->setOtherAnimRate(1, lbl_27_rodata_46DC[0]);
    if (g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen == 1) {
        motion->setOtherAnimFrame(lbl_27_rodata_46C8[0], 1);
    } else {
        motion->setOtherAnimFrame(lbl_27_rodata_46DC[0], 1);
    }
    Vec3f pos;
    Vec3f rot;
    rot.m_x = lbl_27_rodata_46DC[0];
    rot.m_y = lbl_27_rodata_46DC[0];
    rot.m_z = lbl_27_rodata_46DC[0];
    pos.m_x = lbl_27_rodata_46DC[0];
    pos.m_y = lbl_27_rodata_46DC[0];
    pos.m_z = lbl_27_rodata_46DC[0];
    item->m_moduleAccesser->getEffectModule().req2D(static_cast<EfID>(0x70), &pos, &rot, lbl_27_rodata_46C8[0], 4);
}

// The link event (kind 0x451) the Warp Star sends its rider once.
struct itWarpstarLinkEventArgs : public soLinkEventArgs {
    itWarpstarLinkEventArgs() : soLinkEventArgs(0x451) { }
};

// In motion 3 the star flies sideways by the stick (param 0x1D1A) and tells its rider once.
// TODO: clean up - the kinetic slot 0xC0 call also passes the module accesser (clearSpeed is declared with one argument).
bool itWarpstarCustomizer::onProcessFixPosition(BaseItem* item) {
    if (!soSlow::getInstance()->isAdjust()) {
        return true;
    }
    if (item->m_moduleAccesser->getMotionModule().getKind() != 3) {
        return true;
    }
    soWorkManageModule* work = &itWork(item);
    soControllerModule* controller = &item->m_moduleAccesser->getControllerModule();
    itKineticModuleImpl& kinetic = dynamic_cast<itKineticModuleImpl&>(item->m_moduleAccesser->getKineticModule());
    Vec3f speed(lbl_27_rodata_46DC[0], lbl_27_rodata_46DC[0], lbl_27_rodata_46DC[0]);
    float speedMul = item->getParamFloat(0x1D1A);
    speed.m_x = speedMul * controller->getStickX();
    kinetic.clearSpeed(true);
    kinetic.addSpeed(&speed, item->m_moduleAccesser);
    if (!work->isFlag(0x12000000)) {
        itWarpstarLinkEventArgs args;
        item->m_moduleAccesser->getLinkModule().sendEventParents(3, args);
        work->onFlag(0x12000000);
    }
    return true;
}

// Swinging the rod launches a star (item 0x3B) from node 2; swing kinds 0 and 1 have their own power and speed params.
// TODO: clean up - MATCH-ONLY: the literals are read through the unit's pool label (46C8 + 0: 1, + 0x14: 0, + 0x48: 2.6).
u32 itStarRodCustomizer::onShoot(BaseItem* item, u32 kind, float lr, float) {
    const float* pool = lbl_27_rodata_46C8;
    soWorkManageModule* work = &itWork(item);
    int bullets = work->getInt(0x10000000);
    if (bullets <= 0) {
        return 0;
    }
    if (item->getEmbeddedModuleAccesser().getStatusModule().getStatusKind() != 2) {
        return 0;
    }
    Vec3f pos = item->m_moduleAccesser->getModelModule().getNodeGlobalPosition(2, false);
    pos.m_z = pool[0x14 / 4];
    BaseItem* star = itManager::getInstance()->createItem(&item->getHavedParentSafePos(), &pos, lr, static_cast<itKind>(0x3B), 0, item->m_taskId);
    if (star != NULL) {
        star->m_moduleAccesser->getGroundModule().setTestCollStop(true, 0);
        star->setTeamOwnerTaskId(item);
        star->updateOwnerScale(item);
        Vec3f speed(pool[0x48 / 4] * lr, pool[0x14 / 4], pool[0x14 / 4]);
        float power = pool[0];
        switch (kind) {
        case 0:
            power = item->getParamFloat(0x1AF7);
            speed.m_x *= item->getParamFloat(0x1AF5);
            break;
        case 1:
            power = item->getParamFloat(0x1AF8);
            speed.m_x *= item->getParamFloat(0x1AF6);
            break;
        }
        star->throwAttack(lr, &speed, power);
        if (bullets > 0 && --bullets == 0) {
            item->m_moduleAccesser->getMotionModule().addOtherAnim(0, 8, false, pool[0x14 / 4], pool[0]);
            item->m_moduleAccesser->getMotionModule().setOtherAnimRate(0, pool[0x14 / 4]);
            item->m_moduleAccesser->getMotionModule().setOtherAnimFrame(pool[0], 0);
        }
    }
    work->setInt(bullets, 0x10000000);
    return bullets;
}

// Fires a shot (item 0x2D) from node 1; an empty gun plays the empty motion.
// TODO: clean up - MATCH-ONLY: the literals are read through the unit's pool label (46C8 + 0x14: 0, + 0x2C: 5, + 0x34: 8,
// + 0x38: 1.6).
u32 itRayGunCustomizer::onShoot(BaseItem* item, u32, float lr, float) {
    const float* pool = lbl_27_rodata_46C8;
    soWorkManageModule* work = &itWork(item);
    int bullets = work->getInt(0x10000000);
    if (bullets <= 0) {
        item->changeMotion(8, false);
        return 0;
    }
    Vec3f pos = item->m_moduleAccesser->getModelModule().getNodeGlobalPosition(1, false);
    float scale = item->getLinkOwnerScale();
    pos.m_x += pool[0x34 / 4] * lr * scale;
    pos.m_y += pool[0x38 / 4] * scale;
    pos.m_z = pool[0x14 / 4];
    BaseItem* shot = itManager::getInstance()->createItem(&item->getHavedParentSafePos(), &pos, lr, static_cast<itKind>(0x2D), 0, item->m_taskId);
    if (shot != NULL) {
        shot->m_moduleAccesser->getGroundModule().setTestCollStop(true, 0);
        shot->setTeamOwnerTaskId(item);
        shot->updateOwnerScale(item);
        Vec3f speed(pool[0x2C / 4] * lr, pool[0x14 / 4], pool[0x14 / 4]);
        shot->m_moduleAccesser->getKineticModule().addSpeed(&speed, shot->m_moduleAccesser);
        if (--bullets <= 0) {
            bullets = 0;
        }
        item->changeMotion(7, false);
    }
    work->setInt(bullets, 0x10000000);
    return bullets;
}

// Throws a ball (item 0xE) from node 4 at the given angle (degrees from up) and the speed in float work 0x11000000.
// TODO: clean up - MATCH-ONLY: the literals are read through the unit's pool label (46C8 + 0: 1, + 8: pi/180, + 0x14: 0,
// + 0x30: pi/2, + 0x44: 2).
u32 itClackerCustomizer::onShoot(BaseItem* item, u32, float lr, float angle) {
    const float* pool = lbl_27_rodata_46C8;
    soWorkManageModule* work = &itWork(item);
    int bullets = work->getInt(0x10000000);
    if (bullets <= 0) {
        item->changeMotion(7, false);
        return 0;
    }
    float scale = item->getLinkOwnerScale() - pool[0];
    if (scale < pool[0x14 / 4]) {
        scale = pool[0x14 / 4];
    }
    Vec3f offset(pool[0x44 / 4] * scale, pool[0x14 / 4], pool[0x14 / 4]);
    Vec3f pos;
    pos = item->m_moduleAccesser->getModelModule().getNodeGlobalPosition(4, &offset, false, false);
    pos.m_z = pool[0x14 / 4];
    BaseItem* ball = itManager::getInstance()->createItem(&item->getHavedParentSafePos(), &pos, lr, static_cast<itKind>(0xE), 0, item->m_taskId);
    if (ball != NULL) {
        ball->m_moduleAccesser->getGroundModule().setTestCollStop(true, 0);
        ball->setTeamOwnerTaskId(item);
        ball->updateOwnerScale(item);
        float speed = work->getFloat(0x11000000);
        Vec3f dir(pool[0x14 / 4], pool[0x14 / 4], pool[0x14 / 4]);
        mtSinCosf(pool[0x30 / 4] + -(pool[8 / 4] * angle), &dir.m_y, &dir.m_x);
        dir.m_x *= speed * lr;
        dir.m_y *= speed;
        ball->m_moduleAccesser->getKineticModule().addSpeed(&dir, ball->m_moduleAccesser);
        if (bullets == 1) {
            ball->changeMotion(1, false);
        }
        if (--bullets <= 0) {
            bullets = 0;
        }
    }
    work->setInt(bullets, 0x10000000);
    return bullets;
}

// Shoots a fireball (item 0x19) from node 4 with a random speed (params 0x1482-0x1483) and angle (0x1484-0x1485).
// TODO: clean up - MATCH-ONLY: the literals are read through the unit's pool label (46C8 + 8: pi/180, + 0x14: 0,
// + 0x30: pi/2).
u32 itFireFlowerCustomizer::onShoot(BaseItem* item, u32, float lr, float) {
    const float* pool = lbl_27_rodata_46C8;
    soWorkManageModule* work = &itWork(item);
    int bullets = work->getInt(0x10000000);
    if (bullets <= 0) {
        return 0;
    }
    Vec3f pos = item->m_moduleAccesser->getModelModule().getNodeGlobalPosition(4, false);
    pos.m_z = pool[0x14 / 4];
    BaseItem* shot = itManager::getInstance()->createItem(&item->getHavedParentSafePos(), &pos, lr, static_cast<itKind>(0x19), 0, item->m_taskId);
    if (shot != NULL) {
        shot->m_moduleAccesser->getGroundModule().setTestCollStop(true, 0);
        shot->setTeamOwnerTaskId(item);
        shot->updateOwnerScale(item);
        float speedMin = shot->getParamFloat(0x1482);
        float speedMax = shot->getParamFloat(0x1483);
        float speed = speedMin + (speedMax - speedMin) * randf();
        float angleMin = shot->getParamFloat(0x1484);
        float angleMax = shot->getParamFloat(0x1485);
        float angle = angleMin + (angleMax - angleMin) * randf();
        Vec3f dir(pool[0x14 / 4], pool[0x14 / 4], pool[0x14 / 4]);
        mtSinCosf(pool[0x30 / 4] + -(pool[8 / 4] * angle), &dir.m_y, &dir.m_x);
        dir.m_x *= speed * lr;
        dir.m_y *= speed;
        shot->m_moduleAccesser->getKineticModule().addSpeed(&dir, shot->m_moduleAccesser);
        if (--bullets <= 0) {
            bullets = 0;
            item->changeMotion(10, false);
        }
    }
    work->setInt(bullets, 0x10000000);
    return bullets;
}

// Swinging the stick launches a shot (item 0x30) from node 2; swing kinds 0 and 1 have their own power and speed params.
u32 itRipStickCustomizer::onShoot(BaseItem* item, u32 kind, float lr, float) {
    soWorkManageModule* work = &itWork(item);
    int bullets = work->getInt(0x10000000);
    if (bullets <= 0) {
        return 0;
    }
    Vec3f pos = item->m_moduleAccesser->getModelModule().getNodeGlobalPosition(2, false);
    pos.m_z = lbl_27_rodata_46DC[0];
    BaseItem* shot = itManager::getInstance()->createItem(&item->getHavedParentSafePos(), &pos, lr, static_cast<itKind>(0x30), 0, item->m_taskId);
    if (shot != NULL) {
        shot->m_moduleAccesser->getGroundModule().setTestCollStop(true, 0);
        shot->setTeamOwnerTaskId(item);
        shot->updateOwnerScale(item);
        float power = lbl_27_rodata_46C8[0];
        Vec3f speed(lr * item->getParamFloat(0x189C), lbl_27_rodata_46DC[0], lbl_27_rodata_46DC[0]);
        switch (kind) {
        case 0:
            power = item->getParamFloat(0x189F);
            speed.m_x *= item->getParamFloat(0x189D);
            break;
        case 1:
            power = item->getParamFloat(0x18A0);
            speed.m_x *= item->getParamFloat(0x189E);
            break;
        }
        shot->throwAttack(lr, &speed, power);
        if (--bullets <= 0) {
            bullets = 0;
        }
        item->changeMotion(7, false);
    }
    work->setInt(bullets, 0x10000000);
    return bullets;
}

extern "C" const float lbl_27_rodata_46F4[];

// Fires a shot (item 0x3E) whose size, speed and ammunition cost follow the charge level (params 0x69AB-0x69B6 and
// 0x1B8A-0x1B9D).
u32 itSuperScopeCustomizer::onShoot(BaseItem* item, u32 charge, float lr, float) {
    soWorkManageModule* work = &itWork(item);
    int bullets = work->getInt(0x10000000);
    if (bullets <= 0) {
        item->changeMotion(7, false);
        return 0;
    }
    Vec3f pos = item->m_moduleAccesser->getModelModule().getNodeGlobalPosition(1, false);
    pos.m_x += lbl_27_rodata_46F4[0] * lr * item->getLinkOwnerScale();
    pos.m_z = lbl_27_rodata_46DC[0];
    BaseItem* shot = itManager::getInstance()->createItem(&item->getHavedParentSafePos(), &pos, lr, static_cast<itKind>(0x3E), 0, item->m_taskId);
    if (shot != NULL) {
        shot->m_moduleAccesser->getGroundModule().setTestCollStop(true, 0);
        shot->setTeamOwnerTaskId(item);
        shot->updateOwnerScale(item);
        int level;
        if (static_cast<int>(charge) <= item->getParamInt(0x69AB)) {
            level = 0;
        } else if (static_cast<int>(charge) <= item->getParamInt(0x69AC) * 8) {
            level = static_cast<int>(charge) / item->getParamInt(0x69AC) + 1;
        } else {
            level = 9;
        }
        if (level >= 0 && level <= 9) {
            int paramIndex = level * 2;
            double speedParam = item->getParamFloat(0x1B8B + paramIndex);
            Vec3f speed(speedParam * lr, lbl_27_rodata_46DC[0], lbl_27_rodata_46DC[0]);
            shot->m_moduleAccesser->getKineticModule().addSpeed(&speed, shot->m_moduleAccesser);
            shot->m_moduleAccesser->getPostureModule().setScale(item->getParamFloat(0x1B8A + paramIndex));
            shot->m_moduleAccesser->getGroundModule().setShapeFlag(static_cast<soGroundShapeImpl::ShapeFlagId>(1), true, 0);
            shot->updateNodeSRT();
            shot->changeMotion(level, false);
            bullets -= item->getParamInt(0x69AD + level);
            if (bullets <= 0) {
                bullets = 0;
            }
        }
    }
    work->setInt(bullets, 0x10000000);
    return bullets;
}

// TODO: clean up - MATCH-ONLY: soAnimCmdArgList::getFloat returns a float here (the shadow header declares double), so
// it is called through its symbol.
extern "C" float getFloat__16soAnimCmdArgListFUl(itAnimCmdArgList* args, u32 index);

// Anim cmd type 0x11 with seven arguments shows a message in Mr. Resetti's window (messages 0x4F and 0x6F name someone).
bool itResetsanCustomizer::onNotifyEventAnimCmd(BaseItem* item, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3) {
    switch (acmd->getType()) {
    case 0x11:
        if (acmd->getArgNum() == 7) {
            itAnimCmdArgList args(moduleAccesser, acmd->getArgList());
            int msgId = args.getInt(0);
            float delay1 = getFloat__16soAnimCmdArgListFUl(&args, 1);
            float delay3 = getFloat__16soAnimCmdArgListFUl(&args, 2);
            float delay4 = getFloat__16soAnimCmdArgListFUl(&args, 3);
            float delay2 = getFloat__16soAnimCmdArgListFUl(&args, 4);
            float wait = getFloat__16soAnimCmdArgListFUl(&args, 5);
            float posY = getFloat__16soAnimCmdArgListFUl(&args, 6);
            if (!args.m_errorOnValueFetch) {
                // TODO: clean up - the window's task id is a BaseItem field that is not declared yet (0x3D2C).
                u32 taskId = *reinterpret_cast<u32*>(reinterpret_cast<u8*>(item) + 0x3D2C);
                if (taskId != 0xFFFFFFFF) {
                    IfResetsanTask* task = dynamic_cast<IfResetsanTask*>(gfTask::getTask(taskId));
                    if (task != NULL) {
                        if (msgId == 0x4F || msgId == 0x6F) {
                            int kind1;
                            int kind2;
                            const char* str1 = NULL;
                            const char* str2 = NULL;
                            const wchar_t* wstr1 = NULL;
                            const wchar_t* wstr2 = NULL;
                            soWorkManageModule* work = &itWork(item);
                            getMessageNames(&kind1, &str1, &wstr1, &kind2, &str2, &wstr2);
                            work->setInt(kind1, 0x10000002);
                            task->m_nameKind1 = kind1;
                            task->m_nameStr1 = str1;
                            task->m_nameWStr1 = wstr1;
                            task->m_nameKind2 = kind2;
                            task->m_nameStr2 = str2;
                            task->m_nameWStr2 = wstr2;
                        }
                        if (msgId == 4) {
                            gmSetUseRisetsan();
                        }
                        task->setMessage(msgId, delay1, delay3, delay4, delay2, wait, posY);
                        return true;
                    }
                }
            }
        }
        break;
    }
    return false;
}

// The link event (kind 0x34) the Gooey Bomb sends what it holds when it lets go.
struct itGokulinLinkEventArgs : public soLinkEventArgs {
    itGokulinLinkEventArgs() : soLinkEventArgs(0x34) { }
};

// Lets go of whatever the bomb holds (link 0).
static inline void itGokulinRelease(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().isLinked(0) == true) {
        itGokulinLinkEventArgs args;
        moduleAccesser->getLinkModule().sendEventNodes(0, args, 0);
    }
    if (moduleAccesser->getLinkModule().isLink(0) == true) {
        moduleAccesser->getLinkModule().removeModelConstraint(true);
        moduleAccesser->getLinkModule().unlink(0);
    }
    static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->offCatchFlag();
}

void itGokulinCustomizer::onDestruct(BaseItem* item) {
    if (item->m_moduleAccesser->getLinkModule().isLinked(0) == true) {
        itGokulinLinkEventArgs args;
        item->m_moduleAccesser->getLinkModule().sendEventNodes(0, args, 0);
    }
    if (item->m_moduleAccesser->getLinkModule().isLink(0) == true) {
        item->m_moduleAccesser->getLinkModule().removeModelConstraint(true);
        item->m_moduleAccesser->getLinkModule().unlink(0);
    }
    static_cast<soCatchModule*>(item->m_moduleAccesser->m_enumerationStart->m_catchModule)->offCatchFlag();
}

// Status 0xA (stuck to someone) marks its link 4 and clears the float work 0x11000003; other statuses release the
// holder unless flag 0x12000002 is set.
void itGokulinCustomizer::onNotifyEventChangeStatus(BaseItem* item, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    if (statusKind == 0xA) {
        if (moduleAccesser->getLinkModule().isLink(4) == true) {
            moduleAccesser->getLinkModule().setAttribute(4, static_cast<soLinkConnection::Attribute>(4), true);
            itWork(item).setFloat(lbl_27_rodata_46DC[0], 0x11000003);
        }
    } else if (moduleAccesser->getLinkModule().isLinked(0) && !itWork(item).isFlag(0x12000002)) {
        itGokulinRelease(moduleAccesser);
    }
}

// Ends Tingle's effect (int work 0x10000000): effect 2 restores the other fighters' footing, effect 4 the owner's zoom.
// TODO: clean up - placeholder name of this non-virtual itTingleCustomizer helper (0x29EF44), defined with C linkage so
// the hooks' calls keep its symbol; ftManager::enumEntryId is declared with an int but takes the entry id array.
extern "C" void fn_27_29EF44(itTingleCustomizer* self, BaseItem* item) {
    soWorkManageModule* work = &itWork(item);
    int effect = work->getInt(0x10000000);
    if (effect < 0) {
        return;
    }
    int entryIds[8];
    int entryCount = g_ftManager->enumEntryId(reinterpret_cast<int>(entryIds));
    u32 ownerTaskId = item->m_moduleAccesser->getTeamModule().getTeamOwnerId();
    switch (effect) {
    case 2:
        for (int i = 0; i < entryCount; i++) {
            int entryId = entryIds[i];
            if (g_ftManager->isValidEntryId(entryId) && entryId != g_ftManager->getEntryIdFromTaskId(ownerTaskId, NULL)) {
                g_ftManager->getOwner(entryId)->setSlipMul(lbl_27_rodata_46C8[0]);
                g_ftManager->getOwner(entryId)->setSlipInterval(true);
            }
        }
        break;
    case 4:
        if (ownerTaskId != 0xFFFFFFFF) {
            int entryId = g_ftManager->getEntryIdFromTaskId(ownerTaskId, NULL);
            if (g_ftManager->isValidEntryId(entryId)) {
                g_ftManager->setZoom(entryId, lbl_27_rodata_46C8[0], 0, 1);
            }
        }
        break;
    }
    work->setInt(-1, 0x10000000);
}

void itTingleCustomizer::onDestruct(BaseItem* item) { fn_27_29EF44(this, item); }

extern "C" const float lbl_27_rodata_4790[];

// Starts Tingle's effect once flag 0x12000000 is set: 1 gives everyone curry, 2 makes the others slip, 4 zooms on the
// owner. Flag 0x12000001 says whether the owner can be helped (no Final Smash running and an owner exists).
bool itTingleCustomizer::onProcessFixPosition(BaseItem* item) {
    if (!soSlow::getInstance()->isAdjust()) {
        return true;
    }
    if (item->getEmbeddedModuleAccesser().getStatusModule().getStatusKind() == 6) {
        fn_27_29EF44(this, item);
        return true;
    }
    soWorkManageModule* work = &itWork(item);
    u32 ownerTaskId = item->m_moduleAccesser->getTeamModule().getTeamOwnerId();
    bool hasOwner = false;
    if (!g_ftManager->isProcessTechnique() && ownerTaskId != 0xFFFFFFFF) {
        hasOwner = true;
    }
    work->setFlag(hasOwner, 0x12000001);
    if (!work->isFlag(0x12000000)) {
        return true;
    }
    work->offFlag(0x12000000);
    int effect = work->getInt(0x10000000);
    if (effect < 0) {
        return true;
    }
    int entryIds[8];
    int entryCount = g_ftManager->enumEntryId(reinterpret_cast<int>(entryIds));
    switch (effect) {
    case 1:
        for (int i = 0; i < entryCount; i++) {
            g_ftManager->setCurry(entryIds[i]);
        }
        break;
    case 2:
        for (int i = 0; i < entryCount; i++) {
            int entryId = entryIds[i];
            if (entryId != g_ftManager->getEntryIdFromTaskId(ownerTaskId, NULL)) {
                g_ftManager->getOwner(entryId)->setSlipMul(lbl_27_rodata_4790[0]);
                g_ftManager->getOwner(entryId)->setSlipInterval(false);
            }
        }
        break;
    case 4:
        if (hasOwner) {
            g_ftManager->setZoom(g_ftManager->getEntryIdFromTaskId(ownerTaskId, NULL), lbl_27_rodata_46C8[0], 1, 0x3C);
        }
        break;
    }
    return true;
}
