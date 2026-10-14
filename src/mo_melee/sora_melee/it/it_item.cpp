// BaseItem: the base class of every item (food, bombs, capsules, trophies' stand-ins, Pokeballs, ...). It is a
// StageObject that carries its own module set inline and asks an itCustomizer (one per item kind) for the behavior that
// differs between kinds: most of the small members below just forward to a module that is embedded in the item or to
// the customizer.
//
// Unit boundary (sora_melee): .text 0x27F4D8-0x29065C (item.cpp: the camera helpers, BaseItem and the weak helpers its
// constructor instantiates).

#define IT_CUSTOMIZER_BOOL_HOOKS // the default hooks below return values, see it/item.h
#include <it/item.h>
#include <so/team/so_team.h>
#include <so/event/so_event_manage_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/param/so_param_customize_module_impl.h>

// MATCH-ONLY: it_manager.h cannot be included here (its customizer declarations disagree with the bool hooks of
// itCustomizerInterface below), so the two itManager members BaseItem uses are declared on their own.
class itManager {
public:
    static itManager* getInstance();
    void removeItem(BaseItem* item);
};

struct PostureView {
    u8 pad[0xDB4];
    soPostureModuleImpl posture;
};

namespace {
// HYPOTHESIS: the modules that BaseItem::BaseItem builds inline, by offset in the item (module accesser at 0xCC).
const int kEventManageModule = 0x264;
const int kSituationModule = 0xF04;
const int kPostureModule = 0xDB4;
const int kTeamModule = 0xF60;
const int kLinkModule = 0x27A8;
const int kVisibilityModule = 0x27FC;
const int kWorkManageModule = 0x2AAC;
const int kStatusModule = 0x2C34;
const int kKineticModule = 0x3474;
const int kParamCustomizeModule = 0x3D10;

template <class T>
inline T& embedded(BaseItem* item, int offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<u8*>(item) + offset);
}
} // namespace

// Work value ids the item keeps in its work manage module (see BaseItem::Instance::Work).
itKind BaseItem::getItKind() {
    return m_kind;
}

u32 BaseItem::getItVariation() {
    return m_variation;
}

itCustomizerInterface* BaseItem::getCustomizer() {
    return m_customizer;
}

int BaseItem::getArticleId() {
    return m_articleId;
}

s16 BaseItem::getArticleEventManageId() {
    return embedded<soEventManageModuleImpl>(this, kEventManageModule).getManageId();
}

int BaseItem::getTaskId() {
    return m_taskId;
}

int BaseItem::getLotCreateItKind() {
    return m_lotCreateKind;
}

bool BaseItem::isSyncOwnerStatus() {
    return false;
}

void BaseItem::setSyncOwnerStatus(bool) { }

soKind BaseItem::soGetKind() {
    return static_cast<soKind>(4);
}

void BaseItem::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { }
void BaseItem::notifyEventCollisionShield(soCollisionAttackModule*, float, soCollisionLog*, int, float, float, soModuleAccesser*) { }

// ---- Forwarders to the embedded modules -----------------------------------------------------------------------------

void BaseItem::changeStatus(int statusKind) {
    embedded<soStatusModule>(this, kStatusModule).changeStatusRequest(statusKind, &moduleAccesser);
}

// The item is lost (it times out, is thrown away, ...) / is born (a lot-created item appears): both are status requests.
void BaseItem::requestLost() {
    embedded<soStatusModule>(this, kStatusModule).changeStatusRequest(Status::Lost, &moduleAccesser);
}

void BaseItem::born() {
    embedded<soStatusModule>(this, kStatusModule).changeStatusRequest(Status::Born, &moduleAccesser);
}

u8 BaseItem::getVisibilityWhole() {
    return embedded<soVisibilityModule>(this, kVisibilityModule).getWhole();
}

void BaseItem::setSituationKind(SituationKind kind) {
    embedded<soSituationModule>(this, kSituationModule).setKind(kind, false);
}

bool BaseItem::isConstraint() {
    return embedded<soLinkModule>(this, kLinkModule).isModelConstraint();
}

void BaseItem::setConstraintTargetNode(int node) {
    moduleAccesser.getLinkModule().setModelConstraintTargetNode(node);
}

void BaseItem::setAttackPowerMulPattern(float pattern) {
    embedded<soParamCustomizeModule>(this, kParamCustomizeModule).setAttackPowerMulPattern(pattern);
}

bool BaseItem::isActiveArticle() {
    return isActive();
}

void BaseItem::deactivateArticle() {
    deactivate();
}

bool BaseItem::isVanishMode() {
    return embedded<soWorkManageModule>(this, kWorkManageModule).isFlag(Instance::Work::Flag_Vanish);
}

void BaseItem::setVanishMode(bool vanish) {
    embedded<soWorkManageModule>(this, kWorkManageModule).setFlag(vanish, Instance::Work::Flag_Vanish);
}

void BaseItem::linkOwner(int index, int taskId) {
    embedded<soLinkModule>(this, kLinkModule).link(index, taskId);
}

