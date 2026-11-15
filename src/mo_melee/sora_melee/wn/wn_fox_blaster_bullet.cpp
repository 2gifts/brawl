#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
#define FT_MODULE_BUILDER
#define WN_MARIO_FIREBALL_UNIT // the sora_melee article unit mode shared with the Mario fireball (builders out of line)
#define WN_ZELDA_LIGHTING_BOW_UNIT // the member layout and constructor of the module builder of this unit mode (wn/zelda/wn_zelda_lighting_bow.h)
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_STATUS_NO_TAIL
#define WN_ARTICLE_ANIMCMD_INT // BYTES: the builder vtable takes Weapon's int-index notifyEventAnimCmd thunk (@72@), as in the original
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#include <wn/wn_module_accesser_builder.h>
#include <wn/zelda/wn_zelda_lighting_bow.h>
#include <wn/fox/wn_fox_blaster_bullet.h>
#include <so/model/so_model_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <gf/gf_task_scheduler.h>
#include <types.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <wn/wn_activate_desc.h>
#include <so/motion/so_motion_change_param.h>


#include <so/so_slow.h>
#include <so/so_value_accesser.h>
#include <math.h>
#include <wn/wn_class_info_impl.h>

// The unit's read-only data opens with these two constants (as do the Falco and Wolf bullet units, and nothing else in the
// game). MWCC keeps external-linkage constants at the start of .rodata and folds their values into the literal pool of the
// code that reads them, which is the native layout: activate has its own 8.0 and -8.0.
// HYPOTHESIS: the names; no code reads the first one.
extern const float g_wnFoxBlasterBulletGroundOffsetUnread = 9.25f;
extern const float g_wnFoxBlasterBulletGroundOffsetX = 8.0f;

// Registers the Fox Blaster article class (kind 31) in the class info table during static initialization.
wnClassInfoImpl<31, wnFoxBlasterBullet, wnFoxBlasterBulletData> g_wnFoxBlasterBulletClassInfo;

// BYTES: the motion change parameter constructor is expanded in this unit (the weak inline copy of the original).
inline soMotionChangeParam::soMotionChangeParam(int kind, float frame, float rate, u8 a, u8 b, u8 c, u8 d) :
    m_kind(kind), m_frame(frame), m_rate(rate), _12(a), _13(b), _14(c), _15(d) { }

// BYTES: the empty base destructors that sora_melee always expands (they have no out-of-line copy there).
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }

wnFoxBlasterBullet::wnFoxBlasterBullet(s32 articleId, const wnArticleBuildInfo& info, const wnFoxBlasterBulletData* data) :
    wnWeaponBuilder<wnFoxBlasterBulletModuleAccesserBuildConfig>(
        "FoxBlasterBullet", articleId, info,
        wnModuleAccesserBuildData(31, data, info.getHeapModule()->getInstanceHeap(),
                                  info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1,
                                  &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param), unk2174(info.kindInfo->subKind) {
    Weapon::deactivate(false);
}

// BYTES: the .text order of the original unit is the article constructor (with its weak copies, the weapon builder
// destructor first), the parameter accesser destructor, getModuleAccesser, the module accesser builder constructor,
// then the article members; MWCC emits weak copies in definition order, so these are defined here.
inline wnFoxBlasterBulletParamAccesser::~wnFoxBlasterBulletParamAccesser() { }

#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnFoxBlasterBulletModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>

// The bolt's length scale (work float 0x11000001, applied to the model's Z scale in updateNodeSRT) grows with the distance
// flown each frame, up to the article's constant 0xFA0.
inline void wnFoxBlasterBullet::updateStretch() {
    float speedMagnitude =
        Vec2f(m_moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1))).length();
    float maximum = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA0, 0);
    float current = m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    float next = current + speedMagnitude / soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA1, 0);
    if (next > maximum) {
        next = maximum;
    }
    if (next < 0.00001f) {
        next = 0.001f;
    }
    m_moduleAccesser->getWorkManageModule().setFloat(next, 0x11000001);
}

