#include <wn/weapon.h>
#include <gf/gf_task.h>
#include <gm/gm_global.h>
#include <so/ground/so_ground_util.h>
#include <so/motion/so_motion_change_param.h>
#include <so/so_common_data_accesser.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_slow.h>
#include <types.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_info.h>
#include <wn/wn_module_slots.h>

// The base class of every article: Mario's fireball, Fox's blaster bullet, a Pikmin... A Weapon is a StageObject that owns
// a module accesser built by the article's builder, registers itself as observer of the status, attack and hit events of
// its modules, and implements the generic article protocol: activation and deactivation, linking to the owner that threw
// it, the reflect and hop reactions, being inhaled (Kirby) and the removal outside of the dead area.

// HYPOTHESIS: names of the work variables; the numbers come from the accessors below.
enum {
    Weapon_Work_Int_Activate_Founder_Id = 0x10000000,
    Weapon_Work_Int_Log_Attack_Info0 = 0x10000001,
    Weapon_Work_Int_Log_Attack_Info1 = 0x10000002,
    Weapon_Work_Int_Init_Life = 0x10000003,
    Weapon_Work_Int_Life = 0x10000004,
    Weapon_Work_Float_Active_Global_Frame = 0x11000000,
    Weapon_Work_Flag_No_Dead_Area_Remove = 0x12000000,
    Weapon_Work_Flag_Keep_Model = 0x12000001,
};

// Modules of the accesser whose classes are not reconstructed yet (offsets in the module table: 0x98 target search, 0x9c
// physics, 0xa0 slope, 0xb4 abnormal, 0xc8 glow) are reached through their vtable slots; see wn/wn_module_slots.h.
#define WN_MODULE_AT(acc, offset) (*reinterpret_cast<void**>(reinterpret_cast<char*>((acc)->m_enumerationStart) + (offset)))

// Number of final-smash articles that are alive (the screen is dimmed while one exists).
static int g_wnFinalArticleCount;

Weapon::Weapon(const char* name, s32 subKind, const void* data, s32 articleId, const void* kindInfo,
               soModuleAccesser* moduleAccesser, int deadAreaBits) :
    StageObject(name, 12, 9, 5, 1, NULL),
    soStatusEventObserver(-1, -1),
    soCollisionAttackEventObserver(-1, -1),
    soCollisionHitEventObserver(-1, -1) {
    const s32* const* info = reinterpret_cast<const s32* const*>(kindInfo);
    m_reflectDirX = 0.0f;
    m_reflectDirY = 0.0f;
    m_deadAreaMask = deadAreaBits & 3;
    m_inhaleCount = 0;
    m_inhaleStartPos.m_x = 0.0f;
    m_inhaleStartPos.m_y = 0.0f;
    m_inhaleStartPos.m_z = 0.0f;
    m_subKind = subKind;
    m_data = data;
    m_articleId = articleId;
    unkC4 = (*info)[0];
    unkC8 = (*info)[1];
    m_moduleAccesser = moduleAccesser;
}

Weapon::~Weapon() {
}

