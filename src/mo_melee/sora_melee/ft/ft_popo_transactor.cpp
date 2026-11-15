#include <StaticAssert.h>
#include <ft/iceclimber/ft_iceclimber_transactor.h>
#include <so/so_module_accesser.h>
#include <wn/wn_weapon_builder.h>

// The ice shot article (its header needs the weapon builder chain): the members this unit uses.
class wnPopoIceShot : public Weapon {
public:
    void shoot();
};

// TODO: activeArticle and activeArticle1 (the ice shot start).

ftPopoTransactor::ftPopoTransactor() { }
ftPopoTransactor::~ftPopoTransactor() { }

// Any situation but the air (2) clears work flag 0x1200003d.
void ftPopoTransactor::notifyEventChangeSituation(int situationKind, int prevSituationKind, soModuleAccesser* moduleAccesser) {
    if (situationKind != 2) {
        moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
    }
}

// The ice shot is article 0xd.
bool ftPopoTransactor::shootArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xd) {
        wnPopoIceShot* shot = dynamic_cast<wnPopoIceShot*>(article);
        if (shot == NULL) {
            return false;
        }
        shot->shoot();
        return false;
    }
    return false;
}
