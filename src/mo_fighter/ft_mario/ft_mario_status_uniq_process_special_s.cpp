#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Local view of soGenerateArticleManageModule: the BrawlHeaders copy does not place the two-argument removal at
// vtable slot 0x48. The dummies keep the 16-entry prefix (vtable offset = 8 + 4 * index).
class ftMarioArticleManageView {
public:
    virtual void slot00();
    virtual void slot01();
    virtual void slot02();
    virtual void slot03();
    virtual void slot04();
    virtual void slot05();
    virtual void slot06();
    virtual void slot07();
    virtual void slot08();
    virtual void slot09();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void unk48(int, int);   // name unknown: two-argument call through slot 0x48
};

class ftMarioStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftMarioStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftMarioStatusUniqProcessSpecialS::initStatus(soModuleAccesser* moduleAccesser) {
    // TODO: the air-start energy setup (kinetic energies 2/3, Vec2f reset) is not reconstructed yet.
}

void ftMarioStatusUniqProcessSpecialS::execStatus(soModuleAccesser* moduleAccesser) {
    // TODO: not reconstructed yet.
}

void ftMarioStatusUniqProcessSpecialS::exitStatus(soModuleAccesser* moduleAccesser, int) {
    ftMarioArticleManageView* articles = reinterpret_cast<ftMarioArticleManageView*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule);
    articles->unk48(0, 0);
}

ftMarioStatusUniqProcessSpecialS g_ftMarioStatusUniqProcessSpecialS;