void Weapon::activate(wnActivateDesc* desc) {
    unk30 = 9;
    StageObject::activate(&desc->pos, desc->lr, 0.0f, true);
    bool hasOwner = false;
    if (m_moduleAccesser->getLinkModule().isNull() == false && desc->founderTaskId != -1 && desc->flagBit7 != 0) {
        hasOwner = m_moduleAccesser->getLinkModule().link(3, desc->founderTaskId);
    }
    int attachKind = 2;
    if (desc->resourceId != 0xFFFF) {
        attachKind = 1;
    }
    attachInstance(desc->resourceId, desc->unk8, desc->unkC, desc->unk10, desc->unk14, desc->unk18, desc->unk1C, attachKind,
                   desc->unk38, desc->unk44, desc->flagBit5, desc->flagBit4, desc->flagBit2, desc->flagBit1, desc->unk48,
                   desc->flagBit0, desc->unk3C);
    m_moduleAccesser->getWorkManageModule().setInt(desc->founderTaskId, Weapon_Work_Int_Activate_Founder_Id);
    m_moduleAccesser->getWorkManageModule().offFlag(Weapon_Work_Flag_No_Dead_Area_Remove);
    int teamOwner = desc->founderTaskId;
    if (desc->team != -1) {
        m_moduleAccesser->getTeamModule().setTeam(desc->team, true);
        m_moduleAccesser->getTeamModule().setHitTeam(desc->team);
        m_moduleAccesser->getTeamModule().setTeamOwnerId(teamOwner);
        teamOwner = m_moduleAccesser->getTeamModule().getTeamOwnerId();
    }
    if (hasOwner == true) {
        if (desc->flagBit6 == 1) {
            constraint(-1, -1, -1);
        } else {
            if (desc->flagBit3 == 1) {
                m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute_Scale, true);
                m_moduleAccesser->getPostureModule().setOwnerScale(m_moduleAccesser->getLinkModule().getLinkScale());
                m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute_Scale, false);
            }
            if (m_moduleAccesser->getGroundModule().isNull() != true) {
                if (m_moduleAccesser->getLinkModule().isValidParentShape(3) == true) {
                    Vec2f center = m_moduleAccesser->getLinkModule().getParentShapeCenterPos(3);
                    m_moduleAccesser->getGroundModule().setShapeSafePos(&center, 0);
                } else {
                    int ownerTask = m_moduleAccesser->getTeamModule().getTeamOwnerId();
                    Vec2f center;
                    if (ownerTask != -1 && m_moduleAccesser->getLinkModule().getParentShapeCenterPosTaskId(ownerTask, &center) == true) {
                        m_moduleAccesser->getGroundModule().setShapeSafePos(&center, 0);
                    }
                }
            }
        }
        m_moduleAccesser->getCollisionAttackModule().setPowerMul2nd(m_moduleAccesser->getLinkModule().getOwnerAttackPowerMulArticle(3));
        if (m_moduleAccesser->getLinkModule().isParentGlow(3) == true) {
            int linkIndex = 3;
            if (m_moduleAccesser->getLinkModule().link(4, teamOwner) == true) {
                linkIndex = 4;
            }
            int glow = m_moduleAccesser->getLinkModule().getParentWeaponGlowAttack(linkIndex);
            WN_MODULE_SLOT(void, WN_MODULE_AT(m_moduleAccesser, 0xC8), 0, 0x1C, int)(WN_MODULE_AT(m_moduleAccesser, 0xC8), desc->unk40 + glow);
            m_moduleAccesser->getLinkModule().setAttribute(linkIndex, soLinkConnection::Attribute_Glow, true);
            if (desc->flag4DBit7 == 1) {
                WN_MODULE_SLOT(void, WN_MODULE_AT(m_moduleAccesser, 0xC8), 0, 0x78, int)(WN_MODULE_AT(m_moduleAccesser, 0xC8), linkIndex);
            }
        }
        if (m_moduleAccesser->getModelModule().isEnvMap() == true) {
            m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute_EnvMap, true);
            if (m_moduleAccesser->getLinkModule().isParentEnableEnvMap(3) == true) {
                m_moduleAccesser->getModelModule().switchEnvMap(true);
            }
        }
        if (m_moduleAccesser->getLinkModule().isParentEffectSyncScale(3) == true) {
            m_moduleAccesser->getEffectModule().setSyncScale(true);
        }
        if (desc->flag4DBit6 == 1 && m_moduleAccesser->getLinkModule().isParentEnableSubColor(3) == true) {
            GXColor color = m_moduleAccesser->getLinkModule().getParentSubColor(3);
            m_moduleAccesser->getColorBlendModule().setSubColor(color, 1);
        }
    }
    updateNodeSRT();
    m_inhaleCount = 0;
    unkA4 &= 0x4F;
    initialize();
    if (wnInfo::isFinal(m_subKind) == true) {
        g_wnFinalArticleCount++;
    }
    m_moduleAccesser->getWorkManageModule().setInt(desc->life, Weapon_Work_Int_Life);
    m_moduleAccesser->getWorkManageModule().setInt(desc->life, Weapon_Work_Int_Init_Life);
    m_moduleAccesser->getWorkManageModule().setFloat(g_GameGlobal->getGameFrame(), Weapon_Work_Float_Active_Global_Frame);
    m_moduleAccesser->getShadowModule().setIgnoreMode(1);
    m_moduleAccesser->getGroundModule().setEventToGround(false, 0);
    m_moduleAccesser->getCollisionHitModule().setCheckCatch(false, 0);
}

