#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/article/so_generate_article_manage_module.h>
#include <types.h>

class ftSheikStatusUniqProcessSpecialNShoot : public soStatusUniqProcess {
public:
    virtual ~ftSheikStatusUniqProcessSpecialNShoot() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftSheikStatusUniqProcessSpecialNShoot g_ftSheikStatusUniqProcessSpecialNShoot;

// Fires on every third frame of the shot counter, then advances the counter.
void ftSheikStatusUniqProcessSpecialNShoot::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (work.getInt(0x20000000) % 3 == 2) {
        // HYPOTHESIS: flag 0x22000011 is the per-shot flag.
        work.onFlag(0x22000011);
    }
    work.incInt(0x20000000);
}

// Fires the shot when the counter allows it, on the Sheik or Pikachu-style special-N states (0xE / 0x5).
void ftSheikStatusUniqProcessSpecialNShoot::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getPrevStatusKind(0) != 0) {
        return;
    }
    int status = moduleAccesser->getStatusModule().getStatusKind();
    if (status != 0xe && status != 5) {
        return;
    }
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (work.isFlag(0x22000011)) {
        work.offFlag(0x22000011);
        if (work.getInt(0x10000040) > 0) {
            int articleId = (status == 0xe) ? 0 : 0xd;
            soGenerateArticleManageModule* articles = static_cast<soGenerateArticleManageModule*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule);
            articles->shoot(articleId, 0, false);
            moduleAccesser->getWorkManageModule().decInt(0x10000040);
        }
    }
}

void ftSheikStatusUniqProcessSpecialNShoot::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status == 0x10) {
        float value = moduleAccesser->getConstantFloatKirby(0xFA4);
        // HYPOTHESIS: threshold from lbl_27_rodata_7738.
        if (value > 0.0f) {
            moduleAccesser->getWorkManageModule().setFloat(value, 0x11000000);
        }
    }
    moduleAccesser->getWorkManageModule().setInt(0, 0x10000040);
}

bool ftSheikStatusUniqProcessSpecialNShoot::checkDamage(soModuleAccesser* moduleAccesser, void*) { return false; }
