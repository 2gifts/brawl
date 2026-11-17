#include <StaticAssert.h>
#include <ft/ft_entry.h>
#include <ft/lucas/ft_lucas_transactor.h>
#include <so/so_module_accesser.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module_impl.h>
#include <wn/lucas/wn_lucas_pkflash.h>

ftLucasTransactor::ftLucasTransactor() { }
ftLucasTransactor::~ftLucasTransactor() { }

// The PK Flash is article 0xd.
bool ftLucasTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    switch (article->getArticleId()) {
    case 0xd: {
        wnLucasPKFlash* weapon = dynamic_cast<wnLucasPKFlash*>(article);
        return activeArticle1(weapon, acc);
    }
    default:
        return false;
    }
}

bool ftLucasTransactor::shootArticle(soArticle* article, soModuleAccesser* acc) {
    switch (article->getArticleId()) {
    case 0xd:
        return shootArticle1(dynamic_cast<wnLucasPKFlash*>(article), acc);
    default:
        return false;
    }
}

// PK Flash: starts at the head node (Kirby's copy uses its own node), raised by 3 units scaled with the fighter.
bool ftLucasTransactor::activeArticle1(wnLucasPKFlash* weapon, soModuleAccesser* acc) {
    int node = acc->getStageObject().soGetSubKind() == Fighter_Kirby ? 0x198 : 0x27;
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(node, false);
    float lr = acc->getPostureModule().getLr();
    pos.m_y += 3.0f * acc->getPostureModule().getScale();
    pos.m_z = 0.0f;
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, acc->getTeamModule().getTeam()->getNo(), &pos, lr);
    return true;
}

bool ftLucasTransactor::shootArticle1(wnLucasPKFlash* weapon, soModuleAccesser* acc) {
    weapon->shoot();
    return false;
}
