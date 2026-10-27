// The Vec3f subtraction stays a call to its weak out-of-line copy, as in the original.
#define MT_VEC3F_SUB_NOINLINE
// MATCH-ONLY: Vec2f copies are the aggregate word copies of a plain base, assignments the scalar float copies.
#define MT_VEC2F_PLAIN_BASE
#include <em/em_shot.h>
#include <em/prim/wnem_proc_prim.h>
#include <em/wnem/wnem_proc_common.h>
#include <math.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module.h>
#include <types.h>

// The procs of the Primid's boomerang. It is held (Have) until it is thrown, flies off (Fly), bouncing off the ground
// and turning back when it hits something or slows down, then homes in on the Primid that threw it (Turn), which takes
// it back when it comes close; Erase removes it. A boomerang that belongs to a team only drops to the ground.

// The Vec2f helpers the procs call are weak copies in the enemy kinetic module unit, which keeps their address names.
extern "C" {
// Vec2f::length (map name).
float fn_41_10830(const Vec2f& v);
// Vec2f operator*(const Vec2f&, float) (map name Vec2f::operator*).
Vec2f fn_41_108CC(const Vec2f& v, float c);
// atan2f(y, x), a weak copy (map name emKineticModuleImpl::atan2f).
float fn_41_15FB0(float y, float x);
}

// sora_melee: the angle between two vectors (radians).
float vec2fAngle(Vec2f*, Vec2f*);

// HYPOTHESIS: proc numbers: 2 is Fly, 3 is Turn; 0x3D is the boomerang's touch-ground / end-of-life proc (Erase).
static const s32 Proc_BoomerangFly = 2;
static const s32 Proc_BoomerangTurn = 3;
static const s32 Proc_BoomerangErase = 0x3d;
// The team number of an article without a team.
static const int TeamNo_None = 10000;
// The link index of the Primid that threw the boomerang.
static const int LinkNo_Owner = 3;

// HYPOTHESIS: the link event the boomerang sends its owner when it comes back; the Primid answers it in the event
// (emPrim::notifyEventLink), which makes the boomerang disappear.
struct wnemPrimBoomerangReturnEventArgs : public soLinkEventArgs {
    int m_linkNo;
    wnemPrimBoomerangReturnEventArgs() : soLinkEventArgs(0x13fe), m_linkNo(LinkNo_Owner) { }
};

// The boomerang's parameters (wnemSimple::unk2234). HYPOTHESIS: roles from the reads below.
struct wnemPrimBoomerangParam {
    u32 m_ownerNodeId;     // 0x0, the model node of the owner the boomerang returns to
    u32 m_catchFrame;      // 0x4, the frames of Turn before the owner can take it back
    float m_turnSpeed;     // 0x8, the largest turn per frame on the way back (degrees)
    u32 m_homingFrame;     // 0xC, the frames of Turn during which it homes in on its owner
    float m_catchDistance; // 0x10, the distance at which the owner takes it back
    float m_minFlySpeed;   // 0x14, Fly turns back below this speed
    float m_maxTurnSpeed;  // 0x18, the largest speed on the way back
    u8 unk1C[0x4];
    float m_bounceAngle;   // 0x20, Fly turns back when it hits a wall at more than 90 degrees plus this (degrees)
};

// HYPOTHESIS: 0x21F8/0x21FC are the boomerang's speed, 0x2200/0x2204 its acceleration.
static inline Vec2f* getSpeed(wnemSimple* wn) {
    return reinterpret_cast<Vec2f*>(&wn->unk21F8);
}

void wnemPrimProc::BoomerangHave(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemPrimProc::BoomerangHave  \n");
        wn->changeProcFnc(Proc_BoomerangFly, 1);
        wn->changeMotion(0, true);
    }
}

