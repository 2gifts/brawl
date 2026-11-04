#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftDiddyStatusUniqProcessSpecialPopGun : public soStatusUniqProcess {
public:
    virtual ~ftDiddyStatusUniqProcessSpecialPopGun() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDiddyStatusUniqProcessSpecialPopGun::initStatus(soModuleAccesser* moduleAccesser) {}
void ftDiddyStatusUniqProcessSpecialPopGun::execStatus(soModuleAccesser* moduleAccesser) {
    int status = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(
        0, moduleAccesser->getStatusModule().getStatusKind(), moduleAccesser, true);
    switch (status) {
    case 0x117:
        moduleAccesser->getWorkManageModule().addFloat(1.0f, 0x21000004);
        break;
    case 0x119:
        moduleAccesser->getWorkManageModule().addFloat(1.0f, 0x21000005);
        break;
    }
}
void ftDiddyStatusUniqProcessSpecialPopGun::execFixPos(soModuleAccesser* moduleAccesser) {}
void ftDiddyStatusUniqProcessSpecialPopGun::execStop(soModuleAccesser* moduleAccesser) {}
void ftDiddyStatusUniqProcessSpecialPopGun::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    int status = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(
        0, nextStatus, moduleAccesser, true);
    switch (status) {
    case 0x117:
    case 0x118:
    case 0x119:
    case 0x11A:
        return;
    default:
        break;
    }
    if (moduleAccesser->getStageObject().soGetSubKind() == 5) {
        moduleAccesser->getGenerateArticleManageModule().removeExist(0xD, 0);
    } else {
        moduleAccesser->getGenerateArticleManageModule().removeExist(0, 0);
    }
}

ftDiddyStatusUniqProcessSpecialPopGun g_ftDiddyStatusUniqProcessSpecialPopGun;
