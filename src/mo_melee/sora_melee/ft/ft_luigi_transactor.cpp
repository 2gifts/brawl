#include <StaticAssert.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/luigi/ft_luigi_transactor.h>
#include <so/so_module_accesser.h>
#include <wn/wn_weapon_builder.h>

// The fireball article (wn/mario/wn_mario_fireball.h needs the weapon builder chain): the members this unit uses.
class wnMarioFireball : public Weapon {
public:
    void activate(int founderTaskId, int resourceId, int team, const Vec3f* pos, int statusKind, float lr);
};

ftLuigiTransactor::ftLuigiTransactor() { }
ftLuigiTransactor::~ftLuigiTransactor() { }

// Same as Mario's, with Luigi's Kirby resource.
bool ftLuigiTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xd) {
        wnMarioFireball* fireball = dynamic_cast<wnMarioFireball*>(article);
        if (fireball == NULL) {
            return false;
        }
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        return activeArticle1(fireball, acc, resources.getKirbyResId(Fighter_Luigi), 0x1b1);
    }
    return false;
}

bool ftLuigiTransactor::activeArticle1(wnMarioFireball* weapon, soModuleAccesser* acc, u32 etcResId, int nodeId) {
    return ftMarioTransactor::activeArticle1(weapon, acc, etcResId, nodeId);
}