// TODO(nonmatching): 97.2%, (a)/(b) not yet classified: the saved-register assignment differs (wn, the string base,
// the literal base and the parameters), and Vec2f::operator= is called out of line where the original inlines it.
void wnemPrimProc::BoomerangFly(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemPrimProc::BoomerangFly  \n");
        wn->setProcFnc_LifeZero(Proc_BoomerangErase);
        wn->executeAnimCmd(0u);
        acc->getSituationModule().setKind(Situation_Air, true);
    }
    wnemPrimBoomerangParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemPrimBoomerangParam*>(wn->unk2234);
    }
    if (acc->getTeamModule().getTeam()->getNo() != TeamNo_None) {
        wn->setProcFnc_TouchGround(Proc_BoomerangErase);
        wn->unk2200 = wn->unk2204 = 0.0f;
        return;
    }
    float bounceAngle = 0.017453292f * param->m_bounceAngle;
    Vec2f normal;
    if (acc->getGroundModule().isTouch(grCollStatus::TouchMask(0xff), 0)) {
        // A wall it flies into nearly head-on sends it back.
        if (acc->getPostureModule().getLr() == 1.0f) {
            if (acc->getGroundModule().isTouch(grCollStatus::TouchMask(4), 0) == true) {
                normal = acc->getGroundModule().getTouchNormal(4, 0);
                if (vec2fAngle(&normal, getSpeed(wn)) >= 1.5707964f + bounceAngle) {
                    wn->changeProcFnc(Proc_BoomerangTurn, 1);
                    wn->unk21F8 = wn->unk21FC = 0.0f;
                    return;
                }
            }
        } else {
            if (acc->getGroundModule().isTouch(grCollStatus::TouchMask(2), 0) == true) {
                normal = acc->getGroundModule().getTouchNormal(2, 0);
                if (vec2fAngle(&normal, getSpeed(wn)) >= 1.5707964f + bounceAngle) {
                    wn->changeProcFnc(Proc_BoomerangTurn, 1);
                    wn->unk21F8 = wn->unk21FC = 0.0f;
                    return;
                }
            }
        }
        // Otherwise it bounces off the ground: the speed is reflected about the touch normal.
        normal = acc->getGroundModule().getTouchNormal(0xff, 0);
        float dot = normal.m_x * wn->unk21F8 + normal.m_y * wn->unk21FC;
        *getSpeed(wn) = *getSpeed(wn) - fn_41_108CC(fn_41_108CC(normal, 2.0f), dot);
        emShotParam* shotParam = wn->m_shotWork.m_param;
        float speed = fn_41_10830(*getSpeed(wn));
        if (speed > 0.00001f) {
            Vec2f accel = fn_41_108CC(*getSpeed(wn), shotParam->m_accel / speed);
            wn->unk2200 = accel.m_x;
            wn->unk2204 = accel.m_y;
        }
    }
    // Hitting something sends it back.
    if (wn->unk2215) {
        wn->unk21F8 = wn->unk21FC = 0.0f;
        wn->unk2215 = false;
        wnemCommonProc::Report("wnemPrimProc::BoomerangFly : isInfliction true \n");
        wn->changeProcFnc(Proc_BoomerangTurn, 1);
    }
    // So does slowing down.
    if (fn_41_10830(*getSpeed(wn)) <= param->m_minFlySpeed) {
        wn->changeProcFnc(Proc_BoomerangTurn, 1);
        wnemCommonProc::Report("wnemPrimProc::BoomerangFly : FlySpeedMin true \n");
    }
}

