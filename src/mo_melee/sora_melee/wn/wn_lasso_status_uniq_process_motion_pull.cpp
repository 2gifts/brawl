#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <math.h>
#include <mt/mt_vector.h>
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <wn/wn_lasso_status_view.h>
#include <types.h>

class wnLassoStatusUniqProcessMotionPull : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessMotionPull() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnLassoStatusUniqProcessMotionPull g_wnLassoStatusUniqProcessMotionPull;

class wnLassoMotionPullLinkEvent : public soLinkEventArgs {
public:
    Vec3f m_pos;
    wnLassoMotionPullLinkEvent(int eventKind, const Vec3f& pos) : soLinkEventArgs(eventKind), m_pos(pos) { }
};

class wnLassoMotionPullEndEvent : public soLinkEventArgs {
public:
    wnLassoMotionPullEndEvent(int eventKind) : soLinkEventArgs(eventKind) { }
};

void wnLassoStatusUniqProcessMotionPull::initStatus(soModuleAccesser* moduleAccesser) {
    soLinkModule& link = moduleAccesser->getLinkModule();
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    // HYPOTHESIS: the link event 0x838 carries a zero position; its kind and the constant 0x2B58 are unverified.
    wnLassoMotionPullLinkEvent startEvent(0x838, Vec3f(0.0f, 0.0f, 0.0f));
    link.sendEventParents(3, startEvent);
    if (startEvent._0x4) {
        int kind = workManage.getInt(0x20000000);
        float limit = workManage.getFloat(0x21000000);
        float speed = workManage.getFloat(0x21000001);
        float span = workManage.getFloat(0x21000002);
        Vec3f linkPos = wnLassoGetLink(moduleAccesser)->unk174VecOut();
        float distance = linkPos.m_x - startEvent.m_pos.m_x;
        float clamped = (float)fabs(distance);
        if (clamped > limit) {
            clamped = limit;
        }
        float rateScale = clamped / span;
        float frame = speed * ((limit - clamped) / limit);
        soMotionChangeParam motionParam;
        motionParam.m_kind = kind;
        motionParam.m_frame = frame;
        motionParam.m_rate = (speed - frame) / rateScale;
        motionParam._12 = 0;
        motionParam._13 = 0;
        motionParam._14 = 0;
        motionParam._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&motionParam);
    }
}

void wnLassoStatusUniqProcessMotionPull::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getMotionModule().isEnd()) {
        wnLassoMotionPullEndEvent endEvent(0x839);
        moduleAccesser->getLinkModule().sendEventParents(1, endEvent);
    }
}
