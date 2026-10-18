#include <ft/fighter.h>
#include <ft/ft_owner.h>
#include <it/item.h>
#include <so/article/so_article.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftToonLinkStatusUniqProcessSpecialBomb : public soStatusUniqProcess {
public:
    virtual ~ftToonLinkStatusUniqProcessSpecialBomb() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

void ftToonLinkStatusUniqProcessSpecialBomb::initStatus(soModuleAccesser* moduleAccesser) {
    soItemManageModule& item = moduleAccesser->getItemManageModule();
    if (item.isHaveItem(0)) {
        if (item.getHaveItemKind(0) == 0x5b) {
            moduleAccesser->getStatusModule().changeStatusRequest(0x9b, moduleAccesser);
        }
    }
}

void ftToonLinkStatusUniqProcessSpecialBomb::execFixPos(soModuleAccesser* moduleAccesser) {
    const soModuleEnumeration* modules = moduleAccesser->m_enumerationStart;
    soWorkManageModule& work = *modules->m_workManageModule;
    soGenerateArticleManageModule* articles = static_cast<soGenerateArticleManageModule*>(
        modules->m_generateArticleManageModule);
    bool canCreate = work.isFlag(0x22000011);
    bool alreadyCreated = work.isFlag(0x22000012);
    if (canCreate && !alreadyCreated) {
        if (articles->isGeneratable(6)) {
            soArticle* article = articles->generate(6, false, false);
            if (!article->isNull()) {
                BaseItem& item = dynamic_cast<BaseItem&>(*article);
                articles->entry(article);
                moduleAccesser->getItemManageModule().haveItem(&item, 0, true, true);
                Fighter& fighter = dynamic_cast<Fighter&>(moduleAccesser->getStageObject());
                fighter.getOwner()->addLogInt(1, 0x1d, 0);
            }
        }
        work.onFlag(0x22000012);
    }
}

ftToonLinkStatusUniqProcessSpecialBomb g_ftToonLinkStatusUniqProcessSpecialBomb;
