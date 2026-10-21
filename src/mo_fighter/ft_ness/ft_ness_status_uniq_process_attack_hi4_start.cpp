#include <so/article/so_article.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftNessStatusUniqProcessAttackHi4Start : public soStatusUniqProcess {
public:
    virtual ~ftNessStatusUniqProcessAttackHi4Start() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

struct ftNessLinkEventYoyo : public soLinkEventArgs {
    int unk8;
    int unkC;

    ftNessLinkEventYoyo(int taskId, int kind) : soLinkEventArgs(0x455), unk8(taskId), unkC(kind) { }
};

void ftNessStatusUniqProcessAttackHi4Start::initStatus(soModuleAccesser* moduleAccesser) {
    int status = moduleAccesser->getStatusModule().getStatusKind();
    if (status == 0x30 || status == 0x2d) {
        soGenerateArticleManageModule& articles = moduleAccesser->getGenerateArticleManageModule();
        soLinkModule& link = moduleAccesser->getLinkModule();
        soArticle* article5 = articles.generate(5, false, false);
        if (article5->isNull() == false) {
            articles.entry(article5);
        }
        soArticle* article4 = articles.generate(4, false, false);
        if (article4->isNull() == false) {
            articles.entry(article4);
        }

        ftNessLinkEventYoyo eventToArticle5(article4->getTaskId(), 0x13);
        ftNessLinkEventYoyo eventToArticle4(article5->getTaskId(), 1);
        link.sendEventNodes(-1, article5->getTaskId(), eventToArticle5);
        link.sendEventNodes(-1, article4->getTaskId(), eventToArticle4);
        link.link(7, article5->getTaskId());
        articles.setVisibilityWhole(5, 0, 0);
        articles.setVisibilityWhole(4, 0, 0);
    }
}

void ftNessStatusUniqProcessAttackHi4Start::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().getParentSituationKind(7) == 0) {
        moduleAccesser->getWorkManageModule().onFlag(0x22000010);
    } else {
        moduleAccesser->getWorkManageModule().offFlag(0x22000010);
    }
}

ftNessStatusUniqProcessAttackHi4Start g_ftNessStatusUniqProcessAttackHi4Start;
