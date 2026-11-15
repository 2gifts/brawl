#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define MT_VEC2F_CTOR_OUT_OF_LINE
#define MT_VEC2F_LENGTH_SQ_OUT_OF_LINE
#define MT_VEC2F_DISTANCE_NOINLINE
#define MT_VEC3F_SUB_FRESH_REGS
#include <so/so_module_accesser.h>
#include <wn/pikmin/wn_pikmin_pikmin_status_uniq_process_follow_common.h>

// HYPOTHESIS: a by-value difference built on Vec2f::sub (the result is copied out twice).
static inline Vec2f offsetFrom(Vec2f& from, const Vec2f& to) {
    Vec2f result;
    from.sub(&to, &result);
    return result;
}

void wnPikminPikminStatusUniqProcessFollowCommon::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().setInt(60, 0x20000001);
    moduleAccesser->getWorkManageModule().setInt(60, 0x20000000);
    moduleAccesser->getWorkManageModule().setFloat(20.0f, 0x21000001);
}

void wnPikminPikminStatusUniqProcessFollowCommon::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().getInt(0x20000000) >= 0) {
        // While the counter runs, accumulate how far the Pikmin moved on its own (beyond its kinetic speed).
        moduleAccesser->getWorkManageModule().decInt(0x20000000);
        Vec3f move3 = moduleAccesser->getPostureModule().getPos() - moduleAccesser->getPostureModule().getPrevPos();
        Vec2f move(move3);
        move.sub(&moduleAccesser->getKineticModule().getEnergy(2)->getSpeed(), &move);
        float drift = move.lengthSq();
        moduleAccesser->getWorkManageModule().addFloat(drift, 0x21000000);
        return;
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000001) == 1) {
        return;
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000000) == 1 &&
        moduleAccesser->getWorkManageModule().isFlag(0x12000003) == 0) {
        return;
    }
    int interval = moduleAccesser->getWorkManageModule().getInt(0x20000001);
    moduleAccesser->getWorkManageModule().setInt(interval, 0x20000000);
    if (moduleAccesser->getWorkManageModule().getFloat(0x21000000) <= 25.0f) {
        // Barely moved during the interval: flag it once it is outside the follow radius of its target point.
        soPostureModule& posture = moduleAccesser->getPostureModule();
        Vec2f pos(posture.getPos().m_x, posture.getPos().m_y);
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        float targetY = work.getFloat(0x11000005);
        Vec2f diff = offsetFrom(pos, Vec2f(work.getFloat(0x11000004), targetY));
        float radius = moduleAccesser->getWorkManageModule().getFloat(0x21000001);
        if (diff.lengthSq() >= radius * radius) {
            moduleAccesser->getWorkManageModule().onFlag(0x22000001);
        }
    }
    moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x21000000);
}
