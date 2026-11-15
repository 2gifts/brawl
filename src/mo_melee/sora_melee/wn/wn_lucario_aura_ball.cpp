#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define WN_MARIO_FIREBALL_UNIT // the sora_melee article unit mode shared with the Mario fireball (builders out of line)
#define WN_ZELDA_LIGHTING_BOW_UNIT // the member layout and constructor of the module builder of this unit mode (wn/zelda/wn_zelda_lighting_bow.h)
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_STATUS_NO_TAIL
#define WN_TRANSITION_CTOR_INLINE
#define FT_POOL_ROOT_NONPOLYMORPHIC // BYTES: without it the hierarchy destructor is never_inline
#define WN_ANIMCMD_RESET_MODE_BOX // BYTES: word-aligned reset-mode temporaries (frame size of the anim cmd sub builder)
#define WN_ANIMCMD_PRESENTER_LOCAL_STORE // the interpreters' presenters take the list from the lookup as a value (so_event_presenter.h)
#define WN_ANIMCMD_STACK_PTR // BYTES: the interpreter stacks stay in a saved register across getManageId (ft_builder_animcmd.h)
#define WN_ARTICLE_ANIMCMD_INT // BYTES: the builder vtable takes Weapon's int-index notifyEventAnimCmd thunk (@72@), as in the original
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#define MT_VEC3F_MUL_INLINE
#define MT_VEC3F_ADD_OPERAND_ORDER_REGS // BYTES: register order of the paired-single vector sum (mt_vector.h)
#include <wn/wn_module_accesser_builder.h>
#include <wn/zelda/wn_zelda_lighting_bow.h>
#include <wn/lucario/wn_lucario_aura_ball.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_kinetic_energy_rot_normal.h>
#include <so/so_module_accesser.h>
#include <so/link/so_link_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <ft/ft_common_data_accesser.h>
#include <nw4r/math/math_arithmetic.h>
#include <memory.h>
#include <gf/gf_task_scheduler.h>
#include <so/so_external_value_accesser.h>
#include <math.h>

// BYTES: the empty base destructors that sora_melee always expands (they have no out-of-line copy there).
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }

// Registers the Lucario aura ball article class (kind 99) in the class info table during static initialization.
wnClassInfoImpl<99, wnLucarioAuraBall, wnLucarioAuraBallData> g_wnLucarioAuraBallClassInfo;

// HYPOTHESIS: the Lucario aura ball charge record of the fighter data (only the read fields).
struct ftLucarioAuraBallParam {
    s32 m_chargeLevelNum; // +0x0
    u8 unk4[0xC];
    s32 m_chargeLevelFrame; // +0x10
    u8 unk14[0x4];
    float m_scaleRate;  // +0x18
    float m_scaleBase;  // +0x1C
};


// HYPOTHESIS: inline clamp helper (fsel based), as in ft_status_uniq_process_glide.cpp.
static inline float clampf(float value, float lo, float hi) {
    float diff;
    diff = value - lo;
    value = nw4r::math::FSelect(diff, value, lo);
    diff = value - hi;
    value = nw4r::math::FSelect(diff, hi, value);
    return value;
}

extern soStatusUniqProcess lbl_27_bss_89E4;
extern soStatusUniqProcess lbl_27_bss_89F4;
extern soStatusUniqProcess lbl_27_bss_8A04;

