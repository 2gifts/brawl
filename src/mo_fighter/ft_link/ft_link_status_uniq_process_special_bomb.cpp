#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <ft/fighter.h>
#include <ft/ft_owner.h>
#include <it/item.h>
#include <so/article/so_article.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class soModuleAccesser;

class ftLinkStatusUniqProcessSpecialBomb : public soStatusUniqProcess {
public:
    virtual ~ftLinkStatusUniqProcessSpecialBomb() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

void ftLinkStatusUniqProcessSpecialBomb::initStatus(soModuleAccesser*) { }

void ftLinkStatusUniqProcessSpecialBomb::execFixPos(soModuleAccesser* moduleAccesser) {
    const soModuleEnumeration* modules = moduleAccesser->m_enumerationStart;
    soWorkManageModule& work = *modules->m_workManageModule;
    soGenerateArticleManageModule* articles = static_cast<soGenerateArticleManageModule*>(
        modules->m_generateArticleManageModule);
    bool canCreate = work.isFlag(0x22000011);
    bool alreadyCreated = work.isFlag(0x22000012);
    if (canCreate && !alreadyCreated) {
        if (articles->isGeneratable(6)) {
            soArticle* article = articles->generate(6, NULL, NULL);
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

ftLinkStatusUniqProcessSpecialBomb g_ftLinkStatusUniqProcessSpecialBomb;
