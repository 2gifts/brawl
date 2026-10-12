#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Local view of soGenerateArticleManageModule with the two-argument removal at vtable slot 0x48 (see the Mario
// special-S unit for the same layout).
class ftNessArticleManageView {
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

class ftNessStatusUniqProcessSpecialHi : public soStatusUniqProcess {
public:
    virtual ~ftNessStatusUniqProcessSpecialHi() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftNessStatusUniqProcessSpecialHi::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    // The common data lookup result is not used by this status.
    g_ftCommonDataAccesser.getData(Fighter_Ness);
    // HYPOTHESIS: work id 0x20000003 is a one-shot int set on entry.
    work.setInt(1, 0x20000003);
}

void ftNessStatusUniqProcessSpecialHi::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    // TODO: not reconstructed yet.
}

void ftNessStatusUniqProcessSpecialHi::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    soLinkModule& link = moduleAccesser->getLinkModule();
    if (link.isLink(6)) {
        link.unlink(6);
    }
    ftNessArticleManageView* articles = reinterpret_cast<ftNessArticleManageView*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule);
    articles->unk48(2, 0);
    if (nextStatus == 0x10) {
        g_ftCommonDataAccesser.getData(Fighter_Ness);
    }
}

ftNessStatusUniqProcessSpecialHi g_ftNessStatusUniqProcessSpecialHi;
