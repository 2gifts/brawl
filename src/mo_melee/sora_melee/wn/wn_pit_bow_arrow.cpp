#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define FT_MODULE_BUILDER
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ATTACK_ABSOLUTE_DTOR_INLINE
#define WN_KINETIC_MEDIATOR_INLINE_DTOR
#define WN_TRANSITION_CTOR_INLINE
#define WN_TRANSITION_INFO_BARE
#define SO_GENERAL_WORK_SIMPLE_CTOR_INLINE
#define FT_GENERAL_WORK_ABSTRACT_DTOR_INLINE
#define WN_SIMPLE_INLINE_TEAM
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#define WN_PIT_BOW_FULL_BUILDER
#include <wn/wn_module_accesser_builder.h>
FT_DOL_ARRAY_VECTOR(soStatusUniqProcess*, 3); // BYTES: the status process vector is built and torn down by calls
#include <wn/pit/wn_pit_bow_arrow.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <math.h>
#include <memory.h>

// BYTES: the empty base destructors that sora_melee always expands (they have no out-of-line copy there).
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }
inline soTransitionInfo::~soTransitionInfo() { }
inline soKineticEnergy::~soKineticEnergy() { }
inline soControllerModuleLinkRef::~soControllerModuleLinkRef() { }
inline soStatusModuleImpl::~soStatusModuleImpl() { }

// The status unique process of the flying arrow (wn_pit_bow_arrow_status_uniq_process_fly.cpp).
extern soStatusUniqProcess g_wnPitBowArrowStatusUniqProcessFly;

wnPitBowArrow::wnPitBowArrow(s32 articleId, const wnArticleBuildInfo& info, const wnPitBowArrowData* data)
    : wnWeaponBuilder<wnPitBowArrowModuleAccesserBuildConfig>(
          "PitBowArrow", articleId, info,
          wnModuleAccesserBuildData(91, data,
              info.getHeapModule()->getInstanceHeap(),
              info.getHeapModule()->getNWModelInstanceHeap(),
              info.getHeapModule()->getNWMotionInstanceHeap(),
              static_cast<u8>(data->m_resGroupNo), -1, &m_paramAccesser,
              0, 0, 0, 0, 0, 0, 0)),
      m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[3];
    uniqProcs[0] = 0;
    uniqProcs[2] = 0;
    uniqProcs[1] = &g_wnPitBowArrowStatusUniqProcessFly;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 3);
    Weapon::deactivate(false);
}

// Registers the Pit bow arrow article class (kind 91) in the class info table during static initialization.
wnClassInfoImpl<91, wnPitBowArrow, wnPitBowArrowData> g_wnPitBowArrowClassInfo;

void wnPitBowArrow::activate(int founderTaskId, int resourceId, int team) {
    int life = m_param->unk24;
    Vec3f pos(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0, pos, 0.0f, team, life, 2, 0x80,
                        0, 0x35F, 0);
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getGroundModule().setTestCollStop(true, 0);
    m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute(4), true);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    m_moduleAccesser->getGroundModule().setOffsetY(m_param->unk20, 0);
}

void wnPitBowArrow::toFly(bool charged, int chargeFrame, float lr) {
    Vec3f pos = m_moduleAccesser->getModelModule().getNodeGlobalPosition(getData()->m_nockNodeNo, false);
    pos.m_z = m_moduleAccesser->getGroundModule().getZ();
    m_moduleAccesser->getPostureModule().init(&pos, lr);
    removeConstraint();
    float angle;
    if (charged == true) {
        angle = m_param->unk0;
    } else {
        angle = m_param->unk8;
    }
    if (lr == -1.0f) {
        angle = 180.0f - angle;
    }
    m_moduleAccesser->getPostureModule().setRot(&Vec3f(-angle, 90.0f, 0.0f), 0);
    updateNodeSRT();
    pos = m_moduleAccesser->getModelModule().getTopNodeGlobalPosFromNode(getData()->m_nockNodeNo, &pos);
    pos.m_z = m_moduleAccesser->getGroundModule().getZ();
    m_moduleAccesser->getPostureModule().setPos(&pos);
    m_moduleAccesser->getPostureModule().setLr(lr);
    m_moduleAccesser->getPostureModule().setRot(&Vec3f(-angle, 90.0f, 0.0f), 0);
    m_moduleAccesser->getWorkManageModule().setFloat(static_cast<float>(chargeFrame), 0x21000000);
    m_moduleAccesser->getWorkManageModule().setFlag(charged, 0x12000003);
    m_moduleAccesser->getWorkManageModule().setFloat(angle, 0x11000002);
    m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
}

void wnPitBowArrow::updateNodeSRT() {
    Vec3f scale;
    Vec3f rot;
    Vec3f pos;
    scale.m_x = scale.m_y = scale.m_z = m_moduleAccesser->getPostureModule().getScale();
    pos = m_moduleAccesser->getPostureModule().getPos();
    rot = m_moduleAccesser->getPostureModule().getRot(0);
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rot, &pos);
}

bool wnPitBowArrow::reflect() {
    float angle = m_moduleAccesser->getWorkManageModule().getFloat(0x11000002);
    angle += 180.0f;
    m_moduleAccesser->getWorkManageModule().setFloat(angle, 0x11000002);
    m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute(4), false);
    return Weapon::reflect();
}

bool wnPitBowArrow::hop() {
    Weapon::hop();
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    Vec2f speed = energy.getSpeed();
    if (speed.length() > 1e-05f) {
        float x = speed.m_x;
        float y = speed.m_y;
        float angle = 57.29578f * static_cast<float>(atan2(y, x));
        m_moduleAccesser->getWorkManageModule().setFloat(angle, 0x11000002);
    }
    return false;
}

template class wnExtendParamAccesser<3999, 8, 23999, 2, 0, 0>;