float BaseItem::getInhaledCount() {
    return embedded<soWorkManageModule>(this, kWorkManageModule).getFloat(Instance::Work::Float_Inhaled_Count);
}

void BaseItem::setSmashPlateFigurelized(int figurelized) {
    embedded<soWorkManageModule>(this, kWorkManageModule).setInt(figurelized, Instance::Work::Int_Message_Enemy);
}

void BaseItem::resetRotation(Vec3f* rotation) {
    embedded<soPostureModule>(this, kPostureModule).setRot(rotation, 0);
}

Vec3f BaseItem::getPos() {
    return embedded<soPostureModule>(this, kPostureModule).getPos();
}

Vec3f BaseItem::getRot() {
    return embedded<soPostureModule>(this, kPostureModule).getRot(0);
}

int BaseItem::getTeamOwnerTaskId() {
    return embedded<soTeamModule>(this, kTeamModule).getTeamOwnerId();
}

// ---- Forwarders to the customizer (the per item kind behavior) ------------------------------------------------------

void BaseItem::onUpdateAiMove(Vec2f* a, Vec2f* b, Vec2f* c) {
    m_customizer->onUpdateAiMove(this, a, b, c);
}

void BaseItem::onUpdateKinetic() {
    m_customizer->onUpdateKinetic(this);
}

void BaseItem::onPreBound(Vec2f* speed, BoundMask mask) {
    m_customizer->onPreBound(this, speed, mask);
}

bool BaseItem::onPreDamageCheck(int damageStatusKind, soDamageLog* log) {
    return m_customizer->onPreDamageCheck(this, damageStatusKind, log);
}

void BaseItem::onDamage(int damageStatusKind, soDamageLog* log) {
    m_customizer->onDamage(this, damageStatusKind, log);
}

bool BaseItem::isUseTurnDamage() {
    return m_customizer->isUseTurnDamage(this);
}

bool BaseItem::isUseSpeedDamage() {
    return m_customizer->isUseSpeedDamage(this);
}

bool BaseItem::isUseShake() {
    return m_customizer->isUseShake(this);
}

bool BaseItem::isTreadPassive() {
    return m_customizer->isTreadPassive(this);
}

bool BaseItem::isSafe() {
    return m_customizer->isSafe(this);
}

void BaseItem::notifyEventGimmick(soGimmickEventArgs* eventInfo, int* taskId) {
    m_customizer->onNotifyEventGimmick(this, eventInfo, taskId);
}

int BaseItem::getBullet() {
    return m_customizer->onGetBullet(this);
}

u32 BaseItem::shootBlanks(u32 count) {
    return m_customizer->onShootBlanks(this, count);
}

void BaseItem::action(u32 kind, float rate) {
    m_customizer->onAction(this, kind, rate);
}

int BaseItem::getResult() {
    return m_customizer->onResult(this);
}

// ---- Embedded module helpers, part two -------------------------------------------------------------------------------

// Bit n of this byte says that fighter / player n has held the item (see setHavedLog).
void BaseItem::setHavedLog(int index, bool haved) {
    u8& log = *(reinterpret_cast<u8*>(this) + 0x8D4);
    if (haved) {
        log |= (1 << index);
    } else {
        log &= ~(1 << index);
    }
}

int BaseItem::getTeamNo() {
    return embedded<soTeamModule>(this, kTeamModule).getTeam()->getNo();
}

bool BaseItem::isSmashPlateFigurelized() {
    return embedded<soWorkManageModule>(this, kWorkManageModule).getInt(Instance::Work::Int_Message_Enemy) != 0;
}

// An item that is being sucked in (Kirby's inhale) counts its inhale frames in a float work value; -1 means not inhaled.
bool BaseItem::isInhaled() {
    return embedded<soWorkManageModule>(this, kWorkManageModule).getFloat(Instance::Work::Float_Inhaled_Count) >= 0.0f;
}

// Moves the item to a position: the posture follows and the model node is rewritten.
void BaseItem::move(Vec3f* pos) {
    embedded<soPostureModule>(this, kPostureModule).setPos(pos);
    updateNodeSRT();
}

void BaseItem::resetLr(float lr) {
    PostureView* view = reinterpret_cast<PostureView*>(this);
    view->posture.setLr(lr);
    view->posture.updateRotYLr();
}

// 'e' is the extra animation command group that items observe on top of the StageObject default.
bool BaseItem::isObserv(char group) {
    return StageObject::isObserv(group) || group == 'e';
}

void BaseItem::remove() {
    itManager::getInstance()->removeItem(this);
}

// Back to the standby status; force skips the transition check.
void BaseItem::standby(bool force) {
    if (force) {
        embedded<soStatusModule>(this, kStatusModule).changeStatusForce(Status::Standby, &moduleAccesser);
    } else {
        embedded<soStatusModule>(this, kStatusModule).changeStatusRequest(Status::Standby, &moduleAccesser);
    }
}

