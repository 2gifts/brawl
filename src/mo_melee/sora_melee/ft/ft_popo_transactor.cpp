#define MT_VEC3F_MUL_INLINE
#define MT_VEC3F_SCALE_XY_IN_PLACE // BYTES: the muzzle offset scale keeps x/y in their load register
#include <StaticAssert.h>
#include <ft/iceclimber/ft_iceclimber_transactor.h>
#include <so/so_module_accesser.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module_impl.h>
#include <wn/wn_weapon_builder.h>

// The ice shot article (its header needs the weapon builder chain): the members this unit uses.
class wnPopoIceShot : public Weapon {
public:
    void activate(int founderTaskId, int resourceId, int team, const Vec3f* pos, float lr, int unk40);
    void shoot();
};

// TODO: activeArticle (the dispatcher).

ftPopoTransactor::ftPopoTransactor() { }
ftPopoTransactor::~ftPopoTransactor() { }

// Any situation but the air (2) clears work flag 0x1200003d.
void ftPopoTransactor::notifyEventChangeSituation(int situationKind, int prevSituationKind, soModuleAccesser* moduleAccesser) {
    if (situationKind != 2) {
        moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
    }
}

// The ice shot starts at the top node, pushed forward by the muzzle offset (scaled with the fighter) on the ground's plane;
// work int 0x10000041 is handed to the shot.
bool ftPopoTransactor::activeArticle1(wnPopoIceShot* weapon, soModuleAccesser* acc, u32 etcResId) {
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(0, false);
    float offsetY = acc->getConstantFloatKirby(0xfa3);
    float dir = acc->getPostureModule().getLr() < 0.0f ? -1 : 1;
    Vec3f offset(acc->getConstantFloatKirby(0xfa2) * dir, offsetY, 0.0f);
    Vec3fScale(&offset, &offset, acc->getPostureModule().getScale());
    Vec3fAdd(&pos, &pos, &offset);
    pos.m_z = acc->getGroundModule().getZ();
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, etcResId, acc->getTeamModule().getTeam()->getNo(), &pos, acc->getPostureModule().getLr(),
                     acc->getWorkManageModule().getInt(0x10000041));
    return true;
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
