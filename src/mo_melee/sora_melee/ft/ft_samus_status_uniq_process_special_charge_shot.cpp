#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// BYTES: the unit's own classes are defined ahead of every other header (class definition order decides the order of the
// vtables and RTTI in .data: the five status vtables follow the article and module classes' in the original).
#include <ft/samus/ft_samus_status_uniq_process_special_n.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_controller_module_impl.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <ft/samus/ft_samus_data.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/controller/so_controller_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/motion/so_motion_change_param.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/samus/wn_samus_cshot.h>

ftSamusStatusUniqProcessSpecialNStart g_ftSamusStatusUniqProcessSpecialNStart;
ftSamusStatusUniqProcessSpecialAirNStart g_ftSamusStatusUniqProcessSpecialAirNStart;
ftSamusStatusUniqProcessSpecialNHold g_ftSamusStatusUniqProcessSpecialNHold;
ftSamusStatusUniqProcessSpecialNFire g_ftSamusStatusUniqProcessSpecialNFire;
ftSamusStatusUniqProcessSpecialAirNFire g_ftSamusStatusUniqProcessSpecialAirNFire;

// HYPOTHESIS: flag 0x1200003E requests a fresh charge: clear work bank 2, the charge and the air flag 0x22000014 follow the situation.
void ftSamusStatusUniqProcessSpecialNStart::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& request = moduleAccesser->getWorkManageModule();
    if (request.isFlag(0x1200003E)) {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        work.clearAll(2);
        work.setInt(0, 0x10000042);
        work.onFlag(0x22000010);
        if (moduleAccesser->getSituationModule().getKind() == 0) {
            work.offFlag(0x22000014);
        } else {
            work.onFlag(0x22000014);
        }
        request.offFlag(0x1200003E);
    }
}
void ftSamusStatusUniqProcessSpecialNStart::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, nextStatus, moduleAccesser, true)) {
    case 0x118:
    case 0x11A:
    case 0x11B:
        break;
    default: {
        moduleAccesser->getWorkManageModule().setInt(0, 0x10000041);
        int articleId;
        if (moduleAccesser->getStageObject().soGetSubKind() == Fighter_Kirby) {
            articleId = 13;
        } else {
            articleId = static_cast<ftSamusData*>(g_ftCommonDataAccesser.getData(Fighter_Samus))->chargeShotArticleId;
        }
        moduleAccesser->getGenerateArticleManageModule().removeExist(articleId, 0);
        moduleAccesser->getEffectModule().removeCommon(0x1A);
        break;
    }
    }
}
void ftSamusStatusUniqProcessSpecialAirNStart::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& request = moduleAccesser->getWorkManageModule();
    if (request.isFlag(0x1200003E)) {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        work.clearAll(2);
        work.setInt(0, 0x10000042);
        work.onFlag(0x22000010);
        if (moduleAccesser->getSituationModule().getKind() == 0) {
            work.offFlag(0x22000014);
        } else {
            work.onFlag(0x22000014);
        }
        request.offFlag(0x1200003E);
    }
}
void ftSamusStatusUniqProcessSpecialAirNStart::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, nextStatus, moduleAccesser, true)) {
    case 0x117:
    case 0x11C:
        break;
    default: {
        moduleAccesser->getWorkManageModule().setInt(0, 0x10000041);
        int articleId;
        if (moduleAccesser->getStageObject().soGetSubKind() == Fighter_Kirby) {
            articleId = 13;
        } else {
            articleId = static_cast<ftSamusData*>(g_ftCommonDataAccesser.getData(Fighter_Samus))->chargeShotArticleId;
        }
        moduleAccesser->getGenerateArticleManageModule().removeExist(articleId, 0);
        moduleAccesser->getEffectModule().removeCommon(0x1A);
        break;
    }
    }
}
// HYPOTHESIS: the charge shot parameter block of Samus (and of Kirby's copy): the number of charge steps and the
// frames per step.
struct ftSamusParamChargeShot {
    int maxCharge;
    int unk04;
    float stepFrames;
};

