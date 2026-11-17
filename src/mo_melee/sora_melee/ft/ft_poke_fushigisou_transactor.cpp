#include <StaticAssert.h>
#include <ft/ft_entry.h>
#include <ft/poke/ft_poke_fushigisou_transactor.h>
#include <so/so_module_accesser.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module_impl.h>
#include <wn/wn_weapon_builder.h>

// The seed article (wn/poke/wn_poke_fushigisou_seed.h needs the weapon builder chain): the members this unit uses.
class wnPokeFushigisouSeed : public Weapon {
public:
    // HYPOTHESIS: argument meanings.
    void activate(int founderTaskId, int team, Vec3f* pos, int unk, int kind, float lr, float speed);
};

ftPokeFushigisouTransactor::ftPokeFushigisouTransactor() { }
ftPokeFushigisouTransactor::~ftPokeFushigisouTransactor() { }

// The seed is the only article the transactor starts (kind 1 for Kirby's copy).
bool ftPokeFushigisouTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    wnPokeFushigisouSeed* seed = dynamic_cast<wnPokeFushigisouSeed*>(article);
    return activeArticle1(seed, acc, 1);
}

// The seed leaves the mouth node (Kirby's own node for the copy), raised by the height constant, on the stage plane.
bool ftPokeFushigisouTransactor::activeArticle1(wnPokeFushigisouSeed* weapon, soModuleAccesser* acc, int kind) {
    float speed = acc->getConstantFloatKirby(0xfa0);
    float lr = acc->getPostureModule().getLr();
    Vec3f pos;
    if (acc->getStageObject().soGetSubKind() == Fighter_Kirby) {
        pos = acc->getModelModule().getNodeGlobalPosition(0x19b, false);
    } else {
        pos = acc->getModelModule().getNodeGlobalPosition(0x21, false);
    }
    pos.m_y += acc->getConstantFloatKirby(0xfa1);
    pos.m_z = 0.0f;
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, acc->getTeamModule().getTeam()->getNo(), &pos, -1, kind, lr, speed);
    return false;
}
