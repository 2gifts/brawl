#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <ft/samus/ft_samus_data.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/samus/wn_samus_cshot.h>

// Class shapes from the module map; the empty method bodies are not reconstructed yet.
class ftSamusStatusUniqProcessSpecialNStart : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessSpecialNStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

class ftSamusStatusUniqProcessSpecialAirNStart : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessSpecialAirNStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

class ftSamusStatusUniqProcessSpecialNHold : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessSpecialNHold() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

class ftSamusStatusUniqProcessSpecialNFire : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessSpecialNFire() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftSamusStatusUniqProcessSpecialAirNFire : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessSpecialAirNFire() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

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
void ftSamusStatusUniqProcessSpecialNHold::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
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
