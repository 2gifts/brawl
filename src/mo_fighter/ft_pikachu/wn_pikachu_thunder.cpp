#define WN_PIKACHU_THUNDER_ARRAY_CTOR
#define WN_PIKACHU_THUNDER_UNIT
#define WN_PIKACHU_THUNDER_ACCESSER_HELPERS
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define FT_REL_LINK_EXTERN
#define WN_ARTICLE_ANIMCMD_INT
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#include <wn/wn_module_accesser_builder.h>
#include <wn/pikachu/wn_pikachu_thunder.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_value_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <ec/ec_mgr.h>
#include <ft/ft_common_data_accesser.h>
#include <math.h>

static wnPikachuThunderStatusUniqProcessRegular s_statusUniqProcessRegular;

wnPikachuThunder::wnPikachuThunder(s32 articleId, const wnArticleBuildInfo& info, const wnPikachuThunderData* data) :
    wnWeaponBuilder<wnPikachuThunderModuleAccesserBuildConfig>(
        "wnPikachuThunder", articleId, info,
        wnModuleAccesserBuildData(0xB, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1,
                                  &g_soParamAccesserNull, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param) {
    soModuleAccesser* accesser = m_moduleAccesser;
    soStatusUniqProcess* procs[1];
    procs[0] = &s_statusUniqProcessRegular;
    accesser->getStatusModule().addRangeUniqProc(procs, 1);
    accesser->getWorkManageModule().setInt(-1, 0x10000009);
    Weapon::deactivate(false);
}

// HYPOTHESIS: the model of the bolt is this long in its own units (the scale of a bolt is its length over this size).
static float s_thunderModelSize = 100.0f;

// BYTES: this unit also calls an out-of-line Vec3f assignment, but the activation copies the position as an aggregate (word
// moves); the copy goes through a plain word triple.
struct wnPikachuThunderVec3Words {
    u32 words[3];
};

void wnPikachuThunder::activate(int founderTaskId, int resourceId, int team, const Vec3f* position, wnPikachuCloud* cloud,
                                bool withHead) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    *reinterpret_cast<wnPikachuThunderVec3Words*>(&desc.pos) = *reinterpret_cast<const wnPikachuThunderVec3Words*>(position);
    desc.lr = 1.0f;
    desc.team = team;
    desc.life = 0;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.flag4DBit7 = 0;
    desc.flag4DBit6 = 0;
    desc.unk38 = 3;
    desc.unk3C = 0xFF;
    Weapon::activate(&desc);
    soModuleAccesser* accesser = m_moduleAccesser;
    soGroundModule& ground = accesser->getGroundModule();
    soWorkManageModule& workManage = accesser->getWorkManageModule();
    Vec2f safePos;
    safePos.m_x = position->m_x;
    safePos.m_y = position->m_y;
    ground.setShapeSafePos(&safePos, 0);
    setPosForce(const_cast<Vec3f*>(position));
    updateNodeSRT();
    float scale = 1.0f;
    workManage.setFloat(scale, 0x11000004);
    wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(*accesser->getKineticModule().getEnergy(0));
    Vec2f speed(0.0f, m_param->unk4 * scale);
    normal.m_speed.m_x = speed.m_x;
    normal.m_speed.m_y = speed.m_y;
    ground.modifyRhombus(3.0f * scale, 2.0f * scale, scale, 0); // HYPOTHESIS: the sizes
    workManage.setInt((int)m_param->unk8, 0x10000008);
    float lower = position->m_y - m_param->unk4 * scale;
    workManage.setFloat(position->m_y, 0x11000001);
    workManage.setFloat(lower, 0x11000002);
    rec_cloud(cloud);
    rec_next_thunder(0);
    workManage.offFlag(0x12000003);
    workManage.onFlag(0x12000000);
    accesser->getStatusModule().changeStatusForce(0, accesser);
    ref_log_group(false);
    soEffectModule& effect = accesser->getEffectModule();
    int head = 0;
    if (withHead) {
        head = effect.reqFollow(static_cast<EfID>(0x80008), 0, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, false, 0, 0, -1);
    }
    int body = effect.req(static_cast<EfID>(0x80009), 2, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, &Vec3f(0.0f, 0.0f, 0.0f),
                          &Vec3f(0.0f, 0.0f, 0.0f), false, 0);
    workManage.setInt(head, 0x1000000A);
    workManage.setInt(body, 0x1000000B);
}

