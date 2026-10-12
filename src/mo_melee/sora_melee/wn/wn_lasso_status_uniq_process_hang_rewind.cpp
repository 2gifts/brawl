#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <mt/mt_vector.h>
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <wn/wn_lasso_status_view.h>
#include <types.h>

class wnLassoStatusUniqProcessHangRewind : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessHangRewind() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnLassoStatusUniqProcessHangRewind g_wnLassoStatusUniqProcessHangRewind;

class wnLassoHangRewindLinkEvent : public soLinkEventArgs {
public:
    Vec3f m_pos;
    wnLassoHangRewindLinkEvent(int eventKind, const Vec3f& pos) : soLinkEventArgs(eventKind), m_pos(pos) { }
};

class wnLassoLinkEvent : public soLinkEventArgs {
public:
    wnLassoLinkEvent(int eventKind) : soLinkEventArgs(eventKind) { }
};

void wnLassoStatusUniqProcessHangRewind::initStatus(soModuleAccesser* moduleAccesser) {
    wnLassoGetAltStatus(moduleAccesser)->unkA4Int(5);
}

void wnLassoStatusUniqProcessHangRewind::execFixPos(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: zero position sent with the link event; the event kinds 0x836 and 0x83C are unnamed.
    wnLassoHangRewindLinkEvent hangEvent(0x836, Vec3f(0.0f, 0.0f, 0.0f));
    moduleAccesser->getLinkModule().sendEventParents(3, hangEvent);
    if (hangEvent._0x4) {
        wnLassoGetAltStatus(moduleAccesser)->unkCCVec(0, &hangEvent.m_pos);
    }

    int statusKind = wnLassoGetAltStatus(moduleAccesser)->unk88Int();
    if (statusKind <= 0) {
        wnLassoLinkEvent stopEvent(0x83c);
        moduleAccesser->getLinkModule().sendEventParents(1, stopEvent);
    } else {
        // HYPOTHESIS: the work value is clamped to the status kind before it is passed on.
        int work = moduleAccesser->getWorkManageModule().getInt(0x20000001);
        int value;
        if (work <= 0) {
            value = 1;
        } else {
            value = work;
            if (work > statusKind) {
                value = statusKind;
            }
        }
        wnLassoGetAltStatus(moduleAccesser)->unkB4Int(value);
    }
}
