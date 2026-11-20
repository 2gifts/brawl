#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
#define FT_MODULE_BUILDER
#define WN_ROBOT_BEAM_BUILDER
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define FT_REL_LINK_EXTERN
#include <wn/wn_module_accesser_builder.h>
#include <wn/robot/wn_robot_beam.h>
#include <wn/robot/wn_robot_beam_status_uniq_process.h>
#include <ft/robot/ft_robot_article_pools.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <ac/ac_anim_cmd_impl.h>
#include <so/so_module_accesser.h>
#include <math.h>
#include <memory.h>


inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnRobotBeam*) {
    return reinterpret_cast<const ftRobotArticleDataAccesser*>(&g_ftCommonDataAccesser)->getBeamData(static_cast<ftKind>(subKind), resourceGroup);
}

// The Robo Beam constructor takes the Robot's own construction descriptor (the same build data), so the generic
// article array builder is spelled out for it.
template <>
void wnClassInfoImpl<46, wnRobotBeam, wnRobotBeamData>::createArray(s32 count, s32 heapType, s32 articleId,
                                                                    const wnArticleBuildInfo& info,
                                                                    wnArticleList* list) const {
    bool found = false;
    const wnRobotBeamData* data = static_cast<const wnRobotBeamData*>(
        wnFetchBlasterBulletData(info.kindInfo->subKind, &found, static_cast<const wnRobotBeam*>(nullptr)));
    for (s32 i = 0; i < count; i++) {
        wnRobotBeam* article = new ((Heaps::HeapType)heapType)
            wnRobotBeam(articleId, static_cast<const ftRobotArticleConstructionInfo&>(info), const_cast<wnRobotBeamData*>(data));
        if (article == nullptr) {
            OSReport("wnClassInfoImpl::createArray allocation fail
");
        } else {
            soArticle* entry = static_cast<soArticle*>(article);
            list->insert(entry);
        }
    }
}

// Registers the Robo Beam article class (kind 46) in the class info table during static initialization.
wnClassInfoImpl<46, wnRobotBeam, wnRobotBeamData> g_wnRobotBeamClassInfo;

wnRobotBeam::wnRobotBeam(s32 articleId, const ftRobotArticleConstructionInfo& info, void* data)
    : wnWeaponBuilder<wnRobotBeamModuleAccesserBuildConfig>(
          "RobotBeam", articleId, info,
          wnModuleAccesserBuildData(0x2e, data,
              info.getHeapModule()->getInstanceHeap(),
              info.getHeapModule()->getNWModelInstanceHeap(),
              info.getHeapModule()->getNWMotionInstanceHeap(),
              static_cast<u8>(static_cast<wnRobotBeamData*>(data)->m_resGroupNo), -1, &g_soParamAccesserNull,
              0, 0, 0, 0, 0, 0, 0)) {
    m_param = static_cast<wnRobotBeamData*>(data)->m_param;
    soStatusUniqProcess* procs[1];
    procs[0] = &g_wnRobotBeamStatusUniqProcess;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(procs, 1);
    Weapon::deactivate(false);
}

wnRobotBeam::~wnRobotBeam() { }

// Angle between two vectors in radians (main dol).

// Work variables (from the base weapon): int 0x10000004 is the remaining life, flag 0x12000003 marks the strong beam.

// Starts the beam: it flies along the head tilt (angle, degrees) at the speed of the weak or the strong shot.
void wnRobotBeam::activate(float lr, float angle, s32 founderTaskId, u32 resourceId, s32 team, const Vec3f* position,
                           bool fullCharge, s32 variant) {
    s32 life = fullCharge ? m_param->strongLife : m_param->weakLife;
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = 0xFFFF;
    desc.unk8 = 0xFFFF;
    desc.unkC = 0xFFFF;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = *position;
    desc.lr = lr;
    desc.team = team;
    desc.life = life;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    desc.unk40 = variant;
    Weapon::activate(&desc);
    float speed;
    if (fullCharge) {
        speed = m_param->strongSpeed;
        m_moduleAccesser->getWorkManageModule().onFlag(0x12000003);
    } else {
        speed = m_param->weakSpeed;
    }
    float shotAngle = -angle;
    if (shotAngle < 0.0f) {
        shotAngle += 360.0f;
    }
    float radians = shotAngle * 0.017453292f;
    wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    normal.m_speed = Vec2f(lr * (speed * (float)cos(radians)), speed * (float)sin(radians));
    // Point the model along the new velocity.
    Vec3f rot = m_moduleAccesser->getPostureModule().getRot(0);
    Vec2f velocity;
    Vec2f::copy(velocity, normal.getSpeed());
    velocity.normalize();
    float forward = velocity.m_x * m_moduleAccesser->getPostureModule().getLr();
    float vertical = velocity.m_y;
    rot.m_x = -(float)atan2(vertical, forward) * 57.29578f;
    m_moduleAccesser->getPostureModule().setRot(&rot, 0);
    // Begin flight immediately, bypassing ordinary transition restrictions.
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}

// Anim cmd type 6 is the wall check: the beam bounces off a surface it meets at a shallow enough angle for as long as
// it has life left, and breaks up otherwise.
bool wnRobotBeam::notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* moduleAccesser, int index) {
    if (Weapon::notifyEventAnimCmd(cmd, moduleAccesser, index)) {
        return true;
    }
    s8 group = cmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    if (cmd->getType() > -1 && cmd->getType() < 11) {
        if (cmd->getType() == 6) {
            wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
            Vec2f velocity;
            Vec2f speed;
            Vec2f::copy(speed, normal.getSpeed());
            Vec2f::copy(velocity, speed);
            Vec2f surface;
            Vec2f::copy(surface, moduleAccesser->getGroundModule().getTouchNormal(static_cast<grCollStatus::TouchMask>(0xff), 0));
            float lr = moduleAccesser->getPostureModule().getLr();
            float incidence = velocity.angle(&surface);
            if (m_param->ricochetAngle * 0.017453292f < incidence - 1.5707964f) {
                deactivate(false);
                return true;
            }
            if (moduleAccesser->getWorkManageModule().getInt(0x10000004) - m_param->ricochetLife <= 0) {
                moduleAccesser->getStatusModule().changeStatusRequest(1, moduleAccesser);
                return true;
            }
            // Reflect the velocity about the surface normal.
            velocity = velocity - surface * 2.0f * (surface.m_x * velocity.m_x + surface.m_y * velocity.m_y);
            float newSign = velocity.m_x < 0.0f ? -1 : 1;
            float oldSign = speed.m_x < 0.0f ? -1 : 1;
            if (newSign * oldSign < 0.0f) {
                moduleAccesser->getPostureModule().setLr(lr * -1.0f);
                moduleAccesser->getPostureModule().updateRotYLr();
                moduleAccesser->getStageObject().updateNodeSRT();
            }
            // MATCH-ONLY: the native speed assignment materializes an XY pair.
            normal.m_speed = Vec2f(velocity.m_x, velocity.m_y);
            return true;
        }
    }
    return false;
}

