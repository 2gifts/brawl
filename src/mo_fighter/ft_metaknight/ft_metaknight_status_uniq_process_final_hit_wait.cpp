#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/metaknight/ft_metaknight_status_views.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftMetaknightStatusUniqProcessFinalHitWait : public soStatusUniqProcess {
public:
    virtual ~ftMetaknightStatusUniqProcessFinalHitWait() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    int getFreeAttackGalleryStatusWorkId(soModuleAccesser* moduleAccesser);
};

void ftMetaknightStatusUniqProcessFinalUtil::notifyMetaknightFinalEnd(soModuleAccesser* moduleAccesser) {
    for (int i = 6; i < 12; i++) {
        if (moduleAccesser->getLinkModule().isLink(i) == 1) {
            ftMetaknightFinalLinkEvent event(0x44e);
            moduleAccesser->getLinkModule().sendEventParents(i, event);
        }
    }
}

void ftMetaknightStatusUniqProcessFinalUtil::unLinkFinalTargetAll(soModuleAccesser* moduleAccesser) {
    for (int i = 6; i < 12; i++) {
        if (moduleAccesser->getLinkModule().isLink(i) == 1) {
            moduleAccesser->getLinkModule().unlink(i);
        }
    }
}

void ftMetaknightStatusUniqProcessFinalUtil::sumRect(ftMetaknightRect* out, const ftMetaknightRect* a, const ftMetaknightRect* b) {
    ftMetaknightRect rect = *a;
    if (rect.m_x1 < b->m_x1) {
        rect.m_x1 = b->m_x1;
    }
    if (rect.m_y1 > b->m_y1) {
        rect.m_y1 = b->m_y1;
    }
    if (rect.m_x0 > b->m_x0) {
        rect.m_x0 = b->m_x0;
    }
    if (rect.m_y0 < b->m_y0) {
        rect.m_y0 = b->m_y0;
    }
    *out = rect;
}

int ftMetaknightStatusUniqProcessFinalHitWait::getFreeAttackGalleryStatusWorkId(soModuleAccesser* moduleAccesser) {
    for (int id = 0x20000003; id < 0x2000000a; id++) {
        if (moduleAccesser->getWorkManageModule().getInt(id) == -1) {
            return id;
        }
    }
    return -1;
}

void ftMetaknightStatusUniqProcessFinalHitWait::initStatus(soModuleAccesser* moduleAccesser) {
    // TODO: not reconstructed yet.
}

void ftMetaknightStatusUniqProcessFinalHitWait::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftMetaknightStatusUniqProcessFinalHitWait::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x121 && nextStatus != 0x120) {
        ftMetaknightStatusUniqProcessFinalUtil::notifyMetaknightFinalEnd(moduleAccesser);
        ftMetaknightStatusUniqProcessFinalUtil::unLinkFinalTargetAll(moduleAccesser);
    }
}

ftMetaknightStatusUniqProcessFinalHitWait g_ftMetaknightStatusUniqProcessFinalHitWait;