void Weapon::deactivate(bool treatAsRemoved) {
    setNotifyArticleEventEject(-1, m_moduleAccesser);
    onDeactivate();
    m_moduleAccesser->getStatusModule().changeStatusForce(-1, m_moduleAccesser);
    if (m_moduleAccesser->getWorkManageModule().isFlag(Weapon_Work_Flag_Keep_Model) == false) {
        m_moduleAccesser->getModelModule().deactivate();
    }
    if (isActive() == true && wnInfo::isFinal(m_subKind) == true) {
        g_wnFinalArticleCount--;
    }
    StageObject::deactivate(treatAsRemoved);
}

void Weapon::remove() {
    deactivate(false);
}

void Weapon::deactivateArticle() {
    deactivate(true);
}

void Weapon::initialize() {
    soModuleAccesser* acc = m_moduleAccesser;
    if (acc->getStatusModule().isNull() == false) {
        soStatusEventObserver::setupObserver(acc->getEventManageModule().getManageId());
    }
    if (acc->getAnimCmdModule().getInterpreterNum() > 0) {
        soAnimCmdEventObserver::setupObserver(acc->getEventManageModule().getManageId());
    }
    soLinkEventObserver::setupObserver(acc->getEventManageModule().getManageId());
    if (dynamic_cast<soCollisionAttackModuleNull*>(&acc->getCollisionAttackModule()) == NULL) {
        soCollisionAttackEventObserver::setupObserver(acc->getEventManageModule().getManageId());
    }
    if (dynamic_cast<soCollisionHitModuleNull*>(&acc->getCollisionHitModule()) == NULL) {
        soCollisionHitEventObserver::setupObserver(acc->getEventManageModule().getManageId());
    }
    soEventPresenter<soArticleEventObserver>::setManageId(acc->getEventManageModule().getManageId());
    // HYPOTHESIS: debug output when the build data names an article animation but the ground module is a null object.
    if (m_data == NULL) {
    }
    acc->getEffectModule().setSyncVisibility(true);
}

void Weapon::updatePosture(bool applyKinetic) {
    soModuleAccesser* acc = m_moduleAccesser;
    Vec3f pos = acc->getPostureModule().getPos();
    if (applyKinetic == true) {
        Vec3f speed = acc->getKineticModule().getSumSpeed3f(soKineticEnergy::AttributeFlag(0xFFF7));
        float scale = acc->getPostureModule().getOwnerScale();
        Vec3f scaled(speed.m_x * scale, speed.m_y * scale, speed.m_z * scale);
        pos.m_x += scaled.m_x;
        pos.m_y += scaled.m_y;
        pos.m_z += scaled.m_z;
        Vec3f rotation = acc->getKineticModule().getSumRotation(soKineticEnergy::AttributeFlag(0xFFFF));
        float rotScale = acc->getSlowModule().getRate();
        rotation.m_x *= rotScale;
        rotation.m_y *= rotScale;
        rotation.m_z *= rotScale;
        Vec3f current = acc->getPostureModule().getRot(0);
        Vec3f next(current.m_x + rotation.m_x, current.m_y + rotation.m_y, current.m_z + rotation.m_z);
        acc->getPostureModule().setRot(&next, 0);
    }
    Vec3f offset = acc->getKineticModule().getSumSpeed3f(soKineticEnergy::AttributeFlag(8));
    pos.m_x += offset.m_x;
    pos.m_y += offset.m_y;
    pos.m_z += offset.m_z;
    acc->getPostureModule().setPos(&pos);
}

void Weapon::processUpdate() {
    StageObject::processUpdate();
    unkA4Bit7 = 0;
}

