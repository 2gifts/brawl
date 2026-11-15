// BYTES: the transactor singleton, its guard and the status table keep separate addresses in this unit.
#pragma pool_data off
#define FT_LUCARIO_TRANSACTOR_MELEE_INSTANCE
#include <StaticAssert.h>
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <ft/lucario/ft_lucario_transactor.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <wn/lucario/wn_lucario_aura_ball.h>

class ftLucarioStatusUniqProcessSpecialN : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialN() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);

private:
    static bool keepsCharge(soModuleAccesser* moduleAccesser, int origStatus, int nextStatus);
    // The statuses that keep a charged aura ball when the charge is left for them (defined after the code, as the
    // order of the unit's read-only data shows).
    static const int s_keepStatuses[6];
};

ftLucarioStatusUniqProcessSpecialN g_ftLucarioStatusUniqProcessSpecialN;

// Leaving the charge for one of these statuses keeps the charged aura ball (as a stored charge).
// HYPOTHESIS: the routine boundary; the charge mark (common effect 0x1a) stays only when the hold status is left.
inline bool ftLucarioStatusUniqProcessSpecialN::keepsCharge(soModuleAccesser* moduleAccesser, int origStatus, int nextStatus) {
    if (origStatus == 0x117 || origStatus == 0x118) {
        for (int i = 0; i < 6; i++) {
            if (nextStatus == s_keepStatuses[i]) {
                if (origStatus == 0x118) {
                    moduleAccesser->getEffectModule().reqCommon(0x1a, 0.0f);
                }
                return true;
            }
        }
    }
    moduleAccesser->getEffectModule().removeCommon(0x1a);
    return false;
}

// Statuses 0x112 (start), 0x117 (charge), 0x118 (hold) and 0x119 (shoot) for Lucario, 0x1aa-0x1ad for the copying Kirby.
void ftLucarioStatusUniqProcessSpecialN::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    soGenerateArticleManageModule& generate = moduleAccesser->getGenerateArticleManageModule();
    int status = moduleAccesser->getStatusModule().getStatusKind();
    int startStatus;
    int chargeStatus;
    int holdStatus;
    int shootStatus;
    int articleId;
    switch (moduleAccesser->m_stageObject->soGetSubKind()) {
    case 0x21:
    default:
        startStatus = 0x112;
        chargeStatus = 0x117;
        holdStatus = 0x118;
        shootStatus = 0x119;
        articleId = 0;
        break;
    case 5:
        startStatus = 0x1aa;
        chargeStatus = 0x1ab;
        holdStatus = 0x1ac;
        shootStatus = 0x1ad;
        articleId = 0xd;
        break;
    }
    if ((status == startStatus && nextStatus != chargeStatus && nextStatus != shootStatus) ||
        (status == chargeStatus && nextStatus != holdStatus && nextStatus != shootStatus) ||
        (status == holdStatus && nextStatus != shootStatus) || (status == shootStatus && generate.isExist(articleId))) {
        getLucarioTransactor()->saveAuraBallStatus(moduleAccesser, false, 0, 0);
        int origStatus = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, status, moduleAccesser, true);
        soArticle* article;
        if (keepsCharge(moduleAccesser, origStatus, nextStatus) && (article = generate.getArticle(articleId)) != NULL) {
            wnLucarioAuraBall& auraBall = dynamic_cast<wnLucarioAuraBall&>(*article);
            int chargeFrame = auraBall.getChargeFrame();
            int chargeLevel = auraBall.getChargeLevel();
            getLucarioTransactor()->saveAuraBallStatus(moduleAccesser, true, chargeFrame, chargeLevel);
        }
        generate.removeExist(articleId, 0);
    }
    if (status == startStatus && nextStatus == shootStatus) {
        moduleAccesser->getEffectModule().removeCommon(0x1a);
    }
    if (status == shootStatus) {
        getLucarioTransactor()->saveAuraBallStatus(moduleAccesser, false, 0, 0);
    }
}

const int ftLucarioStatusUniqProcessSpecialN::s_keepStatuses[6] = {0x1a, 0x1e, 0x1f, 0x20, 0x21, 0x34};
