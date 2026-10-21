#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_energy.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class soArticle;

// Local view of the article manage module (module slot 0x84). Two vtable words precede the first declared slot here.
// HYPOTHESIS: slot order follows soGenerateArticleManageModule in the header after those leading slots.
class ftSheikArticleManageView {
public:
    virtual void reserved();
    virtual void activate();
    virtual void deactivate();
    virtual soArticle* generate(int articleId, bool, bool);
    virtual void have(int articleId, int, int selector, int taskId);
    virtual void shoot(int articleId, int selector, bool);
    virtual void changeMotion(int articleId, int motionKind, bool);
    virtual void setFrame(int articleId, float);
    virtual void setRate(int articleId, float);
    virtual void changeStatus(int articleId, int statusKind, int selector);
    virtual void setVisibilityWhole(int articleId, u8, int selector);
    virtual void set2nd(int articleId, u8, int selector);
    virtual void setSituationKind(int articleId, int situationKind);
    virtual void entry(soArticle* article);
    virtual void eject(int taskId);
    virtual void remove(int articleId, int selector);
    virtual void removeExist(int articleId, int selector);
    virtual void removeExistTaskId(int taskId);
    virtual bool isExist(int articleId);
};

// Local view of an article object; the first declared slot is a boolean query.
class ftSheikArticleView {
public:
    virtual bool query();
};

class ftSheikStatusUniqProcessSpecialSStart : public soStatusUniqProcess {
public:
    virtual ~ftSheikStatusUniqProcessSpecialSStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftSheikStatusUniqProcessSpecialSStart::initStatus(soModuleAccesser* moduleAccesser) {
    soSituationModule& situation = moduleAccesser->getSituationModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    if (situation.getKind() == 0) {
        kinetic.changeKinetic(0xf, moduleAccesser);
    } else {
        kinetic.changeKinetic(0x64, moduleAccesser);
        ftKineticEnergyGravity& energy1 = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
        energy1.disable();
    }
}

// HYPOTHESIS: not reconstructed yet (paired-single position math and effect/article calls); left as a stub.
void ftSheikStatusUniqProcessSpecialSStart::execFixPosCounter(soModuleAccesser*) {
}

void ftSheikStatusUniqProcessSpecialSStart::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    ftSheikArticleManageView* articles = reinterpret_cast<ftSheikArticleManageView*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule);
    if (nextStatus == 0x80) {
        if (!articles->isExist(2)) {
            soArticle* article = articles->generate(2, false, false);
            if (!reinterpret_cast<ftSheikArticleView*>(article)->query()) {
                articles->entry(article);
            }
        }
    } else if (nextStatus != 0x11a && nextStatus != 0x81) {
        articles->removeExist(2, 0);
    }
}

ftSheikStatusUniqProcessSpecialSStart g_ftSheikStatusUniqProcessSpecialSStart;