// TODO(nonmatching): 98.7%, (a)/(b) not yet classified: Vec2f::operator= is called out of line (the original stores both
// scaled components straight from registers), the stack frame is 0x8 larger, and .rodata has 0.01 before 1.17549435e-38
// (the original pools the weak Vec2f::length copy's literal first).
void wnemPrimProc::BoomerangTurn(wnemSimple* wn, soModuleAccesser* acc) {
    emShotParam* shotParam = wn->m_shotWork.m_param;
    wnemPrimBoomerangParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemPrimBoomerangParam*>(wn->unk2234);
    }
    if (acc->getTeamModule().getTeam()->getNo() != TeamNo_None) {
        wn->setProcFnc_TouchGround(Proc_BoomerangErase);
        wn->unk2200 = wn->unk2204 = 0.0f;
        return;
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("wnemPrimProc::BoomerangTurn : ---- OnEnter --------- \n");
        wn->setProcFnc_LifeZero(Proc_BoomerangErase);
        wn->executeAnimCmd(1u);
        wn->unk2200 = 0.0f;
        wn->unk2204 = 0.0f;
        // It starts back toward its owner.
        if (acc->getLinkModule().isLink(LinkNo_Owner) == true) {
            Vec3f ownerPos = acc->getLinkModule().getParentModelNodeGlobalPosition(LinkNo_Owner, param->m_ownerNodeId, false);
            Vec3f toOwner = ownerPos - acc->getPostureModule().getPos();
            toOwner.m_x *= 0.01f;
            toOwner.m_y *= 0.01f;
            toOwner.m_z *= 0.01f;
            *getSpeed(wn) = Vec2f(toOwner.m_x, toOwner.m_y);
        }
    }
    float angle = fn_41_15FB0(wn->unk21FC, wn->unk21F8);
    float catchDistance = param->m_catchDistance;
    float turnSpeed = 0.017453292f * param->m_turnSpeed;
    float homingFrame = param->m_homingFrame;
    u32 ownerNodeId = param->m_ownerNodeId;
    if (acc->getLinkModule().isLink(LinkNo_Owner) == true && wn->getFrameCounter(1) < homingFrame) {
        Vec3f ownerPos = acc->getLinkModule().getParentModelNodeGlobalPosition(LinkNo_Owner, ownerNodeId, false);
        Vec3f toOwner = ownerPos - acc->getPostureModule().getPos();
        // Close enough, the owner takes it back.
        if (wn->getFrameCounter(1) >= param->m_catchFrame) {
            float lengthSq = toOwner.m_y * toOwner.m_y + (toOwner.m_x * toOwner.m_x + toOwner.m_z * toOwner.m_z);
            float length;
            if (static_cast<float>(__fabs(lengthSq)) <= 1.17549435e-38f) {
                length = 0.0f;
            } else {
                length = lengthSq * rsqrtf(lengthSq);
            }
            if (length < catchDistance) {
                wnemPrimBoomerangReturnEventArgs args;
                acc->getLinkModule().sendEventParents(LinkNo_Owner, args);
                if (args._0x4 == true) {
                    wn->RequestDeactive();
                }
            }
        }
        // Turn toward the owner, at most turnSpeed per frame.
        // Both angles are brought back into [-pi, pi].
        float turn = fn_41_15FB0(toOwner.m_y, toOwner.m_x);
        if (turn < -3.1415927f) {
            turn += 6.2831855f;
        } else if (turn > 3.1415927f) {
            turn -= 6.2831855f;
        }
        turn -= angle;
        if (turn < -3.1415927f) {
            turn += 6.2831855f;
        } else if (turn > 3.1415927f) {
            turn -= 6.2831855f;
        }
        if (turn > turnSpeed) {
            turn = turnSpeed;
        } else if (turn < -turnSpeed) {
            turn = -turnSpeed;
        }
        angle += turn;
    }
    float speed = -1.0f * shotParam->m_accel + fn_41_10830(*getSpeed(wn));
    if (speed > param->m_maxTurnSpeed) {
        speed = param->m_maxTurnSpeed;
    }
    wn->unk21F8 = speed * static_cast<float>(cos(angle));
    wn->unk21FC = speed * static_cast<float>(sin(angle));
}

// TODO(nonmatching): 99.9%, (a) relocations and one branch distance only (the epilogue block sits elsewhere).
void wnemPrimProc::BoomerangErase(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemPrimProc::BoomerangErase  \n");
        wn->m_shotWork.unk2C[6] = true;
        wn->unk21B8 = false;
        wn->unk21BB = false;
        wn->unk21B9 = false;
        acc->getCollisionAttackModule().clearAll();
        wn->executeAnimCmd(2u);
        wn->unk21F8 = wn->unk21FC = wn->unk2200 = wn->unk2204 = 0.0f;
        wn->setVisibilityWhole(0);
        acc->getCollisionAttackModule().clearAll();
        wn->RequestDeactive();
    }
}
