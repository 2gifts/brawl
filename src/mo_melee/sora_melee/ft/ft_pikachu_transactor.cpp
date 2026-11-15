#include <StaticAssert.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/pikachu/ft_pikachu_transactor.h>
#include <mt/mt_matrix.h>
#include <so/so_module_accesser.h>
#include <so/team/so_team.h>
#include <wn/pikachu/wn_pikachu_thunder_jolt2.h>

// The jolt article (wn/pikachu/wn_pikachu_thunder_jolt.h needs the weapon builder chain): the members this unit uses.
class wnPikachuThunderJolt : public Weapon {
public:
    // HYPOTHESIS: argument types; kind is 3, or 0xe for Kirby's copy.
    void activate(int founderTaskId, u32 resId, Vec3f* pos, int teamNo, int kind, float lr);
    static bool activeArticle(wnPikachuThunderJolt2* jolt2, soModuleAccesser* acc);
};

ftPikachuTransactor::ftPikachuTransactor() { }
ftPikachuTransactor::~ftPikachuTransactor() { }

// The Thunder Jolt is article 0xd (started with Pikachu's Kirby resource id) and its ground-travelling second stage 0xe.
bool ftPikachuTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xd) {
        wnPikachuThunderJolt* jolt = dynamic_cast<wnPikachuThunderJolt*>(article);
        if (jolt == NULL) {
            return false;
        }
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        return activeArticle(jolt, acc, resources.getKirbyResId(Fighter_Pikachu));
    } else if (article->getArticleId() == 0xe) {
        wnPikachuThunderJolt2* jolt2 = dynamic_cast<wnPikachuThunderJolt2*>(article);
        if (jolt2 == NULL) {
            return false;
        }
        return wnPikachuThunderJolt::activeArticle(jolt2, acc);
    }
    return false;
}

// The jolt starts from the top node, offset by the ground or air spawn constants, at the ground's depth.
bool ftPikachuTransactor::activeArticle(wnPikachuThunderJolt* weapon, soModuleAccesser* acc, u32 etcResId) {
    Vec3f offset;
    if (acc->getSituationModule().getKind() == 0) {
        offset.m_x = 0.0f;
        offset.m_y = acc->getConstantFloatKirby(0xfa1);
        offset.m_z = acc->getConstantFloatKirby(0xfa0);
    } else {
        offset.m_x = 0.0f;
        offset.m_y = acc->getConstantFloatKirby(0xfa3);
        offset.m_z = acc->getConstantFloatKirby(0xfa2);
    }
    Matrix mtx(true);
    acc->getModelModule().getNodeGlobalMtx(0, &mtx, false);
    Vec3f pos;
    mtx.mulPos(&offset, &pos);
    pos.m_z = acc->getGroundModule().getZ();
    int kind = 3;
    if (acc->getStageObject().soGetSubKind() == Fighter_Kirby) {
        kind = 0xe;
    }
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, etcResId, &pos, acc->getTeamModule().getTeam()->getNo(), kind,
                     acc->getPostureModule().getLr());
    return false;
}