void Weapon::processFixPosition() {
    soModuleAccesser* acc = m_moduleAccesser;
    bool adjust = soSlow::getInstance()->isAdjust();
    if (adjust == true) {
        bool stopped = acc->getStopModule().isStop();
        if (stopped == false) {
            acc->getAreaModule().check();
            acc->getItemManageModule().check();
            WN_MODULE_SLOT(void, WN_MODULE_AT(acc, 0x98), 8, 0x28)(WN_MODULE_AT(acc, 0x98));
        }
        acc->getGroundModule().processFixPosition();
        updatePosture(true);
        bool hitStop = acc->getSlowModule().isSlow();
        if (stopped == false) {
            acc->getMotionModule().processFixPosition();
        }
        if (hitStop == false) {
            acc->getStatusModule().processFixPosition(acc, false);
        } else if (acc->getMotionModule().isLooped() == true) {
            acc->getStatusModule().processFixPosition(acc, true);
        }
        acc->getStatusModule().checkTransition(acc);
        if (stopped == false && hitStop == false) {
            acc->getStatusModule().execFixPosCounter(acc);
        }
        acc->getWorkManageModule().m_frameFlags |= 0x79FC;
        acc->getStatusModule().execFixPos(acc);
        acc->getWorkManageModule().m_frameFlags = 0;
        acc->getWorkManageModule().m_frameFlags2 = 0;
        WN_MODULE_SLOT(void, WN_MODULE_AT(acc, 0xB4), 8, 0x1C)(WN_MODULE_AT(acc, 0xB4));
    }
    WN_MODULE_SLOT(void, WN_MODULE_AT(acc, 0xA0), 0, 0x18, bool)(WN_MODULE_AT(acc, 0xA0), adjust);
    WN_MODULE_SLOT(void, WN_MODULE_AT(acc, 0x9C), 8, 0x2C)(WN_MODULE_AT(acc, 0x9C));
    WN_MODULE_SLOT(void, WN_MODULE_AT(acc, 0x9C), 8, 0x3C)(WN_MODULE_AT(acc, 0x9C));
    WN_MODULE_SLOT(void, WN_MODULE_AT(acc, 0x9C), 8, 0x30, int)(WN_MODULE_AT(acc, 0x9C), 0);
}

void Weapon::processHit() {
}

void Weapon::processFixCamera() {
    StageObject::processFixCamera();
    if (soSlow::getInstance()->isAdjust() == true) {
        Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
        if (isTreadPassive() == false && m_moduleAccesser->getWorkManageModule().isFlag(Weapon_Work_Flag_No_Dead_Area_Remove) == false &&
            m_deadAreaMask != -1) {
            int area = soGroundUtil::checkDeadArea(&pos, 0.0f, 0.0f);
            if (area != -1 && (m_deadAreaMask & (1 << area)) == 0) {
                setNotifyArticleEventRemove(m_moduleAccesser);
                if (isActive() == true) {
                    deactivate(false);
                }
            }
        }
        if (pos.m_y >= 4096.0f) {
            setNotifyArticleEventRemove(m_moduleAccesser);
            if (isActive() == true) {
                deactivate(false);
            }
        }
    }
}

void Weapon::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    WN_MODULE_SLOT(void, WN_MODULE_AT(m_moduleAccesser, 0x9C), 8, 0x50)(WN_MODULE_AT(m_moduleAccesser, 0x9C));
    updateNodeSRT();
}

bool Weapon::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3) {
    if (isObserv(acmd->getKind()) == false) {
        return false;
    }
    char kind = acmd->getSubKind();
    if (kind < 5 || kind > 10) {
        return StageObject::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
    }
    switch (acmd->getSubKind()) {
    case 8:
        setNotifyArticleEventRemove(m_moduleAccesser);
        if (isActive() == true) {
            deactivate(false);
        }
        return true;
    case 6:
        constraint(-1, -1, -1);
        return true;
    case 5:
        deactivate(false);
        return true;
    case 10:
        unkA4 |= 0x10;
        return true;
    case 9:
        setNotifyArticleEventEject(-1, m_moduleAccesser);
        return true;
    }
    return false;
}

void Weapon::changeMotion(int motionKind, bool restart) {
    float frame = 0.0f;
    if (restart == true) {
        frame = m_moduleAccesser->getMotionModule().getFrame();
    }
    soMotionChangeParam param(motionKind, frame, 1.0f, 0, 0, restart, 0);
    m_moduleAccesser->getMotionModule().changeMotionRequest(&param);
}

void Weapon::setFrame(float frame) {
    m_moduleAccesser->getMotionModule().setFrame(frame);
}

void Weapon::setRate(float rate) {
    m_moduleAccesser->getMotionModule().setRate(rate);
}

void Weapon::changeStatus(int statusKind) {
    m_moduleAccesser->getStatusModule().changeStatusRequest(statusKind, m_moduleAccesser);
}

void Weapon::setVisibilityWhole(u8 visibility) {
    m_moduleAccesser->getVisibilityModule().setWhole(visibility);
}

void Weapon::setVisibility(int index, u8 visibility) {
    m_moduleAccesser->getVisibilityModule().set(index, visibility);
}

