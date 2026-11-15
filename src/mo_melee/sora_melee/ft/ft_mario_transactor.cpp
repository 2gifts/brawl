#include <StaticAssert.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/mario/ft_mario_transactor.h>
#include <so/so_module_accesser.h>
#include <so/team/so_team.h>
#include <wn/wn_weapon_builder.h>

// The fireball article (wn/mario/wn_mario_fireball.h needs the weapon builder chain): the members this unit uses.
class wnMarioFireball : public Weapon {
public:
    void activate(int founderTaskId, int resourceId, int team, const Vec3f* pos, int statusKind, float lr);
};

ftMarioTransactor::ftMarioTransactor() { }
ftMarioTransactor::~ftMarioTransactor() { }

// The fireball is article 0xd; Kirby's copy starts it from his own hand node with Mario's Kirby resource.
bool ftMarioTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xd) {
        wnMarioFireball* fireball = dynamic_cast<wnMarioFireball*>(article);
        if (fireball == NULL) {
            return false;
        }
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        return activeArticle1(fireball, acc, resources.getKirbyResId(Fighter_Mario), 0x1b1);
    }
    return false;
}

bool ftMarioTransactor::activeArticle1(wnMarioFireball* weapon, soModuleAccesser* acc, u32 etcResId, int nodeId) {
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(nodeId, false);
    pos.m_z = acc->getGroundModule().getZ();
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, etcResId, acc->getTeamModule().getTeam()->getNo(), &pos, 0, acc->getPostureModule().getLr());
    return false;
}
