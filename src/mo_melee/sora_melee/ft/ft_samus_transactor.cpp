#include <StaticAssert.h>
#include <ft/samus/ft_samus_transactor.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/so_module_accesser.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module_impl.h>
#include <wn/samus/wn_samus_cshot.h>

ftSamusTransactor::ftSamusTransactor() { }
ftSamusTransactor::~ftSamusTransactor() { }

// HYPOTHESIS: the three work ints hold the Charge Shot effect handles (exit removes the first one).
void ftSamusTransactor::init(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    work.setInt(0, 0x10000040);
    work.setInt(0, 0x10000041);
    work.setInt(0, 0x10000042);
}

void ftSamusTransactor::exit(soModuleAccesser* moduleAccesser) {
    int effectHandle = moduleAccesser->getWorkManageModule().getInt(0x10000040);
    if (effectHandle != 0) {
        moduleAccesser->getEffectModule().remove(effectHandle);
    }
}

inline bool ftSamusTransactor::activeArticle1(wnSamusCShot* shot, soModuleAccesser* acc, u32 etcResId) {
    int founderTaskId = acc->getStageObject().m_taskId;
    soTeamModule* const teamModule = &acc->getTeamModule();
    shot->activate(founderTaskId, etcResId, teamModule->getTeam()->getNo(), acc->getStageObject().soGetSubKind());
    return true;
}

// The Charge Shot is article 0xd; its kind is the fighter's sub kind (Samus or Kirby).
bool ftSamusTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xd) {
        wnSamusCShot* shot = dynamic_cast<wnSamusCShot*>(article);
        if (shot == NULL) {
            return false;
        }
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        return activeArticle1(shot, acc, resources.getKirbyResId(Fighter_Samus));
    }
    return false;
}

// Keeps the "fully charged" effect on the arm cannon while the stored charge (work int 0x10000041) is at the limit;
// work int 0x10000040 holds the effect handle. Kirby's copy uses his own effect and node.
void ftSamusTransactor::processUpdateSpecialNTransact(soModuleAccesser* acc) {
    soWorkManageModule& work = acc->getWorkManageModule();
    soEffectModule& effect = acc->getEffectModule();
    int kind = acc->getStageObject().soGetSubKind();
    int* chargeLimit = static_cast<int*>(g_ftCommonDataAccesser.getParamSamusChargeShot(static_cast<ftKind>(kind)));
    if (work.getInt(0x10000041) >= *chargeLimit) {
        if (work.getInt(0x10000040) == 0) {
            // The two vectors are temporaries of the call (offset, then rotation).
            int handle;
            if (kind == Fighter_Samus) {
                handle = effect.reqFollow(static_cast<EfID>(0x40006), 0x2b, &Vec3f(-8.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, true, 0, 0, -1);
            } else {
                handle = effect.reqFollow(static_cast<EfID>(0x1080004), 0x1b8, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, true, 0, 0, -1);
            }
            work.setInt(handle, 0x10000040);
        }
    } else if (work.getInt(0x10000040) != 0) {
        effect.kill(work.getInt(0x10000040), true, true);
        work.setInt(0, 0x10000040);
    }
}

void ftSamusTransactor::processUpdate(soModuleAccesser* moduleAccesser) {
    processUpdateSpecialNTransact(moduleAccesser);
}

// Slot 13: fires the Charge Shot (article 0xd) with the charge counted in work int 0x10000042.
bool ftSamusTransactor::shootArticle(soArticle* article, soModuleAccesser* acc) {
    switch (article->getArticleId()) {
    case 0xd: {
        wnSamusCShot* shot = dynamic_cast<wnSamusCShot*>(article);
        int* chargeLimit = static_cast<int*>(g_ftCommonDataAccesser.getParamSamusChargeShot(static_cast<ftKind>(acc->getStageObject().soGetSubKind())));
        soWorkManageModule& work = acc->getWorkManageModule();
        shot->setCharge(static_cast<float>(work.getInt(0x10000042)) / static_cast<float>(*chargeLimit));
        soPostureModule& posture = acc->getPostureModule();
        shot->toShoot(posture.getLr(), work.getInt(0x10000042));
        return false;
    }
    }
    return false;
}