void Weapon::set2nd(u8 enable) {
    WN_MODULE_SLOT(void, WN_MODULE_AT(m_moduleAccesser, 0x9C), 8, 0x4C, u8)(WN_MODULE_AT(m_moduleAccesser, 0x9C), enable);
}

void Weapon::setSituationKind(SituationKind kind) {
    m_moduleAccesser->getSituationModule().setKind(kind, false);
}

bool Weapon::isConstraint() {
    return m_moduleAccesser->getLinkModule().isModelConstraint();
}

int Weapon::getArticleEventManageId() {
    return m_moduleAccesser->getEventManageModule().getManageId();
}

void Weapon::linkOwner(int index, int taskId) {
    m_moduleAccesser->getLinkModule().link(index, taskId);
}

void Weapon::unlinkOwner(int index) {
    if (m_moduleAccesser->getLinkModule().isLink(index) == true) {
        m_moduleAccesser->getLinkModule().unlink(index);
    }
}

void Weapon::have(int ownerTaskId, int node, int unk) {
    constraintImpl(ownerTaskId, unk, node);
}

void Weapon::constraint(int taskId, int unk1, int unk2) {
    constraintImpl(taskId, unk1, unk2);
}

// Keeps the article attached to the node of its owner that the build data names.
void Weapon::constraintImpl(int ownerTaskId, int unk1, int node) {
    if (ownerTaskId == -1) {
        ownerTaskId = m_moduleAccesser->getWorkManageModule().getInt(Weapon_Work_Int_Activate_Founder_Id);
        if (ownerTaskId == -1) {
            return;
        }
    }
    StageObject* parent = m_moduleAccesser->getLinkModule().getParent(3);
    if (parent == NULL || ownerTaskId != static_cast<int>(parent->m_taskId)) {
        return;
    }
    if (unk1 == -1) {
        unk1 = reinterpret_cast<const s32*>(m_data)[2];
        if (unk1 == -1) {
            return;
        }
    }
    if (node == -1) {
        node = reinterpret_cast<const s32*>(m_data)[1];
        if (node == -1) {
            return;
        }
    }
    m_moduleAccesser->getLinkModule().setModelConstraintPosOrt(3, node, unk1, 3, true);
    m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute_Constraint, true);
    m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute_Visible, true);
    m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute_Scale0, true);
    m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute_Team, true);
}

void Weapon::removeConstraint() {
    float scale = m_moduleAccesser->getLinkModule().getConstraintModelScale();
    float ownerScale = m_moduleAccesser->getLinkModule().getLinkScale();
    m_moduleAccesser->getLinkModule().removeModelConstraint(true);
    m_moduleAccesser->getModelModule().setScale(scale);
    m_moduleAccesser->getPostureModule().setOwnerScale(ownerScale);
    m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute_Constraint, false);
}

void Weapon::notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    gfTask* task = gfTaskScheduler::getInstance()->getTaskById(collisionLog->m_category, collisionLog->m_taskId);
    switch (collisionLog->m_kind) {
    case 0:
        if (task != NULL) {
            Vec2f hitPos(0.0f, 0.0f);
            Vec2f hitNormal(0.0f, 0.0f);
            moduleAccesser->getCollisionAttackModule().checkRebound(power, moduleAccesser, dynamic_cast<StageObject*>(task),
                                                                    collisionLog, &hitPos, &hitNormal);
        }
        break;
    case 1:
        break;
    default:
        unkA4Bit7 = 0;
        if (m_moduleAccesser->getSituationModule().getKind() == 2) {
            soCollisionAttackData* data = m_moduleAccesser->getCollisionAttackModule().getData(collisionLog->m_partIndex, false);
            if (data->m_unk3C >= 0) {
                soCollisionShieldModule& shield = soExternalValueAccesser::getCollisionShieldModule(dynamic_cast<StageObject*>(task));
                if (shield.isHop(collisionLog->m_shieldIndex) == true) {
                    float angle = shield.getHopAngle(collisionLog->m_shieldIndex) * 0.017453292f;
                    m_reflectDirX = cos(angle) * shield.getAttackMul(0);
                    m_reflectDirY = sin(angle);
                    unkA4Bit7 = 1;
                } else if (shield.isNoHop(collisionLog->m_shieldIndex) == false) {
                    Vec3f center = shield.getCenterPos(collisionLog->m_shieldIndex2, collisionLog->m_shieldIndex);
                    Vec2f dir(collisionLog->m_pos.m_x - center.m_x, collisionLog->m_pos.m_y - center.m_y);
                    float length = dir.length();
                    if (length > 0.00001f) {
                        dir.m_x *= 1.0f / length;
                        dir.m_y *= 1.0f / length;
                        Vec2f speed = m_moduleAccesser->getCollisionAttackModule().getSpeed();
                        if (angle(&dir, &speed) * 57.29578f < 135.0f) {
                            m_reflectDirX = dir.m_x;
                            m_reflectDirY = dir.m_y;
                            unkA4Bit7 = 1;
                        }
                    }
                }
            }
        }
        break;
    }
}