void wnPikachuThunder::onDeactivate() {
    soModuleAccesser* accesser = m_moduleAccesser;
    unref_log_group(false);
    accesser->remove_effect(0x1000000A);
    accesser->remove_effect(0x1000000B);
    wnPikachuCloud* cloud = get_cloud();
    if (cloud) {
        cloud->change_head_thunder(get_next_thunder());
        cloud->deactive_thunder((int)this);
    }
    remove();
}

void wnPikachuThunder::ref_log_group(bool chain) {
    wnPikachuCloud* cloud = get_cloud();
    if (cloud) {
        cloud->ref_log_group(&m_moduleAccesser->getCollisionAttackModule());
    }
    if (chain) {
        wnPikachuThunder* next = get_next_thunder();
        if (next) {
            next->ref_log_group(true);
        }
    }
}

void wnPikachuThunder::unref_log_group(bool chain) {
    wnPikachuCloud* cloud = get_cloud();
    if (cloud) {
        cloud->unref_log_group(&m_moduleAccesser->getCollisionAttackModule());
    }
    if (chain) {
        wnPikachuThunder* next = get_next_thunder();
        if (next) {
            next->unref_log_group(true);
        }
    }
}

// HYPOTHESIS: a reflected bolt tells its cloud who reflected it.
void wnPikachuThunder::notifyEventCollisionAttack(float power, const soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    switch (collisionLog->m_kind) {
    case 3: {
        wnPikachuCloud* cloud = get_cloud();
        if (cloud) {
            soReflectModule& reflect = moduleAccesser->getReflectModule();
            int taskId = reflect.getTaskId();
            int team = reflect.getTeam();
            cloud->notify_reflect(team, taskId);
        }
        break;
    }
    }
}

void wnPikachuThunder::change_team(int team, int teamOwner) {
    soTeamModule& teamModule = m_moduleAccesser->getTeamModule();
    teamModule.setTeam(team, true);
    teamModule.setHitTeam(team);
    teamModule.setTeamOwnerId(teamOwner);
    wnPikachuThunder* next = get_next_thunder();
    if (next) {
        next->change_team(team, teamOwner);
    }
}

bool wnPikachuThunder::notifyEventCollisionAttackCheck(u32 flags) {
    return false;
}

void soModuleAccesser::remove_effect(u32 index) {
    soWorkManageModule& workManage = getWorkManageModule();
    int handle = workManage.getInt(index);
    if (handle != 0) {
        getEffectModule().remove(handle);
    }
    workManage.setInt(0, index);
}

// The bolt is stretched between its two ends and follows the cloud.
void wnPikachuThunder::updateNodeSRT() {
    soModuleAccesser* accesser = m_moduleAccesser;
    soWorkManageModule* workManage = &accesser->getWorkManageModule();
    float top = workManage->getFloat(0x11000001);
    float length = fabs(top - workManage->getFloat(0x11000002));
    float scaleFactor = workManage->getFloat(0x11000004);
    Vec3f position = accesser->getPostureModule().getPos();
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    Vec3f scaling(1.0f, 1.0f, 1.0f);
    scaling.m_y = scaleFactor * (length / s_thunderModelSize);
    accesser->getModelModule().setNodeSRT(0, &scaling, &rotation, &position);
    accesser->move_effect(0x1000000A, 0);
    accesser->move_effect(0x1000000B, 2);
}

#pragma push
#pragma dont_inline on
float wnPikachuThunder::getThunderModelSize() {
    return s_thunderModelSize;
}
#pragma pop

