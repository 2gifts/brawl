// Use the native Fighter observer override before transitive Fighter includes;
// this preserves the verified primary vtable slot of getOwner.
#define FT_MODULE_BUILDER
#include <ft/ft_owner.h>
#include <ft/fighter.h>
#include <StaticAssert.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/pit/ft_pit_transactor.h>
#include <so/so_module_accesser.h>
#include <wn/pit/wn_pit_bow.h>
#include <wn/pit/wn_pit_bow_arrow.h>

ftPitTransactor::ftPitTransactor() { }
ftPitTransactor::~ftPitTransactor() { }

bool ftPitTransactor::activeArticle(wnPitBow* weapon, soModuleAccesser* acc, u32 mdlResId, u32 texResId, bool unk1, bool unk2) {
    weapon->activate(acc->getStageObject().m_taskId, mdlResId, texResId, unk1, unk2);
    return true;
}

bool ftPitTransactor::activeArticle1(wnPitBowArrow* weapon, soModuleAccesser* acc, u32 etcResId) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    int founderTaskId = fighter.m_taskId;
    weapon->activate(founderTaskId, etcResId, fighter.getOwner()->getTeam());
    return true;
}

// The bow is article 0xd and its arrow 0xe; both use Pit's Kirby resource id.
bool ftPitTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    switch (article->getArticleId()) {
    case 0xd: {
        wnPitBow* bow = dynamic_cast<wnPitBow*>(article);
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        u32 resId = resources.getKirbyResId(Fighter_Pit);
        return activeArticle(bow, acc, resId, resId, true, true);
    }
    case 0xe: {
        wnPitBowArrow* arrow = dynamic_cast<wnPitBowArrow*>(article);
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        return activeArticle1(arrow, acc, resources.getKirbyResId(Fighter_Pit));
    }
    default:
        return false;
    }
}

// Releases the arrow: work int 0x20000000 is the charge, work flag 0x22000011 marks a full charge.
bool ftPitTransactor::shootArticle(wnPitBowArrow* weapon, soModuleAccesser* acc) {
    soWorkManageModule& work = acc->getWorkManageModule();
    soPostureModule& posture = acc->getPostureModule();
    int charge = work.getInt(0x20000000);
    bool fullCharge = work.isFlag(0x22000011);
    weapon->toFly(fullCharge, charge, posture.getLr());
    return false;
}

bool ftPitTransactor::shootArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xe) {
        wnPitBowArrow* arrow = dynamic_cast<wnPitBowArrow*>(article);
        return shootArticle(arrow, acc);
    }
    return false;
}
