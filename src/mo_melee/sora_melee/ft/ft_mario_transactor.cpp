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
    soTeamModule& team = acc->getTeamModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, etcResId, team.getTeam()->getNo(), &pos, 0, acc->getPostureModule().getLr());
    return false;
}

// TODO: clean up, give variables real names
namespace jj_2D9304 {
extern "C" void unk17__25ftKirbyCopyAbilityProcessFP16soModuleAccesser(void) {}
}

// TODO: clean up, give variables real names
namespace jj_2D9308 {
extern "C" s32 unk16__25ftKirbyCopyAbilityProcessFP16soModuleAccesser(void) {
    return 0x0;
}
}

// TODO: clean up, give variables real names
namespace jj_2D9310 {
extern "C" void unk15__25ftKirbyCopyAbilityProcessFP16soModuleAccesser(void) {}
}

// TODO: clean up, give variables real names
namespace jj_2D9314 {
extern "C" void unk14__25ftKirbyCopyAbilityProcessFP16soModuleAccesser(void) {}
}

// TODO: clean up, give variables real names
namespace jj_2D9318 {
extern "C" s32 shootArticle__25ftKirbyCopyAbilityProcessFP9soArticleP16soModuleAccesser(void) {
    return 0x0;
}
}

// TODO: clean up, give variables real names
namespace jj_2D9320 {
extern "C" s32 unk11__25ftKirbyCopyAbilityProcessFP16soModuleAccesser(void) {
    return 0x0;
}
}

// TODO: clean up, give variables real names
namespace jj_2D9328 {
extern "C" void notifyEventCollisionSearch__25ftKirbyCopyAbilityProcessFP14soCollisionLogP16soModuleAccesser(void) {}
}

// TODO: clean up, give variables real names
namespace jj_2D932C {
extern "C" s32 notifyEventCollisionShieldCheck__25ftKirbyCopyAbilityProcessFP16soModuleAccesser(void) {
    return 0x0;
}
}

// TODO: clean up, give variables real names
namespace jj_2D9334 {
extern "C" void notifyEventCollisionShield__25ftKirbyCopyAbilityProcessFP23soCollisionAttackModulefP14soCollisionLogiffP16soModuleAccesser(void) {}
}

// TODO: clean up, give variables real names
namespace jj_2D9338 {
extern "C" void unk7__25ftKirbyCopyAbilityProcessFP16soModuleAccesser(void) {}
}

// TODO: clean up, give variables real names
namespace jj_2D933C {
extern "C" void notifyEventChangeSituation__25ftKirbyCopyAbilityProcessFiiP16soModuleAccesser(void) {}
}

// TODO: clean up, give variables real names
namespace jj_2D9340 {
extern "C" void processFixPosition__25ftKirbyCopyAbilityProcessFP16soModuleAccesser(void) {}
}

// TODO: clean up, give variables real names
namespace jj_2D9344 {
extern "C" void unk4__25ftKirbyCopyAbilityProcessFP16soModuleAccesser(void) {}
}

// TODO: clean up, give variables real names
namespace jj_2D9348 {
extern "C" void processUpdate__25ftKirbyCopyAbilityProcessFP16soModuleAccesser(void) {}
}

// TODO: clean up, give variables real names
namespace jj_2D934C {
extern "C" void exit__25ftKirbyCopyAbilityProcessFP16soModuleAccesser(void) {}
}

// TODO: clean up, give variables real names
namespace jj_2D9350 {
extern "C" void init__25ftKirbyCopyAbilityProcessFP16soModuleAccesser(void) {}
}

// TODO: clean up, give variables real names
namespace jj_2D9354 {
extern "C" s32 activeArticle__25ftKirbyCopyAbilityProcessFP9soArticleP16soModuleAccesser(void) {
    return 0x0;
}
}