bool wnRobotBeam::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 0x13) {
        return false;
    }
    if (flags & 4) {
        if ((((u32)unkA4 >> 7) & 1) == 1) {
            return hop();
        }
        return false;
    }
    if ((flags & 8) && m_moduleAccesser->getReflectModule().isReflect() == true) {
        m_moduleAccesser->getPostureModule().reverseLr();
        m_moduleAccesser->getPostureModule().updateRotYLr();
        int team = m_moduleAccesser->getReflectModule().getTeam();
        m_moduleAccesser->getTeamModule().setTeam(team, true);
        m_moduleAccesser->getTeamModule().setHitTeam(team);
        soTeamModule& teamModule = m_moduleAccesser->getTeamModule();
        teamModule.setTeamOwnerId(m_moduleAccesser->getReflectModule().getTaskId());
        return reflect();
    }
    return false;
}

// TODO: clean up, give variables real names
namespace jj_3A3BAC {
extern "C" void* fn_27_3A3BAC(void* self) {
    return reinterpret_cast<u8*>(self) + 0x7f8;
}
}

// TODO: clean up, give variables real names
namespace jj_3A574C {
extern "C" void fn_27_3A4714(void*);

extern "C" void fn_27_3A574C(void* self) {
    fn_27_3A4714(reinterpret_cast<u8*>(self) - 0x40);
}
}

// TODO: clean up, give variables real names
namespace jj_3A5754 {
extern "C" void fn_27_3A4714(void*);

extern "C" void fn_27_3A5754(void* self) {
    fn_27_3A4714(reinterpret_cast<u8*>(self) - 0x90);
}
}

// TODO: clean up, give variables real names
namespace jj_3A575C {
extern "C" void fn_27_3A4714(void*);

extern "C" void fn_27_3A575C(void* self) {
    fn_27_3A4714(reinterpret_cast<u8*>(self) - 0x88);
}
}

// TODO: clean up, give variables real names
namespace jj_3A5764 {
extern "C" void __dt__11wnRobotBeamFv(void*);

extern "C" void fn_27_3A5764(void* self) {
    __dt__11wnRobotBeamFv(reinterpret_cast<u8*>(self) - 0x40);
}
}

// TODO: clean up, give variables real names
namespace jj_3A576C {
extern "C" void notifyEventAnimCmd__11wnRobotBeamFP9acAnimCmdP16soModuleAccesseri(void*);

extern "C" void fn_27_3A576C(void* self) {
    notifyEventAnimCmd__11wnRobotBeamFP9acAnimCmdP16soModuleAccesseri(reinterpret_cast<u8*>(self) - 0x48);
}
}

// TODO: clean up, give variables real names
namespace jj_3A5774 {
extern "C" void notifyEventCollisionAttackCheck__11wnRobotBeamFUl(void*);

extern "C" void fn_27_3A5774(void* self) {
    notifyEventCollisionAttackCheck__11wnRobotBeamFUl(reinterpret_cast<u8*>(self) - 0x70);
}
}

// TODO: clean up, give variables real names
namespace jj_3A577C {
extern "C" void __dt__11wnRobotBeamFv(void*);

extern "C" void fn_27_3A577C(void* self) {
    __dt__11wnRobotBeamFv(reinterpret_cast<u8*>(self) - 0x90);
}
}

// TODO: clean up, give variables real names
namespace jj_3A5784 {
extern "C" void fn_27_3A54D4(void*);

extern "C" void fn_27_3A5784(void* self) {
    fn_27_3A54D4(reinterpret_cast<u8*>(self) - 0x88);
}
}

// TODO: clean up, give variables real names
namespace jj_3A578C {
extern "C" void __dt__11wnRobotBeamFv(void*);

extern "C" void fn_27_3A578C(void* self) {
    __dt__11wnRobotBeamFv(reinterpret_cast<u8*>(self) - 0x88);
}
}

