#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/article/so_generate_article_manage_module.h>
#include <types.h>

// Zero Suit Samus's final smash: the status process only resets the Samus article when it is updated or exits.
// HYPOTHESIS: work flag 0x22000014 is set when the transformation starts; work int 0x10000040 is cleared when it ends.
class ftSZerosuitStatusUniqProcessFinalSamus : public soStatusUniqProcess {
public:
    virtual ~ftSZerosuitStatusUniqProcessFinalSamus() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

ftSZerosuitStatusUniqProcessFinalSamus g_ftSZerosuitStatusUniqProcessFinalSamus;

void ftSZerosuitStatusUniqProcessFinalSamus::exitStatus(soModuleAccesser* moduleAccesser, int) {
    soGenerateArticleManageModule& articles =
        *static_cast<soGenerateArticleManageModule*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule);
    if (articles.isExist(3)) {
        moduleAccesser->getWorkManageModule().setInt(0, 0x10000040);
        // HYPOTHESIS: article 3 is the Samus article; shoot it with selector 0
        articles.shoot(3, 0, true);
    }
}

void ftSZerosuitStatusUniqProcessFinalSamus::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    if (workManage.isFlag(0x22000014)) {
        workManage.offFlag(0x22000014);
        workManage.setInt(0, 0x10000040);
        soGenerateArticleManageModule& articles =
            *static_cast<soGenerateArticleManageModule*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule);
        articles.shoot(3, 0, true);
    }
}