void wnFoxBlasterBullet::activate(int founderTaskId, int, int team, const Vec3f* pos,
                                   int unkMode, EfID effectId, float lr,
                                   float speed, float angle) {
    wnActivateDesc desc(founderTaskId, 0xFFFF, 0xFFFF, 0xFFFF, -1, -1, 0, 0, *pos, -lr, team, m_param->unk0, 2, 0x80, 0,
                        0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);

    m_moduleAccesser->getWorkManageModule().setFloat(0.001f, 0x11000001);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);

    u32 effect = m_moduleAccesser->getEffectModule().reqFollow(effectId, 0, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f),
                                                                1.0f, false, 0, 0, -1);
    m_moduleAccesser->getWorkManageModule().setInt(effect, 0x10000005);

    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(
        *m_moduleAccesser->getKineticModule().getEnergy(0));
    Vec2f velocity(lr * (speed * (float)cos(angle)), speed * (float)sin(angle));
    energy.m_speed = velocity;
    m_moduleAccesser->getCollisionAttackModule().setSpeed(&velocity);
    m_moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x11000001);

    if (unkMode == 0) {
        if (lr == 1.0f) {
            m_moduleAccesser->getGroundModule().setOffsetX(-g_wnFoxBlasterBulletGroundOffsetX, 0);
            m_moduleAccesser->getGroundModule().setShapeFlag(static_cast<soGroundShapeImpl::ShapeFlagId>(4), true, 0);
        } else {
            m_moduleAccesser->getGroundModule().setOffsetX(g_wnFoxBlasterBulletGroundOffsetX, 0);
            m_moduleAccesser->getGroundModule().setShapeFlag(static_cast<soGroundShapeImpl::ShapeFlagId>(3), true, 0);
        }
        m_moduleAccesser->getMotionModule().changeMotionRequest(&soMotionChangeParam(0, 0.0f, 1.0f, 0, 0, 0, 0));
    } else {
        m_moduleAccesser->getGroundModule().setShapeFlag(static_cast<soGroundShapeImpl::ShapeFlagId>(4), true, 0);
        m_moduleAccesser->getGroundModule().setShapeFlag(static_cast<soGroundShapeImpl::ShapeFlagId>(3), true, 0);
        m_moduleAccesser->getMotionModule().changeMotionRequest(&soMotionChangeParam(1, 0.0f, 1.0f, 0, 0, 0, 0));
    }

    updateStretch();
    updateNodeSRT();
}

bool wnFoxBlasterBullet::reflect() {
    m_moduleAccesser->getWorkManageModule().setFloat(0.001f, 0x11000001);
    return Weapon::reflect();
}

float wnFoxBlasterBullet::getCollisionLr(soModuleAccesser* moduleAccesser) {
    return -moduleAccesser->getPostureModule().getLr();
}

void wnFoxBlasterBullet::processUpdate() {
    Weapon::processUpdate();

    soSlow* slow = soSlow::getInstance();
    if (slow->isEstimate() != 1) {
        return;
    }
    if (m_moduleAccesser->getStopModule().isStop()) {
        return;
    }

    updateStretch();
}

void wnFoxBlasterBullet::updateNodeSRT() {
    Vec2f speed = m_moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    float negativeLr = -m_moduleAccesser->getPostureModule().getLr();
    float angle = (float)atan2(speed.m_y, speed.m_x * negativeLr);
    Vec3f position = m_moduleAccesser->getPostureModule().getPos();
    Vec3f rotation = m_moduleAccesser->getPostureModule().getRot(0);
    rotation.m_x = 57.29578f * angle;
    rotation.m_y += m_moduleAccesser->getPostureModule().getRotYLr();
    float scaleValue = m_moduleAccesser->getPostureModule().getScale();
    Vec3f scale;
    scale.m_x = scale.m_y = scale.m_z = scaleValue;
    scale.m_z *= m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rotation, &position);
}

// The destructor expands the simple module builders down to their impl destructors; the big ones stay calls (the module
// accesser builder destructor in wn_zelda_lighting_bow_builder_ctor.h is compiled with inlining off).
wnFoxBlasterBullet::~wnFoxBlasterBullet() { }