s32 wnPikachuThunder::getThunderGenerateInterval() {
    const wnPikachuThunderData* data = static_cast<const wnPikachuThunderData*>(g_ftCommonDataAccesser.getPikachuThunderData());
    float interval = (float)fabs(s_thunderModelSize / data->m_param->unk4);
    return (s32)interval;
}

void soModuleAccesser::move_effect(u32 index, int node) {
    int handle = getWorkManageModule().getInt(index);
    if (handle != 0) {
        Vec3f position;
        Vec3f nodePosition = getModelModule().getNodeGlobalPosition(node, false);
        position = nodePosition;
        g_ecMgr->setPos(handle, &position);
    }
}

void wnPikachuThunder::deactive_cloud() {
    rec_cloud(0);
    wnPikachuThunder* next = get_next_thunder();
    if (next) {
        next->deactive_cloud();
    }
}

void wnPikachuThunder::rec_cloud(wnPikachuCloud* cloud) {
    m_moduleAccesser->getWorkManageModule().setInt((int)cloud, 0x10000005);
}

wnPikachuCloud* wnPikachuThunder::get_cloud() {
    return (wnPikachuCloud*)m_moduleAccesser->getWorkManageModule().getInt(0x10000005);
}

void wnPikachuThunder::chain_setting(wnPikachuThunder* last) {
    soModuleAccesser* accesser = m_moduleAccesser;
    soWorkManageModule& workManage = accesser->getWorkManageModule();
    int handle = workManage.getInt(0x1000000B);
    accesser->getEffectModule().remove(handle);
    workManage.setInt(0, 0x1000000B);
    bool isLast = workManage.isFlag(0x12000003);
    if (last && isLast) {
        last->rec_end_pos(workManage.getFloat(0x11000003));
    }
}

void wnPikachuThunder::rec_next_thunder(wnPikachuThunder* thunder) {
    m_moduleAccesser->getWorkManageModule().setInt((int)thunder, 0x10000007);
}

wnPikachuThunder* wnPikachuThunder::get_next_thunder() {
    return (wnPikachuThunder*)m_moduleAccesser->getWorkManageModule().getInt(0x10000007);
}

void wnPikachuThunder::rec_prev_thunder(wnPikachuThunder* thunder) {
    m_moduleAccesser->getWorkManageModule().setInt((int)thunder, 0x10000006);
}

// The bolt leaves the chain: its neighbours are linked to each other.
void wnPikachuThunder::remove() {
    wnPikachuThunder* prev = get_prev_thunder();
    wnPikachuThunder* next = get_next_thunder();
    if (prev) {
        prev->rec_next_thunder(next);
    }
    if (next) {
        next->rec_prev_thunder(prev);
    }
}

wnPikachuThunder* wnPikachuThunder::get_prev_thunder() {
    return (wnPikachuThunder*)m_moduleAccesser->getWorkManageModule().getInt(0x10000006);
}

void wnPikachuThunder::chain_last(wnPikachuThunder* thunder) {
    wnPikachuThunder* next = get_next_thunder();
    if (next) {
        next->chain_last(thunder);
    } else {
        rec_next_thunder(thunder);
        chain_setting(next);
    }
}

void wnPikachuThunder::remove_chain() {
    wnPikachuThunder* next = get_next_thunder();
    deactivate(false);
    if (next) {
        next->remove_chain();
    }
}

void wnPikachuThunder::rec_end_pos(float y) {
    soWorkManageModule& workManage = m_moduleAccesser->getWorkManageModule();
    workManage.onFlag(0x12000003);
    workManage.setFloat(y, 0x11000003);
    wnPikachuThunder* next = get_next_thunder();
    if (next) {
        next->rec_end_pos(y);
    }
}

void wnPikachuThunderStatusUniqProcessRegular::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().offFlag(0x12000004);
}

