#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Local view of the article manage module (module slot 0x84). Two vtable words precede the first declared slot here.
// HYPOTHESIS: slot order follows soGenerateArticleManageModule in the header after those leading slots.
class ftSheikArticleManageView {
public:
    virtual void reserved();
    virtual void activate();
    virtual void deactivate();
    virtual void generate(int articleId, soModuleAccesser* moduleAccesser, void* logAttackInfo);
    virtual void have(int articleId, int, int selector, int taskId);
    virtual void shoot(int articleId, int selector, bool);
    virtual void changeMotion(int articleId, int motionKind, bool);
    virtual void setFrame(int articleId, float);
    virtual void setRate(int articleId, float);
    virtual void changeStatus(int articleId, int statusKind, int selector);
    virtual void setVisibilityWhole(int articleId, u8, int selector);
    virtual void set2nd(int articleId, u8, int selector);
    virtual void setSituationKind(int articleId, int situationKind);
    virtual void entry(void* article);
    virtual void eject(int taskId);
    virtual void remove(int articleId, int selector);
    virtual void removeExist(int articleId, int selector);
};

// Local view of the visibility module; its vtable has two leading words before the first declared slot.
class ftSheikVisibilityModuleView {
public:
    virtual void activate();
    virtual void setData(void*);
    virtual void set(u32, int);
    virtual void setStatusDefault(u32, u8, bool);
    virtual void setDefaultAll();
    virtual void update();
    virtual void hideAll();
    virtual void setWhole(u8);
};

class ftSheikStatusUniqProcessSpecialSLoop : public soStatusUniqProcess {
public:
    virtual ~ftSheikStatusUniqProcessSpecialSLoop() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftSheikStatusUniqProcessSpecialSLoop::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: extendParam[1] of the Sheik data points to an int table; entry 1 seeds the work int 0x20000004.
    ftData* data = g_ftCommonDataAccesser.getData(Fighter_Sheik);
    int* table = static_cast<int*>(data->extendParam[1]);
    moduleAccesser->getWorkManageModule().setInt(table[1], 0x20000004);
}

// HYPOTHESIS: not reconstructed yet (per-frame loop and kinetic/effect updates); left as a stub.
void ftSheikStatusUniqProcessSpecialSLoop::execStatus(soModuleAccesser*) {
}

// HYPOTHESIS: not reconstructed yet (vector math and article/effect updates); left as a stub.
void ftSheikStatusUniqProcessSpecialSLoop::execFixPosCounter(soModuleAccesser*) {
}

void ftSheikStatusUniqProcessSpecialSLoop::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x11b && nextStatus != 0x81) {
        reinterpret_cast<ftSheikArticleManageView*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule)->removeExist(2, 0);
    }
}

ftSheikStatusUniqProcessSpecialSLoop g_ftSheikStatusUniqProcessSpecialSLoop;