bool Weapon::reflect() {
    float speedMul = m_moduleAccesser->getReflectModule().getSpeedMul();
    Vec3f mul(-speedMul, 1.0f, 1.0f);
    m_moduleAccesser->getKineticModule().mulSpeed(&mul, soKineticEnergy::AttributeFlag(1));
    m_moduleAccesser->getKineticModule().mulAccel(&mul, soKineticEnergy::AttributeFlag(1));
    float lifeMul = m_moduleAccesser->getReflectModule().getLifeMul();
    int initLife = m_moduleAccesser->getWorkManageModule().getInt(Weapon_Work_Int_Init_Life);
    m_moduleAccesser->getWorkManageModule().setInt((int)((float)initLife * lifeMul), Weapon_Work_Int_Life);
    return false;
}

bool Weapon::hop() {
    Vec3f dir(m_reflectDirX, m_reflectDirY, 0.0f);
    m_moduleAccesser->getKineticModule().reflectSpeed(&dir, soKineticEnergy::AttributeFlag(1));
    return false;
}

void Weapon::reflectSettingTeam() {
    m_moduleAccesser->getPostureModule().reverseLr();
    m_moduleAccesser->getPostureModule().updateRotYLr();
    int team = m_moduleAccesser->getReflectModule().getTeam();
    m_moduleAccesser->getTeamModule().setTeam(team, true);
    m_moduleAccesser->getTeamModule().setHitTeam(team);
    m_moduleAccesser->getTeamModule().setTeamOwnerId(m_moduleAccesser->getReflectModule().getTaskId());
}

bool Weapon::notifyEventCollisionAttackCheck(u32 flags) {
    if ((flags & 0x13) != 0) {
        deactivate(false);
        return true;
    }
    if ((flags & 4) != 0) {
        if (unkA4Bit7) {
            return hop();
        }
        deactivate(false);
        return true;
    }
    if ((flags & 8) != 0 && m_moduleAccesser->getReflectModule().isReflect() == true) {
        if (m_keepOnReflectLimit == false && m_moduleAccesser->getReflectModule().isCountMax() == true) {
            setNotifyArticleEventRemove(m_moduleAccesser);
            if (isActive() == true) {
                deactivate(false);
            }
            return true;
        }
        reflectSettingTeam();
        return reflect();
    }
    return false;
}

void Weapon::notifyEventCollisionHit(float power, soCollisionAttackData* attackData, u32 index, int unk, soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog) {
}

void Weapon::notifyEventCollisionHit2nd(float posX, float collisionLr, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, bool unk) {
}

void Weapon::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4) {
    if (eventInfo->m_kind == 0x835) {
        deactivate(false);
    }
    StageObject::notifyEventLink(eventInfo, moduleAccesser, sender, unk4);
}

// Per article kind: how the article reacts when its owner is deactivated.
int Weapon::getOwnerDeactiveTreatType() {
    bool inRange;
    if (m_subKind < 0 || m_subKind > 0x99) {
        inRange = false;
    } else {
        inRange = true;
    }
    if (!inRange) {
        return 0;
    }
    return g_wnOwnerDeactiveTreatType[m_subKind];
}

void Weapon::setHitStop(float power, int allowSdi, float minFrame) {
    int frames = soStopModuleImpl::calcHitStopFrame(power, 0.3846154f, 5.0f, minFrame, 30.0f);
    m_moduleAccesser->getStopModule().setHitStopFrame(frames, allowSdi);
}

