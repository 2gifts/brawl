// Use the native Fighter observer override before transitive Fighter includes;
// this preserves the verified primary vtable slot of getOwner.
#define FT_MODULE_BUILDER
#include <ft/ft_owner.h>
#include <ft/fighter.h>
#include <StaticAssert.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/toonlink/ft_toonlink_transactor.h>
#include <so/so_module_accesser.h>
#include <wn/toon_link/wn_toon_link_bow.h>
#include <wn/toon_link/wn_toon_link_bow_arrow.h>

ftToonLinkTransactor::ftToonLinkTransactor() { }
ftToonLinkTransactor::~ftToonLinkTransactor() { }

// The bow is article 0xd and its arrow 0xe; both use ToonLink's Kirby resource id.
bool ftToonLinkTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    switch (article->getArticleId()) {
    case 0xd: {
        wnToonLinkBow* bow = dynamic_cast<wnToonLinkBow*>(article);
        if (bow != NULL) {
            ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
            return activeArticle1(bow, acc, resources.getKirbyResId(Fighter_ToonLink));
        }
        break;
    }
    case 0xe: {
        wnToonLinkBowArrow* arrow = dynamic_cast<wnToonLinkBowArrow*>(article);
        if (arrow != NULL) {
            ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
            return activeArticle2(arrow, acc, resources.getKirbyResId(Fighter_ToonLink), acc->getPostureModule().getLr());
        }
        break;
    }
    }
    return false;
}

bool ftToonLinkTransactor::activeArticle1(wnToonLinkBow* weapon, soModuleAccesser* acc, u32 etcResId) {
    weapon->activate(acc->getStageObject().m_taskId, etcResId);
    return true;
}

bool ftToonLinkTransactor::activeArticle2(wnToonLinkBowArrow* weapon, soModuleAccesser* acc, u32 etcResId, float lr) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    int founderTaskId = fighter.m_taskId;
    weapon->activate(founderTaskId, etcResId, fighter.getOwner()->getTeam(), lr);
    return true;
}

// The arrow is article 0xe.
bool ftToonLinkTransactor::shootArticle(soArticle* article, soModuleAccesser* acc) {
    switch (article->getArticleId()) {
    case 0xe: {
        wnToonLinkBowArrow* arrow = dynamic_cast<wnToonLinkBowArrow*>(article);
        if (arrow != NULL) {
            return shootArticle1(arrow, acc);
        }
        break;
    }
    }
    return false;
}

// Releases the arrow with the charge (work float 0x21000004) scaled by the bow parameter.
bool ftToonLinkTransactor::shootArticle1(wnToonLinkBowArrow* weapon, soModuleAccesser* acc) {
    float* param = g_ftCommonDataAccesser.getToonLinkBowParam(acc->getStageObject().soGetSubKind());
    float charge = acc->getWorkManageModule().getFloat(0x21000004);
    float lr = acc->getPostureModule().getLr();
    weapon->toFly(lr, param[3], charge / param[0]);
    return false;
}
