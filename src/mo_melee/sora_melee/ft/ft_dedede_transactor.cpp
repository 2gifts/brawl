#include <StaticAssert.h>
#include <ft/dedede/ft_dedede_transactor.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <so/link/so_link_event_presenter.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <wn/wn_weapon_builder.h>

// The star missile article (its header needs the weapon builder chain): only its type is used here.
class wnDededeStarmissile : public Weapon { };

// TODO: notifyEventCollisionSearch (the inhale search) and the star missile start.

ftDededeTransactor::ftDededeTransactor() { }
ftDededeTransactor::~ftDededeTransactor() { }

// Inhale statuses (0x112, 0x117): once the search caught an article (work flag 0x22000019), link it, tell it it was
// swallowed (link event 0x835) and change to the swallow status (Kirby's copy has its own).
bool ftDededeTransactor::notifyEventCollisionSearchCheck(soModuleAccesser* moduleAccesser) {
    int status = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, moduleAccesser->getStatusModule().getStatusKind(), moduleAccesser, true);
    if ((status == 0x112 || status == 0x117) && moduleAccesser->getWorkManageModule().isFlag(0x22000019)) {
        int caughtTaskId = moduleAccesser->getWorkManageModule().getInt(0x20000006);
        moduleAccesser->getLinkModule().link(6, caughtTaskId);
        soLinkEventArgs args(0x835);
        moduleAccesser->getLinkModule().sendEventParents(6, args);
        moduleAccesser->getWorkManageModule().onFlag(0x2200001a);
        if (moduleAccesser->getStageObject().soGetSubKind() == Fighter_Kirby) {
            moduleAccesser->getStatusModule().changeStatusRequest(0x1a3, moduleAccesser);
        } else {
            moduleAccesser->getStatusModule().changeStatusRequest(0x127, moduleAccesser);
        }
        moduleAccesser->getWorkManageModule().offFlag(0x22000019);
        return true;
    }
    return false;
}

// The star missile is article 0xd (Dedede's Kirby resource id).
bool ftDededeTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xd) {
        wnDededeStarmissile* missile = dynamic_cast<wnDededeStarmissile*>(article);
        if (missile == NULL) {
            return false;
        }
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        return activeArticle(missile, acc, resources.getKirbyResId(Fighter_Dedede));
    }
    return false;
}
