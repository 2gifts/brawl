#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/lucas/ft_lucas_status_uniq_process_special_hi.h>
#include <ft/ft_common_data_accesser.h>
#include <so/so_module_accesser.h>
#include <so/link/so_link_module_impl.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/so_external_value_accesser.h>
#include <so/ground/so_ground_module_impl.h>
#include <mt/mt_vector.h>

// Native helper: vec2fAngle__FP5Vec2fP5Vec2f at 0x8003DDD0.
float vec2fAngle(Vec2f*, Vec2f*);
void ftLucasStatusUniqProcessSpecialHi::initStatus(soModuleAccesser* a) {
    soWorkManageModule& work = a->getWorkManageModule();
    // HYPOTHESIS: This observed call may lazily initialize or cache fighter data.
    g_ftCommonDataAccesser.getData(Fighter_Lucas);
    work.setInt(1, 0x20000003);
}
extern "C" float fn_114_C4CC(float x);
void ftLucasStatusUniqProcessSpecialHi::execFixPosCounter(soModuleAccesser* a) {
    soWorkManageModule& work = a->getWorkManageModule();
    soSituationModule& situation = a->getSituationModule();
    soPostureModule& posture = a->getPostureModule();
    soStatusModule& status = a->getStatusModule();
    soGroundModule& ground = a->getGroundModule();
    soLinkModule& link = a->getLinkModule();
    if (!link.isLink(6)) return;

    ftData* lucasData = g_ftCommonDataAccesser.getData(Fighter_Lucas);
    const u8* lucasParamGroup3 = static_cast<const u8*>(lucasData->extendParam[2]);
    bool inAimWindow = false;
    int phase = work.getInt(0x20000003);
    work.getInt(0x20000004);
    float scale = posture.getScale();
    Vec3f fighterPos = posture.getPos();
    u32 parentNode = 0;
    Vec3f targetPos = link.getParentModelNodeGlobalPosition(6, parentNode, false);
    link.getParentModelNodeGlobalPosition(6, parentNode, true);
    fighterPos.m_y += 5.0f * scale;
    if (fn_114_C4CC(fighterPos.m_x - targetPos.m_x) < 8.333333f * scale &&
        fn_114_C4CC(fighterPos.m_y - targetPos.m_y) < 12.333333f * scale) {
        inAimWindow = true;
    }

    switch (phase) {
    case 0:
        if (inAimWindow) {
            phase = 2;
            // MATCH-ONLY: declaring the normal first places aim below it in the native frame.
            Vec2f groundNormal;
            Vec2f aim;
            aim.m_x = fighterPos.m_x - targetPos.m_x;
            aim.m_y = fighterPos.m_y - targetPos.m_y;
            float lr = aim.m_x >= 0.0f ? 1.0f : -1.0f;
            float vertical = aim.m_y >= 0.0f ? 1.0f : -1.0f;
            float angle = atan2f(aim.m_y, aim.m_x * lr);
            posture.setLr(lr);
            work.setFloat(vertical, 0x21000005);
            work.setFloat(angle, 0x21000004);
            work.offFlag(0x22000011);
            if (situation.getKind() == Situation_Ground) {
                groundNormal = ground.getTouchNormal(static_cast<grCollStatus::TouchMask>(8), 0);
                groundNormal.normalize();
                aim.normalize();
                float groundAngle = vec2fAngle(&groundNormal, &aim);
                if (groundAngle < 1.5707964f) work.onFlag(0x22000011);
                // HYPOTHESIS: Lucas extend-param group 3 +0x24 is the per-move angular allowance.
                if (groundAngle > 0.017453292f * (90.0f + *reinterpret_cast<const float*>(lucasParamGroup3 + 0x24))) {
                    status.changeStatusRequest(0x4a, a);
                } else {
                    status.changeStatusRequest(0x11c, a);
                }
            } else {
                work.onFlag(0x22000011);
                status.changeStatusRequest(0x11c, a);
            }
        }
        break;
    case 1:
        if (!inAimWindow) phase = 0;
        break;
    }

    work.setInt(phase, 0x20000003);
}
void ftLucasStatusUniqProcessSpecialHi::exitStatus(soModuleAccesser* a, int nextStatus) {
    soLinkModule& link = a->getLinkModule();
    soWorkManageModule& work = a->getWorkManageModule();
    if (link.isLinked(6)) link.unlink(6);
    soGenerateArticleManageModule& articleManage = *reinterpret_cast<soGenerateArticleManageModule*>(
        const_cast<soModuleEnumeration*>(a->m_enumerationStart)->m_generateArticleManageModule);
    articleManage.removeExist(2, 0);
    work.setInt(0, 0x20000003);
    if (nextStatus == 0x10) {
        // HYPOTHESIS: This observed call may lazily initialize or cache fighter data.
        g_ftCommonDataAccesser.getData(Fighter_Lucas);
    }
}
ftLucasStatusUniqProcessSpecialHi::~ftLucasStatusUniqProcessSpecialHi() {}
ftLucasStatusUniqProcessSpecialHi g_ftLucasStatusUniqProcessSpecialHi;