wnLucarioAuraBall::wnLucarioAuraBall(s32 articleId, const wnArticleBuildInfo& info, const wnLucarioAuraBallData* data) :
    wnWeaponBuilder<wnLucarioAuraBallModuleAccesserBuildConfig>(
        "LucarioAuraBall", articleId, info,
        wnModuleAccesserBuildData(0x63, data, info.getHeapModule()->getInstanceHeap(),
            info.getHeapModule()->getNWModelInstanceHeap(), info.getHeapModule()->getNWMotionInstanceHeap(),
            static_cast<u8>(data->m_resGroupNo), -1, &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soModuleAccesser* acc = m_moduleAccesser;
    soStatusUniqProcess* uniqProcs[3];
    uniqProcs[0] = &lbl_27_bss_89E4;
    uniqProcs[1] = &lbl_27_bss_89F4;
    uniqProcs[2] = &lbl_27_bss_8A04;
    acc->getStatusModule().addRangeUniqProc(uniqProcs, 3);
    Weapon::deactivate(false);
}

#pragma push
#pragma pool_data off
template soMotionModuleBuilder<
    wnLucarioAuraBallModuleAccesserBuildConfig::MotionModuleBuildConfig>::soMotionModuleBuilder(soModuleAccesser*, void*);
#pragma pop

#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnLucarioAuraBallModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>

void wnLucarioAuraBall::activate(int founderTaskId, int team, float lr, int chargeFrame, float chargeScale, int fighterKind) {
    Vec3f zeroPos(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc(founderTaskId, 0xFFFF, 0xFFFF, 0xFFFF, -1, -1, 0, 0, zeroPos, lr, team, 0, 2, 0x80, 0, 0x35F,
                        0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    soModuleAccesser* acc = m_moduleAccesser;
    soWorkManageModule& work = acc->getWorkManageModule();
    work.setInt(chargeFrame, 0x10000005);
    work.setFloat(chargeScale, 0x11000009);
    work.setInt(fighterKind, 0x10000009);
    work.setFloat(1.0f, 0x1100000A);
    EfID effectId = static_cast<EfID>(fighterKind == 0x21 ? 0x220007 : 0x1230001);
    work.setInt(acc->getEffectModule().reqFollow(effectId, 0, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f,
                                                 false, 0, 0, -1),
                0x10000008);
    setEffectScale(acc);
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000000);
    acc->getStatusModule().changeStatusForce(0, acc);
    updateNodeSRT();
}

// The charge-record lookup and the effect removal are helpers over the module accesser.
static inline const ftLucarioAuraBallParam* getAuraBallParam(soModuleAccesser* acc) {
    return g_ftCommonDataAccesser.getLucarioAuraBallParam(acc->getWorkManageModule().getInt(0x10000009));
}

static inline void removeChargeEffect(soModuleAccesser* acc) {
    soWorkManageModule& work = acc->getWorkManageModule();
    int handle = work.getInt(0x10000008);
    if (handle != 0) {
        acc->getEffectModule().remove(handle);
        work.setInt(0, 0x10000008);
    }
}

void wnLucarioAuraBall::onDeactivate() {
    removeChargeEffect(m_moduleAccesser);
}

// The ball sits between Lucario's hands (or at the copying Kirby's hand node) while it is held.
static inline void holdAtHand(soModuleAccesser* acc, Vec3f* out) {
    soLinkModule& link = acc->getLinkModule();
    Vec3f pos;
    switch (acc->getWorkManageModule().getInt(0x10000009)) {
    case 0x21:
    default: {
        Vec3f first = link.getParentModelNodeGlobalPosition(3, 0x1f, false);
        Vec3f second = link.getParentModelNodeGlobalPosition(3, 0x35, false);
        pos = (first + second) * 0.5f;
        break;
    }
    case 5:
        pos = link.getParentModelNodeGlobalPosition(3, 0x1cb, false);
        break;
    }
    acc->getPostureModule().setPos(&pos);
    *out = pos;
}

void wnLucarioAuraBall::notifyEventCollisionAttack(float power, const soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    gfTask* task = gfTaskScheduler::getInstance()->getTaskById(static_cast<gfTask::Category>(collisionLog->m_taskCategory),
                                                                collisionLog->m_taskId);
    switch (collisionLog->m_kind) {
    case 2:
        unkA4Bit7 = 0;
        if (m_moduleAccesser->getSituationModule().getKind() == 2) {
            soCollisionAttackData* data =
                m_moduleAccesser->getCollisionAttackModule().getData(collisionLog->m_damageIndex, false);
            if (data->m_shapeType == soCollision::Shape_Sphere) {
                soCollisionShieldModule& shield =
                    *soExternalValueAccesser::getCollisionShieldModule(dynamic_cast<StageObject*>(task));
                if (shield.isHop(collisionLog->_35) == true) {
                    float hopCos;
                    float hopAngle = shield.getHopAngle(collisionLog->_35) * 0.017453292f;
                    hopCos = cos(hopAngle);
                    m_reflectDirX = hopCos * shield.getLR(0);
                    m_reflectDirY = sin(hopAngle);
                    unkA4Bit7 = 1;
                } else if (shield.isNoHop(collisionLog->_35) == false) {
                    Vec3f center = shield.getCenterPos(collisionLog->m_collsionIndex, collisionLog->_35);
                    Vec3f logPos;
                    logPos = collisionLog->m_pos;
                    Vec2f dir;
                    dir.m_x = logPos.m_x - center.m_x;
                    dir.m_y = logPos.m_y - center.m_y;
                    float length = dir.length();
                    if (length > 0.00001f) {
                        dir.m_x *= 1.0f / length;
                        dir.m_y *= 1.0f / length;
                        wnKineticEnergyNormal& energy =
                            dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
                        Vec2f speed = energy.getSpeed();
                        if (dir.angle(&speed) * 57.29578f < 135.0f) {
                            m_reflectDirX = dir.m_x;
                            m_reflectDirY = dir.m_y;
                            unkA4Bit7 = 1;
                        }
                    }
                }
            }
        }
        break;
    case 1:
        break;
    case 0:
        if (task != NULL) {
            float reboundX = 0.0f;
            float reboundY = 0.0f;
            if (moduleAccesser->getCollisionAttackModule().checkRebound(moduleAccesser, &dynamic_cast<StageObject&>(*task),
                                                                        collisionLog, power, &reboundX, &reboundY)) {
                return;
            }
        }
        break;
    }
}

bool wnLucarioAuraBall::notifyEventCollisionAttackCheck(u32 flags) {
    int status = m_moduleAccesser->getStatusModule().getStatusKind();
    switch (status) {
    case 0:
    case 1:
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

// Leaves the owner's hand: the ball keeps the hand position, takes the owner's depth and a neutral rotation.
static inline void releaseFromHand(soModuleAccesser* acc) {
    Vec3f pos;
    holdAtHand(acc, &pos);
    acc->getWorkManageModule().setFloat(pos.m_x, 0x11000001);
    acc->getWorkManageModule().setFloat(pos.m_y, 0x11000002);
    pos.m_z = acc->getGroundModule().getZ();
    soPostureModule& posture = acc->getPostureModule();
    posture.init(&pos, posture.getLr());
    Vec2f center = acc->getLinkModule().getParentShapeCenterPos(3);
    acc->getGroundModule().setShapeSafePos(&center, 0);
    Vec3f rot(0.0f, 0.0f, 0.0f);
    acc->getPostureModule().setRot(&rot, 0);
    acc->m_stageObject->updateNodeSRT();
}

// The launch speed grows with the charge between the article's two speed constants.
static inline void setLaunchSpeed(soModuleAccesser* acc, float lr) {
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*acc->getKineticModule().getEnergy(0));
    float angle = soValueAccesser::getConstantFloat(acc, 0xfa0, 0);
    float speedMin = soValueAccesser::getConstantFloat(acc, 0xfa1, 0);
    float speedMax = soValueAccesser::getConstantFloat(acc, 0xfa2, 0);
    float rate = wnLucarioAuraBall::getChargeRate(acc);
    float speed = speedMin + rate * (speedMax - speedMin);
    float vx = lr * (speed * static_cast<float>(cos(angle)));
    float vy = speed * static_cast<float>(sin(angle));
    Vec2f velocity(vx, vy);
    energy.m_speed = velocity;
    energy.disable();
}

void wnLucarioAuraBall::shoot(float lr, float unk, float scale) {
    soModuleAccesser* const acc = m_moduleAccesser;
    soWorkManageModule* const work = &acc->getWorkManageModule();
    releaseFromHand(acc);
    acc->getPostureModule().setLr(lr);
    acc->getStatusModule().changeStatusRequest(2, acc);
    setLaunchSpeed(acc, lr);
    work->setFloat(scale, 0x1100000A);
    setEffectScale(acc);
    updateNodeSRT();
    work->offFlag(0x12000000);
    work->setFloat(unk, 0x11000009);
}

// The share of the full charge time that has passed (not yet limited to 0..1).
static inline float getFrameRate(int frame, int maxFrame) {
    float rate = static_cast<float>(frame) / static_cast<float>(maxFrame);
    return rate;
}

float wnLucarioAuraBall::getChargeRate(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    const ftLucarioAuraBallParam* param = g_ftCommonDataAccesser.getLucarioAuraBallParam(work.getInt(0x10000009));
    int maxFrame = param->m_chargeLevelFrame * param->m_chargeLevelNum;
    float rate = getFrameRate(work.getInt(0x10000005), maxFrame);
    return clampf(rate, 0.0f, 1.0f);
}

int wnLucarioAuraBall::getChargeLevel() const {
    const ftLucarioAuraBallParam* param = getAuraBallParam(m_moduleAccesser);
    int level = m_moduleAccesser->getWorkManageModule().getInt(0x10000005) / param->m_chargeLevelFrame;
    if (level < 0) {
        level = 0;
    } else if (level >= param->m_chargeLevelNum) {
        level = param->m_chargeLevelNum;
    }
    return level;
}

int wnLucarioAuraBall::getChargeFrame() {
    return m_moduleAccesser->getWorkManageModule().getInt(0x10000005);
}

void wnLucarioAuraBall::setEffectScale(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    wnLucarioAuraBall& auraBall = dynamic_cast<wnLucarioAuraBall&>(*moduleAccesser->m_stageObject);
    const ftLucarioAuraBallParam* param = g_ftCommonDataAccesser.getLucarioAuraBallParam(work.getInt(0x10000009));
    float baseScale = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa6, 0);
    float levelScale = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa7 + auraBall.getChargeLevel(), 0);
    float mul = work.getFloat(0x1100000A);
    float charge = work.getFloat(0x11000009);
    moduleAccesser->getPostureModule().setScale(
        mul * ((1.0f / baseScale) * (param->m_scaleBase + levelScale * (charge * param->m_scaleRate))));
}

void wnLucarioAuraBall::updateNodeSRT() {
    soModuleAccesser* acc = m_moduleAccesser;
    Vec3f position(0.0f, 0.0f, 0.0f);
    switch (acc->getStatusModule().getStatusKind()) {
    case 0:
    case 1:
        holdAtHand(acc, &position);
        break;
    case 2:
        setEffectScale(acc);
        position = acc->getPostureModule().getPos();
        break;
    }
    Vec3f rotation = acc->getPostureModule().getRot(0);
    rotation.m_y += acc->getPostureModule().getRotYLr();
    float scaleValue = acc->getPostureModule().getScale();
    Vec3f scale;
    scale.m_x = scale.m_y = scale.m_z = scaleValue;
    acc->getModelModule().setNodeSRT(0, &scale, &rotation, &position);
}