// Held by a fighter: either in the Have status or linked to a holder (link number 3).
bool BaseItem::isHaved() {
    if (moduleAccesser.getStatusModule().getStatusKind() == Status::Have) {
        return true;
    }
    return embedded<soLinkModule>(this, kLinkModule).isLink(Link::No_Have);
}

void BaseItem::renderDebug() {
    StageObject::renderDebug();
    m_customizer->onRenderDebug(this);
}

// ---- itCustomizerInterface: what an item does when its kind does not override a hook ---------------------------------
// An item that does not customize anything (itNullCustomizer) behaves like this: no per-frame work, the damage and
// speed-damage hooks accept the hit, the item uses its z scale, can be held by the usual constraint, is safe to pick up
// and has no bullets / results.

void itCustomizerInterface::onConstruct(BaseItem*) { }
void itCustomizerInterface::onDestruct(BaseItem*) { }
void itCustomizerInterface::onReset(BaseItem*) { }
void itCustomizerInterface::onProcessUpdate(BaseItem*) { }
bool itCustomizerInterface::onProcessFixPosition(BaseItem*) { return true; }
void itCustomizerInterface::onProcessHit(BaseItem*) { }
void itCustomizerInterface::onProcessFixCamera(BaseItem*) { }
void itCustomizerInterface::onProcessGameProc(BaseItem*) { }
void itCustomizerInterface::onRenderPre(BaseItem*) { }
void itCustomizerInterface::onRenderDebug(BaseItem*) { }
void itCustomizerInterface::onNotifyEventChangeStatus(BaseItem*, int, int, soStatusData*, soModuleAccesser*) { }
bool itCustomizerInterface::onNotifyEventAnimCmd(BaseItem*, acAnimCmd*, soModuleAccesser*, int) { return false; }
void itCustomizerInterface::onNotifyEventCollisionAttackCheck(BaseItem*, int) { }
bool itCustomizerInterface::onNotifyEventCollisionSearch(BaseItem*, soCollisionLog*) { return true; }
bool itCustomizerInterface::onNotifyEventCollisionSearchCheck(BaseItem*, int, u8, Vec3f*) { return false; }
void itCustomizerInterface::onNotifyEventLink(BaseItem*, void*, soModuleAccesser*, StageObject*, int) { }
bool itCustomizerInterface::onNotifyEventCaptureStatus(BaseItem*) { return false; }
void itCustomizerInterface::onNotifyEventGimmick(BaseItem*, soGimmickEventArgs*, int*) { }
void itCustomizerInterface::onUpdateKinetic(BaseItem*) { }
void itCustomizerInterface::onUpdateAiDir(BaseItem*, Vec2f*, Vec2f*, Vec2f*, Vec2f*) { }
void itCustomizerInterface::onUpdateAiMove(BaseItem*, Vec2f*, Vec2f*, Vec2f*) { }
bool itCustomizerInterface::onSearchTarget(BaseItem*, int*, Vec3f, int) { return false; }
u32 itCustomizerInterface::onShoot(BaseItem*, u32, float, float) { return 0; }
u32 itCustomizerInterface::onShootBlanks(BaseItem*, u32) { return 0; }
int itCustomizerInterface::onGetBullet(BaseItem*) { return 0; }
void itCustomizerInterface::onThrowAttack(BaseItem*, float, float*, Vec3f*, Vec3f*, float*, bool*) { }
void itCustomizerInterface::onAction(BaseItem*, u32, float) { }
int itCustomizerInterface::onResult(BaseItem*) { return -1; }
bool itCustomizerInterface::onHave(BaseItem*, int) { return true; }
bool itCustomizerInterface::onPreDamageCheck(BaseItem*, int, soDamageLog*) { return true; }
bool itCustomizerInterface::onDamage(BaseItem*, int, soDamageLog*) { return true; }
bool itCustomizerInterface::onReflect(BaseItem*) { return false; }
void itCustomizerInterface::onPreBound(BaseItem*, Vec2f*, BaseItem::BoundMask) { }
bool itCustomizerInterface::onRemoveModelConstraint(BaseItem*, int) { return true; }
bool itCustomizerInterface::onEnumFighter(BaseItem*, Fighter*) { return true; }
bool itCustomizerInterface::isTreadPassive(BaseItem*) { return false; }
bool itCustomizerInterface::isUseTurnDamage(BaseItem*) { return false; }
bool itCustomizerInterface::isUseSpeedDamage(BaseItem*) { return true; }
bool itCustomizerInterface::isUseShake(BaseItem*) { return true; }
bool itCustomizerInterface::isUseScaleZ(BaseItem*) { return true; }
bool itCustomizerInterface::isUseThrowRotParam(BaseItem*) { return true; }
bool itCustomizerInterface::isSafe(BaseItem*) { return true; }
bool itCustomizerInterface::isConstraintHave(BaseItem*) { return true; }
bool itCustomizerInterface::isReferenceControlerHave(BaseItem*) { return false; }
itParam::KineticFlag itCustomizerInterface::getKineticFlags(BaseItem*) { return itParam::KineticFlag(0); }
bool itCustomizerInterface::isUsePhysics(BaseItem*) { return false; }
