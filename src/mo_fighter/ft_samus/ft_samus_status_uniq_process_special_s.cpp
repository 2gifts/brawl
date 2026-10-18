#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/article/so_generate_article_manage_module.h>
#include <types.h>

// Samus's Charge Shot (special S): the shot article is generated and shot while the charge flag is held.
// The initStatus body is not reconstructed yet (class shape only).
class ftSamusArticleNullView {
public:
    virtual bool isNull() const;
};

class ftSamusStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftSamusStatusUniqProcessSpecialS g_ftSamusStatusUniqProcessSpecialS;

void ftSamusStatusUniqProcessSpecialS::initStatus(soModuleAccesser*) {}

void ftSamusStatusUniqProcessSpecialS::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x113) {
        return;
    }
    int statusKind = moduleAccesser->getStatusModule().getStatusKind();
    if (!moduleAccesser->getWorkManageModule().isFlag(0x22000010)) {
        return;
    }
    // HYPOTHESIS: status kinds 0x124 and 0x125 use article 6, other kinds use article 2
    int articleId = 2;
    if (statusKind >= 0x126) {
        articleId = 2;
    } else if (statusKind >= 0x124) {
        articleId = 6;
    }
    soGenerateArticleManageModule& articles =
        *static_cast<soGenerateArticleManageModule*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule);
    if (articles.isGeneratable(articleId)) {
        soArticle* article = articles.generate(articleId, false, false);
        if (!reinterpret_cast<ftSamusArticleNullView*>(article)->isNull()) {
            articles.entry(article);
            articles.shoot(articleId, 0, true);
        }
    }
    moduleAccesser->getWorkManageModule().offFlag(0x22000010);
}