// The bolt falls with the speed of its kinetic energy: its upper end follows it, its lower end follows once it is long enough.
// When it reaches the ground, the end position of the chain is recorded and the effect of the cloud is called.
// HYPOTHESIS: work flag 0x12000004 is set once the bolt left its start; the work float 0x11000005 is the fallen length.
void wnPikachuThunderStatusUniqProcessRegular::execMapCorrection(soModuleAccesser* moduleAccesser) {
    bool deactivate = false;
    wnPikachuThunder& thunder = dynamic_cast<wnPikachuThunder&>(moduleAccesser->getStageObject());
    soGroundModule& ground = moduleAccesser->getGroundModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    float direction = workManage.getFloat(0x11000004);
    float modelSize = wnPikachuThunder::getThunderModelSize();
    if (workManage.isFlag(0x12000004) && !workManage.isFlag(0x12000003)) {
        if ((direction >= 0.0f && ground.isTouch(8, 0)) || (direction < 0.0f && ground.isTouch(1, 0))) {
            Vec2f touchPos;
            if (direction >= 0.0f) {
                touchPos = ground.getTouchPos(8, 0);
            } else {
                touchPos = ground.getTouchPos(1, 0);
            }
            thunder.rec_end_pos(touchPos.m_y);
            ground.setCorrect(soGroundShapeImpl::Correct_None, 0);
            wnPikachuCloud* cloud = thunder.get_cloud();
            if (cloud) {
                float z = ground.getZ();
                float x = touchPos.m_x;
                float y = touchPos.m_y;
                Vec3f hitPos;
                hitPos.m_x = x;
                hitPos.m_y = y;
                hitPos.m_z = z;
                cloud->call_ground_hit_effect(&hitPos);
            }
        }
    }
    bool isLast = workManage.isFlag(0x12000003);
    float endPos = workManage.getFloat(0x11000003);
    float top = workManage.getFloat(0x11000001);
    float bottom = workManage.getFloat(0x11000002);
    float speedY = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(-1)).m_y;
    top += speedY;
    if (isLast) {
        if (direction >= 0.0f) {
            if (top < endPos) {
                top = endPos;
            }
        } else if (direction < 0.0f) {
            if (top > endPos) {
                top = endPos;
            }
        }
    }
    float fallen = workManage.getFloat(0x11000005) + (float)fabs(speedY);
    workManage.setFloat(fallen, 0x11000005);
    if (fallen >= modelSize) {
        bottom += speedY;
    }
    if (isLast) {
        if (direction >= 0.0f) {
            if (top < endPos) {
                top = endPos;
            }
            if (bottom < endPos) {
                bottom = endPos;
            }
            if (!(top < bottom)) {
                deactivate = true;
            }
        } else {
            if (top > endPos) {
                top = endPos;
            }
            if (bottom > endPos) {
                bottom = endPos;
            }
            if (!(top > bottom)) {
                deactivate = true;
            }
        }
    }
    workManage.setFloat(top, 0x11000001);
    workManage.setFloat(bottom, 0x11000002);
    Vec3f position = posture.getPos();
    position.m_y = top;
    posture.setPos(&position);
    if (!workManage.isFlag(0x12000004)) {
        const float* param = *(const float* const*)((const u8*)g_ftCommonDataAccesser.getData(Fighter_Pikachu) + 0x88);
        if (fallen >= (float)fabs(param[0] * param[1])) {
            workManage.onFlag(0x12000004);
            ground.setCorrect(soGroundShapeImpl::Correct_Air, 0);
        }
    }
    if (deactivate) {
        thunder.deactivate(false);
    }
}

// TODO(nonmatching): hitcheck is declared but not defined: it first calls ftPikachu::getThunderParam (ft_pikachu.h cannot be
//   included next to the weapon builders), discards the result, and then walks the chain asking the cloud (isDropGround,
//   call_pikachu_hit_effect) whether the point is inside the span of the bolt.
// TODO(nonmatching): get_cloud and everything calling it differ only in the relocation name: the symbol at 0x14750 of the
//   ft_pikachu module is labelled wnPikminPikmin::getVariation in symbols.txt, but it is wnPikachuThunder::get_cloud.
// TODO(nonmatching): activate, updateNodeSRT, chain_setting, notifyEventCollisionAttack and execMapCorrection differ only in
//   callee-saved register numbering (execMapCorrection also reloads the 0.0f constant for its second direction test).