inline soMotionChangeParam::soMotionChangeParam(int kind, float frame, float rate, u8 a, u8 b, bool c, u8 d) :
    m_kind(kind), m_frame(frame), m_rate(rate), _12(a), _13(b), _14(c), _15(d) {}

// Charging: one step every stepFrames frames up to maxCharge (then the fully charged status on the ground); on the
// ground the attack button fires and the shield button stores the charge.
void ftSamusStatusUniqProcessSpecialNHold::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    ftKind kind = static_cast<ftKind>(moduleAccesser->getStageObject().soGetSubKind());
    ftSamusParamChargeShot* param = static_cast<ftSamusParamChargeShot*>(g_ftCommonDataAccesser.getParamSamusChargeShot(kind));
    soStatusModule& status = moduleAccesser->getStatusModule();
    int situation = moduleAccesser->getSituationModule().getKind();
    moduleAccesser->getWorkManageModule().incInt(0x10000042);
    if (static_cast<float>(moduleAccesser->getWorkManageModule().getInt(0x10000042)) > param->stepFrames) {
        moduleAccesser->getWorkManageModule().setInt(0, 0x10000042);
        moduleAccesser->getWorkManageModule().incInt(0x10000041);
        int maxCharge = param->maxCharge;
        if (moduleAccesser->getWorkManageModule().getInt(0x10000041) >= maxCharge) {
            moduleAccesser->getWorkManageModule().setInt(maxCharge, 0x10000041);
            if (situation == 0) {
                status.changeStatusRequest(g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(0, 0x119, moduleAccesser, false), moduleAccesser);
                return;
            }
        }
    }
    if (situation == 0) {
        // HYPOTHESIS: a checked cast whose result the original does not use (the buttons are read through the base module).
        dynamic_cast<ftControllerModuleImpl&>(moduleAccesser->getControllerModule());
        if (moduleAccesser->getControllerModule().getTrigger() & soController::getButtonMask(static_cast<soController::PadButton>(1))) {
            moduleAccesser->getMotionModule().changeMotionRequest(&soMotionChangeParam(0x1ce, 0.0f, 1.0f, 0, 0, 0, 0));
            status.changeStatusRequest(g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(0, 0x11a, moduleAccesser, false), moduleAccesser);
        } else if (moduleAccesser->getControllerModule().getTrigger() & soController::getButtonMask(static_cast<soController::PadButton>(3))) {
            status.changeStatusRequest(0x1a, moduleAccesser);
        }
    }
}
void ftSamusStatusUniqProcessSpecialNHold::execFixPos(soModuleAccesser* moduleAccesser) {
    ftKind kind = static_cast<ftKind>(moduleAccesser->getStageObject().soGetSubKind());
    int* param = static_cast<int*>(g_ftCommonDataAccesser.getParamSamusChargeShot(kind));
    moduleAccesser->getSituationModule().getKind();
    int articleId;
    if (kind == Fighter_Kirby) {
        articleId = 13;
    } else {
        articleId = static_cast<ftSamusData*>(g_ftCommonDataAccesser.getData(Fighter_Samus))->chargeShotArticleId;
    }
    soArticle* article = moduleAccesser->getGenerateArticleManageModule().getArticle(articleId);
    if (article != NULL) {
        wnSamusCShot* shot = dynamic_cast<wnSamusCShot*>(article);
        shot->setCharge(static_cast<float>(moduleAccesser->getWorkManageModule().getInt(0x10000041)) / *param);
    }
}
void ftSamusStatusUniqProcessSpecialNHold::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, nextStatus, moduleAccesser, true)) {
    case 0x1A:
    case 0x1E:
    case 0x1F:
    case 0x20:
    case 0x119: {
        int articleId;
        if (moduleAccesser->getStageObject().soGetSubKind() == Fighter_Kirby) {
            articleId = 13;
        } else {
            articleId = static_cast<ftSamusData*>(g_ftCommonDataAccesser.getData(Fighter_Samus))->chargeShotArticleId;
        }
        moduleAccesser->getGenerateArticleManageModule().removeExist(articleId, 0);
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        int limit = moduleAccesser->getConstantIntKirby(0x5DC0);
        if (work.getInt(0x10000041) >= limit) {
            moduleAccesser->getEffectModule().reqCommon(0x1A, 0.0f);
        }
        break;
    }
    case 0x11A:
    case 0x11C:
        break;
    default: {
        moduleAccesser->getWorkManageModule().setInt(0, 0x10000041);
        int articleId;
        if (moduleAccesser->getStageObject().soGetSubKind() == Fighter_Kirby) {
            articleId = 13;
        } else {
            articleId = static_cast<ftSamusData*>(g_ftCommonDataAccesser.getData(Fighter_Samus))->chargeShotArticleId;
        }
        moduleAccesser->getGenerateArticleManageModule().removeExist(articleId, 0);
        moduleAccesser->getEffectModule().removeCommon(0x1A);
        break;
    }
    }
}
// HYPOTHESIS: unless it follows status 0x11C, the charge in work int 0x10000041 moves to 0x10000042 and is cleared.
void ftSamusStatusUniqProcessSpecialNFire::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getPrevStatusKind(0) != 0x11C) {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        work.setInt(work.getInt(0x10000041), 0x10000042);
        moduleAccesser->getWorkManageModule().setInt(0, 0x10000041);
    }
    moduleAccesser->getEffectModule().removeCommon(0x1A);
}
void ftSamusStatusUniqProcessSpecialNFire::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000012);
    }
}
void ftSamusStatusUniqProcessSpecialNFire::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, nextStatus, moduleAccesser, true) != 0x11C) {
        moduleAccesser->getWorkManageModule().setInt(0, 0x10000041);
        int articleId;
        if (moduleAccesser->getStageObject().soGetSubKind() == Fighter_Kirby) {
            articleId = 13;
        } else {
            articleId = static_cast<ftSamusData*>(g_ftCommonDataAccesser.getData(Fighter_Samus))->chargeShotArticleId;
        }
        moduleAccesser->getGenerateArticleManageModule().removeExist(articleId, 0);
        moduleAccesser->getEffectModule().removeCommon(0x1A);
    }
}
void ftSamusStatusUniqProcessSpecialAirNFire::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getPrevStatusKind(0) != 0x11A) {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        work.setInt(work.getInt(0x10000041), 0x10000042);
        moduleAccesser->getWorkManageModule().setInt(0, 0x10000041);
    }
    moduleAccesser->getEffectModule().removeCommon(0x1A);
}
void ftSamusStatusUniqProcessSpecialAirNFire::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
        ftKind kind = static_cast<ftKind>(moduleAccesser->getStageObject().soGetSubKind());
        float* param = static_cast<float*>(g_ftCommonDataAccesser.getParamSamusChargeShot(kind));
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        float lr = moduleAccesser->getPostureModule().getLr();
        float recoil = param[1] * work.getInt(0x10000042);
        float speedX = recoil * lr;
        stop.m_speed = Vec2f(speedX, stop.getSpeed().m_y);
        moduleAccesser->getWorkManageModule().offFlag(0x22000012);
    }
}
void ftSamusStatusUniqProcessSpecialAirNFire::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, nextStatus, moduleAccesser, true) != 0x11A) {
        moduleAccesser->getWorkManageModule().setInt(0, 0x10000041);
        int articleId;
        if (moduleAccesser->getStageObject().soGetSubKind() == Fighter_Kirby) {
            articleId = 13;
        } else {
            articleId = static_cast<ftSamusData*>(g_ftCommonDataAccesser.getData(Fighter_Samus))->chargeShotArticleId;
        }
        moduleAccesser->getGenerateArticleManageModule().removeExist(articleId, 0);
        moduleAccesser->getEffectModule().removeCommon(0x1A);
    }
}
