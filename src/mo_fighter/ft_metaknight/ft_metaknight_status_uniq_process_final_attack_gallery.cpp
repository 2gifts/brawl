#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/metaknight/ft_metaknight_status_views.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>


class ftMetaknightStatusUniqProcessFinalAttackGallery : public soStatusUniqProcess {
public:
    virtual ~ftMetaknightStatusUniqProcessFinalAttackGallery() { }
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftMetaknightStatusUniqProcessFinalAttackGallery::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: work id 0x20000002 is a countdown that is decremented while it is positive.
    if (moduleAccesser->getWorkManageModule().getInt(0x20000002) > 0) {
        moduleAccesser->getWorkManageModule().decInt(0x20000002);
    }
}

void ftMetaknightStatusUniqProcessFinalAttackGallery::execFixPos(soModuleAccesser* moduleAccesser) {
    // TODO: the gallery attack sequence (attackGallery, entry lookup) is not reconstructed yet.
}

void ftMetaknightStatusUniqProcessFinalAttackGallery::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x120) {
        ftMetaknightStatusUniqProcessFinalUtil::notifyMetaknightFinalEnd(moduleAccesser);
        ftMetaknightStatusUniqProcessFinalUtil::unLinkFinalTargetAll(moduleAccesser);
    }
}

ftMetaknightStatusUniqProcessFinalAttackGallery g_ftMetaknightStatusUniqProcessFinalAttackGallery;
