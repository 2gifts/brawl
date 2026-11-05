#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Local view of soGenerateArticleManageModule with the two-argument removal at vtable slot 0x48 (see the Mario
// special-S unit for the same layout).
class ftNessArticleManageView {
public:
    virtual void slot00();
    virtual void slot01();
    virtual void slot02();
    virtual void slot03();
    virtual void slot04();
    virtual void slot05();
    virtual void slot06();
    virtual void slot07();
    virtual void slot08();
    virtual void slot09();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void unk48(int, int);   // name unknown: two-argument call through slot 0x48
};

class ftNessStatusUniqProcessSpecialHi : public soStatusUniqProcess {
public:
    virtual ~ftNessStatusUniqProcessSpecialHi() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftNessStatusUniqProcessSpecialHi::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    // The common data lookup result is not used by this status.
    g_ftCommonDataAccesser.getData(Fighter_Ness);
    // HYPOTHESIS: work id 0x20000003 is a one-shot int set on entry.
    work.setInt(1, 0x20000003);
}

// ABS<f> (the map name); src/mo_fighter/ft_ness/ft_ness.cpp calls it by this stand-in name.
extern "C" float fn_101_CA34(float x);
void ftNessStatusUniqProcessSpecialHi::execFixPosCounter(soModuleAccesser* a) {
    soWorkManageModule& work = a->getWorkManageModule();
    soSituationModule& situation = a->getSituationModule();
    soPostureModule& posture = a->getPostureModule();
    soStatusModule& status = a->getStatusModule();
    soGroundModule& ground = a->getGroundModule();
    soLinkModule& link = a->getLinkModule();
    if (!link.isLink(6)) return;

    ftData* nessData = g_ftCommonDataAccesser.getData(Fighter_Ness);
    const float* nessParamGroup3 = static_cast<const float*>(nessData->extendParam[2]);
    bool inAimWindow = false;
    int phase = work.getInt(0x20000003);
    work.getInt(0x20000004);
    float scale = posture.getScale();
    Vec3f fighterPos = posture.getPos();
    u32 parentNode = 0;
    Vec3f targetPos = link.getParentModelNodeGlobalPosition(6, parentNode, false);
    link.getParentModelNodeGlobalPosition(6, parentNode, true);
    fighterPos.m_y += 5.0f * scale;
    if (fn_101_CA34(fighterPos.m_x - targetPos.m_x) < 8.333333f * scale &&
        fn_101_CA34(fighterPos.m_y - targetPos.m_y) < 12.333333f * scale) {
        inAimWindow = true;
    }

    switch (phase) {
    case 0:
        if (inAimWindow) {
            phase = 2;
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
                float groundAngle = groundNormal.angle(&aim);
                if (groundAngle < 1.5707964f) work.onFlag(0x22000011);
                // HYPOTHESIS: Ness extend-param group 3 +0x24 is the per-move angular allowance.
                if (groundAngle > 0.017453292f * (90.0f + nessParamGroup3[9])) {
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
void ftNessStatusUniqProcessSpecialHi::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    soLinkModule& link = moduleAccesser->getLinkModule();
    if (link.isLink(6)) {
        link.unlink(6);
    }
    ftNessArticleManageView* articles = reinterpret_cast<ftNessArticleManageView*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule);
    articles->unk48(2, 0);
    if (nextStatus == 0x10) {
        g_ftCommonDataAccesser.getData(Fighter_Ness);
    }
}

ftNessStatusUniqProcessSpecialHi g_ftNessStatusUniqProcessSpecialHi;
