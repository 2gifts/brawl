#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_MUL_INLINE
#define MT_VEC3F_ADD_NATIVE_ORDER
#include <wn/wn_module_accesser_builder.h>
#include <mt/mt_vector.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/ness/wn_ness_yoyo.h>
#include <wn/wn_lasso_status_view.h>

// The yo-yo string while the head is falling: the string follows the head, segment by segment. HYPOTHESIS: all work variable
// meanings.
extern "C" float fn_101_CA34(float x); // fabsf (weak helper of the fighter unit)

class wnNessYoyoStatusUniqProcessShotFall : public soStatusUniqProcess {
public:
    virtual ~wnNessYoyoStatusUniqProcessShotFall() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnNessYoyoStatusUniqProcessShotFall g_wnNessYoyoStatusUniqProcessShotFall;

void wnNessYoyoStatusUniqProcessShotFall::initStatus(soModuleAccesser* moduleAccesser) {
    const wnNessYoyoParam* param = dynamic_cast<wnNessYoyo&>(moduleAccesser->getStageObject()).m_param;
    const soModuleEnumeration* modules = moduleAccesser->m_enumerationStart;
    soWorkManageModule& work = *modules->m_workManageModule;
    wnLassoAltStatusView* alt = (wnLassoAltStatusView*)modules->m_physicsModule;
    int count;
    if (work.isFlag(0x22000000)) {
        count = param->unk0;
    } else {
        count = param->unk4;
    }
    float length = 0.0f;
    for (int i = 0; i < count; i++) {
        length += alt->unk74Float(i);
    }
    work.setFloat(length, 0x21000000);
    work.setInt(count, 0x20000001);
}

void wnNessYoyoStatusUniqProcessShotFall::execStatus(soModuleAccesser* moduleAccesser) { }

// The distance between two points, with the native guard against a zero length. HYPOTHESIS: an inline distance helper
// (name and owner unknown); its difference vector is allocated after every other stack temporary of execFixPos, as an inlined
// callee's local is, and the squares are summed z + (x + y) as in the Vec3f length.
static inline float wnNessYoyoDistance(Vec3f& from, const Vec3f& to) {
    Vec3f toLink;
    from.sub(&to, &toLink);
    float lengthSquared = toLink.m_z * toLink.m_z + (toLink.m_x * toLink.m_x + toLink.m_y * toLink.m_y);
    double absolute = __fabs(lengthSquared);
    float magnitude = absolute;
    if (magnitude <= 1.17549435e-38f) {
        return 0.0f;
    }
    return lengthSquared * rsqrtf(lengthSquared);
}

void wnNessYoyoStatusUniqProcessShotFall::execFixPos(soModuleAccesser* moduleAccesser) {
    dynamic_cast<wnNessYoyo&>(moduleAccesser->getStageObject());
    const soModuleEnumeration* modules = moduleAccesser->m_enumerationStart;
    soWorkManageModule& work = *modules->m_workManageModule;
    wnLassoLinkView* link = (wnLassoLinkView*)modules->m_linkModule;
    wnLassoAltStatusView* alt = (wnLassoAltStatusView*)modules->m_physicsModule;
    if (work.isFlag(0x22000001)) {
        alt->unk64Int(1);
        work.onFlag(0x22000002);
        work.offFlag(0x22000001);
    }
    float speed = work.getFloat(0x21000000);
    Vec3f linkPos = link->unk174VecOut();
    Vec3f pos(0.0f, 0.0f, 0.0f);
    if (work.isFlag(0x22000002)) {
        pos.m_x = work.getFloat(0x21000002);
        pos.m_y = work.getFloat(0x21000003);
        if (alt->unkD4Int() != 0) {
            alt->unk64Int(2);
            moduleAccesser->getStatusModule().changeStatusRequest(-1, moduleAccesser);
        } else if (link->unkA4Int(5) != 0) {
            Vec2f vel;
            Vec2f acc;
            Vec2f limit;
            vel.m_x = work.getFloat(0x21000004);
            vel.m_y = work.getFloat(0x21000005);
            acc.m_x = work.getFloat(0x21000006);
            acc.m_y = work.getFloat(0x21000007);
            limit.m_x = work.getFloat(0x21000008);
            limit.m_y = work.getFloat(0x21000009);
            // TODO: execFixPos is 99.67%. The native code reloads vel.m_y for the velocity sum instead of reusing the load
            // from the line above (and passes the new x velocity to the fabs from another register). Tried without effect:
            // vel = vel + acc, component-wise sums, pos.m_y = pos.m_y + vel.m_y, a pointer or reference to vel or pos.m_y,
            // Vec2f::set, a clamp helper, and one permuter run (6 minutes, 256 compiles, no gain).
            pos.m_y += vel.m_y;
            vel += acc;
            // The velocities snap to +- the limit when they exceed it.
            if (fn_101_CA34(vel.m_x) > limit.m_x) {
                if (vel.m_x > 0.0f) {
                    vel.m_x = limit.m_x;
                } else if (vel.m_x < 0.0f) {
                    vel.m_x = -limit.m_x;
                }
            }
            if (fn_101_CA34(vel.m_y) > limit.m_y) {
                if (vel.m_y > 0.0f) {
                    vel.m_y = limit.m_y;
                } else if (vel.m_y < 0.0f) {
                    vel.m_y = -limit.m_y;
                }
            }
            work.setFloat(pos.m_x, 0x21000002);
            work.setFloat(pos.m_y, 0x21000003);
            work.setFloat(vel.m_x, 0x21000004);
            work.setFloat(vel.m_y, 0x21000005);
        }
        Vec3f direction;
        pos.sub(&linkPos, &direction);
        direction.normalize();
        pos = direction * speed + linkPos;
        speed -= 0.001f;
        work.setFloat(speed, 0x21000000);
    } else {
        pos = link->unk98NodePos(5, work.getInt(0x20000000), false);
        if (wnNessYoyoDistance(linkPos, pos) > speed) {
            Vec3f direction;
        pos.sub(&linkPos, &direction);
            direction.normalize();
            pos = direction * speed + linkPos;
            work.onFlag(0x22000001);
            wnNessYoyoPullEvent event(work.getFloat(0x21000000));
            link->unk44SendEvent(5, event);
            work.setFloat(event.unkC.m_x, 0x21000004);
            work.setFloat(event.unkC.m_y, 0x21000005);
            work.setFloat(event.unk14.m_x, 0x21000006);
            work.setFloat(event.unk14.m_y, 0x21000007);
            work.setFloat(fn_101_CA34(event.unk1C.m_x), 0x21000008);
            work.setFloat(fn_101_CA34(event.unk1C.m_y), 0x21000009);
        }
        work.setFloat(pos.m_x, 0x21000002);
        work.setFloat(pos.m_y, 0x21000003);
    }
    alt->unk68VecPair(&linkPos, &pos);
    alt->unk8CInt(work.getInt(0x20000001));
}

// The paired-single subtraction (out = this - other), emitted in this unit after its first caller.
void Vec3f::sub(const Vec3f* other, Vec3f* out) {
    Vec3fSub(out, this, other);
}
