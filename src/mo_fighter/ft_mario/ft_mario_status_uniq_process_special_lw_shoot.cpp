// HYPOTHESIS: the Pump charge ratio and speed scale are read from work 0x10000040 and parameter ids 0x5dc0 / 0xfae.
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <wn/weapon.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/article/so_generate_article_manage_module.h>
#include <types.h>

// HYPOTHESIS: the Pump article (wnMarioPump__setCharge in the module map); only setCharge is used here.
class wnMarioPump : public Weapon {
public:
    void setCharge(float charge);
};

class ftMarioStatusUniqProcessSpecialLwShoot : public soStatusUniqProcess {
public:
    virtual ~ftMarioStatusUniqProcessSpecialLwShoot() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

void ftMarioStatusUniqProcessSpecialLwShoot::initStatus(soModuleAccesser* moduleAccesser) {
    soGenerateArticleManageModule* articles = static_cast<soGenerateArticleManageModule*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule);
    wnMarioPump* pump = dynamic_cast<wnMarioPump*>(articles->getArticle(2));
    // HYPOTHESIS: work id 0x10000040 holds the charge count; 0x5dc0 / 0xfae are parameter ids.
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    float chargeMax = (float)soValueAccesser::getConstantInt(moduleAccesser, 0x5dc0, 0);
    pump->setCharge((float)work.getInt(0x10000040) / chargeMax);
    soWorkManageModule& speedWork = moduleAccesser->getWorkManageModule();
    float speedScale = soValueAccesser::getConstantFloat(moduleAccesser, 0xfae, 0);
    float charge = (float)speedWork.getInt(0x10000040);
    Vec3f speed(-(charge * speedScale), 0.0f, 0.0f);
    moduleAccesser->getKineticModule().addSpeed(&speed, moduleAccesser);
}

ftMarioStatusUniqProcessSpecialLwShoot g_ftMarioStatusUniqProcessSpecialLwShoot;