void Weapon::setLogAttackInfo(soLogAttackInfo* logAttackInfo) {
    m_moduleAccesser->getWorkManageModule().setInt(logAttackInfo->_0, Weapon_Work_Int_Log_Attack_Info0);
    m_moduleAccesser->getWorkManageModule().setInt(logAttackInfo->_4, Weapon_Work_Int_Log_Attack_Info1);
}

soLogAttackInfo Weapon::getLogAttackInfo() {
    int info1 = m_moduleAccesser->getWorkManageModule().getInt(Weapon_Work_Int_Log_Attack_Info1);
    int info0 = m_moduleAccesser->getWorkManageModule().getInt(Weapon_Work_Int_Log_Attack_Info0);
    soLogAttackInfo result;
    result._0 = info0;
    result._4 = info1;
    result._8 = 0;
    return result;
}

void Weapon::updateLogAttackInfo() {
    soArticle::updateLogAttackInfo(soExternalValueAccesser::getTeamOwnerId(this));
}

void Weapon::setPosForce(Vec3f* pos) {
    m_moduleAccesser->getPostureModule().setPos(pos);
}

bool Weapon::isCanInhaled() {
    if (m_moduleAccesser->getLinkModule().isModelConstraint() == false) {
        return wnInfo::isInhaled(m_subKind);
    }
    return false;
}

bool Weapon::isInhaled() {
    return m_isInhaled;
}

Vec3f Weapon::startInhaled() {
    m_inhaleCount = 0;
    m_isInhaled = true;
    m_inhaleStartPos = m_moduleAccesser->getPostureModule().getPos();
    soModuleAccesser* acc = m_moduleAccesser;
    acc->getStopModule().end(1);
    acc->getAreaModule().enableArea(0, false, 0);
    acc->getGroundModule().setGrCollisionMode(false);
    acc->getSituationModule().setKind(Situation_Air, false);
    acc->getKineticModule().unableEnergyAll();
    acc->getCollisionShieldModule().clearAll();
    acc->getCollisionReflectorModule().clearAll();
    acc->getWorkManageModule().setInt(Weapon_Work_Int_Init_Life, Weapon_Work_Int_Life);
    return m_inhaleStartPos;
}

void Weapon::updateInhaled() {
    m_inhaleCount++;
    m_moduleAccesser->getGroundModule().setGrCollisionMode(false);
    m_moduleAccesser->getWorkManageModule().setInt(Weapon_Work_Int_Init_Life, Weapon_Work_Int_Life);
}

void Weapon::endInhaled() {
    m_inhaleCount = 0;
    m_isInhaled = false;
    deactivate(false);
}

Vec3f* Weapon::getInhaledStartPos() {
    return &m_inhaleStartPos;
}

int Weapon::getInhaledCount() {
    return m_inhaleCount;
}

bool Weapon::isCanEat() {
    return false;
}

float Weapon::getActiveGlobalFrame() {
    if (isActiveArticle() == false) {
        return -1.0f;
    }
    return m_moduleAccesser->getWorkManageModule().getFloat(Weapon_Work_Float_Active_Global_Frame);
}

int Weapon::getFounderTaskId() {
    int founder = m_moduleAccesser->getWorkManageModule().getInt(Weapon_Work_Int_Activate_Founder_Id);
    if (founder == -1) {
        return -1;
    }
    gfTask* task = gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Weapon, founder);
    StageObject* owner = dynamic_cast<StageObject*>(task);
    if (owner != NULL) {
        return owner->getFounderTaskId();
    }
    return founder;
}

void Weapon::setAttackPowerMulPattern(float pattern) {
    m_moduleAccesser->getParamCustomizeModule().setAttackPowerMulPattern(pattern);
}

void Weapon::setGroundShapeSafePosWithAncestor() {
    if (m_moduleAccesser->getGroundModule().isNull() != true) {
        if (m_moduleAccesser->getLinkModule().isValidParentShape(3) == true) {
            Vec2f center = m_moduleAccesser->getLinkModule().getParentShapeCenterPos(3);
            m_moduleAccesser->getGroundModule().setShapeSafePos(&center, 0);
        } else {
            int ownerTask = m_moduleAccesser->getTeamModule().getTeamOwnerId();
            Vec2f center;
            if (ownerTask != -1 && m_moduleAccesser->getLinkModule().getParentShapeCenterPosTaskId(ownerTask, &center) == true) {
                m_moduleAccesser->getGroundModule().setShapeSafePos(&center, 0);
            }
        }
    }
}
