#include <mt/mt_prng.h>
#include <snd/snd_system.h>
#include <so/article/so_article.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

void ftKineticEnergyDisableAndClear(int, soModuleAccesser*);

// HYPOTHESIS: roles of the numeric work and parameter IDs are inferred from
// their use here; the original declarations have not been recovered.
class ftDededeStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int status);

    void updateRecall(soModuleAccesser* moduleAccesser);
};

void ftDededeStatusUniqProcessFinal::initStatus(soModuleAccesser* moduleAccesser) {
    ftKineticEnergyDisableAndClear(3, moduleAccesser);
    ftKineticEnergyDisableAndClear(1, moduleAccesser);
    ftKineticEnergyDisableAndClear(2, moduleAccesser);
    ftKineticEnergyDisableAndClear(4, moduleAccesser);
    ftKineticEnergyDisableAndClear(0, moduleAccesser);

    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x116:
        moduleAccesser->getCollisionHitModule().setXluGlobal(0);
        moduleAccesser->getWorkManageModule().setInt(-1, 0x20000007);
        break;
    case 0x138: {
        int timerValue = soValueAccesser::getConstantInt(moduleAccesser, 0x5DCC, 0) * 60;
        moduleAccesser->getWorkManageModule().setInt(timerValue, 0x20000002);
        if (moduleAccesser->getPostureModule().getLr() == -1.0f) {
            Vec3f oldRotation = moduleAccesser->getModelModule().getNodeRotate(7);
            Vec3f rotation(oldRotation.m_x, 180.0f, oldRotation.m_z);
            moduleAccesser->getModelModule().setNodeRotate(7, &rotation);
        }
        break;
    }
    }
}

void ftDededeStatusUniqProcessFinal::execStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x116:
        break;
    case 0x138:
        if (moduleAccesser->getPostureModule().getLr() == -1.0f) {
            Vec3f oldRotation = moduleAccesser->getModelModule().getNodeRotate(7);
            Vec3f rotation(oldRotation.m_x, 180.0f, oldRotation.m_z);
            moduleAccesser->getModelModule().setNodeRotate(7, &rotation);
        }
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
            updateRecall(moduleAccesser);
        }
        break;
    }
}

void ftDededeStatusUniqProcessFinal::execStop(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x138 &&
        moduleAccesser->getPostureModule().getLr() == -1.0f) {
        Vec3f oldRotation = moduleAccesser->getModelModule().getNodeRotate(7);
        Vec3f rotation(oldRotation.m_x, 180.0f, oldRotation.m_z);
        moduleAccesser->getModelModule().setNodeRotate(7, &rotation);
    }
}

void ftDededeStatusUniqProcessFinal::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessFinal::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    switch (status) {
    case 0x138:
        return;
    case 0x116:
        break;
    case 0x139: {
        Vec3f oldRotation = moduleAccesser->getModelModule().getNodeRotate(7);
        Vec3f rotation(oldRotation.m_x, 0.0f, oldRotation.m_z);
        moduleAccesser->getModelModule().setNodeRotate(7, &rotation);
        break;
    }
    default:
        if (moduleAccesser->getWorkManageModule().getInt(0x20000007) >= 0) {
            g_sndSystem->finalOffState(0x1B49);
        }
        moduleAccesser->getEffectModule().removeCommon(0x26);
        moduleAccesser->getEffectModule().removeCommon(0x25);
        moduleAccesser->getCollisionHitModule().cancelXluGlobal(0);
        break;
    }
}

void ftDededeStatusUniqProcessFinal::updateRecall(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().getInt(0x20000004) > 0) {
        moduleAccesser->getWorkManageModule().subInt(1, 0x20000004);
    }
    if (moduleAccesser->getWorkManageModule().getInt(0x20000004) != 0) {
        return;
    }

    const soModuleEnumeration* enumeration = moduleAccesser->m_enumerationStart;
    soGenerateArticleManageModule& articleManager =
        *static_cast<soGenerateArticleManageModule*>(enumeration->m_generateArticleManageModule);
    int recallCount = enumeration->m_workManageModule->getInt(0x10000042);
    if (!articleManager.isGeneratable(4)) {
        return;
    }
    if (recallCount >= soValueAccesser::getConstantInt(moduleAccesser, 0x5DCB, 0)) {
        return;
    }

    moduleAccesser->getWorkManageModule().onFlag(0x1200003D);
    soWorkManageModule& periodWork = moduleAccesser->getWorkManageModule();
    int variantPeriod = soValueAccesser::getConstantInt(moduleAccesser, 0x5DCA, 0);
    if (periodWork.getInt(0x20000005) >= variantPeriod) {
        moduleAccesser->getWorkManageModule().onFlag(0x22000012);
        moduleAccesser->getWorkManageModule().setInt(0, 0x20000005);
    } else {
        moduleAccesser->getWorkManageModule().addInt(1, 0x20000005);
    }

    float firstBand = soValueAccesser::getConstantFloat(moduleAccesser, 0xFC2, 0);
    float secondBand = soValueAccesser::getConstantFloat(moduleAccesser, 0xFC3, 0);
    float finalBand = soValueAccesser::getConstantFloat(moduleAccesser, 0xFC4, 0);
    float totalWeight = finalBand + (firstBand + secondBand);
    float randomValue = totalWeight * randf();

    int variant;
    if (randomValue < firstBand) {
        variant = 0;
    } else if (randomValue < firstBand + secondBand) {
        variant = 1;
    } else {
        variant = 2;
    }
    moduleAccesser->getWorkManageModule().setInt(variant, 0x20000006);

    soArticle* article = articleManager.generate(4, NULL, NULL);
    if (!article->isNull()) {
        articleManager.entry(article);
    }

    moduleAccesser->getWorkManageModule().addInt(1, 0x10000042);
    moduleAccesser->getWorkManageModule().offFlag(0x22000012);
    soWorkManageModule& delayWork = moduleAccesser->getWorkManageModule();
    int recallDelay = soValueAccesser::getConstantInt(moduleAccesser, 0x5DC9, 0);
    delayWork.setInt(recallDelay, 0x20000004);
}

ftDededeStatusUniqProcessFinal g_ftDededeStatusUniqProcessFinal;
