#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/metaknight/ft_metaknight_status_views.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>


class ftMetaknightStatusUniqProcessFinalEnd : public soStatusUniqProcess {
public:
    virtual ~ftMetaknightStatusUniqProcessFinalEnd() { }
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);

    // Finds the first link slot in 6..11 that is not linked; -1 when all are in use.
    int getFreeFinalLinkWork(soModuleAccesser* moduleAccesser);
};

void ftMetaknightStatusUniqProcessFinalEnd::execFixPos(soModuleAccesser* moduleAccesser) {
    // TODO: the hit-window loop (links 6..11, attackFinal) is not reconstructed yet.
}

void ftMetaknightStatusUniqProcessFinalEnd::exitStatus(soModuleAccesser* moduleAccesser, int) {
    ftMetaknightStatusUniqProcessFinalUtil::notifyMetaknightFinalEnd(moduleAccesser);
    ftMetaknightStatusUniqProcessFinalUtil::unLinkFinalTargetAll(moduleAccesser);
}

int ftMetaknightStatusUniqProcessFinalEnd::getFreeFinalLinkWork(soModuleAccesser* moduleAccesser) {
    for (int i = 6; i < 12; i++) {
        if (!moduleAccesser->getLinkModule().isLink(i)) {
            return i;
        }
    }
    return -1;
}

ftMetaknightStatusUniqProcessFinalEnd g_ftMetaknightStatusUniqProcessFinalEnd;
