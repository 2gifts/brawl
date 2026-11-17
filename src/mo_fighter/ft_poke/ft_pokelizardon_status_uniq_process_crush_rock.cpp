#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE // BYTES: the original unit holds out-of-line copies of the base defaults, see below
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/article/so_generate_article_manage_module.h>
#include <types.h>

// Lizardon's Crush Rock: the rock article is kept while the next status is one of the listed kinds, and it is
// removed (or shot off when it is the only active one) on exit.
// HYPOTHESIS: work flag 0x22000012 marks the rock as held and 0x22000013 marks the rock as launched.
class ftPokeLizardonStatusUniqProcessCrushRock : public soStatusUniqProcess {
public:
    virtual ~ftPokeLizardonStatusUniqProcessCrushRock();
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftPokeLizardonStatusUniqProcessCrushRock::initStatus(soModuleAccesser*) {}

void ftPokeLizardonStatusUniqProcessCrushRock::execStatus(soModuleAccesser*) {}

static inline soGenerateArticleManageModule& getArticles(soModuleAccesser* moduleAccesser) {
    return *static_cast<soGenerateArticleManageModule*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule);
}

void ftPokeLizardonStatusUniqProcessCrushRock::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    bool keepRock = false;
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000012) == true) {
        if (getArticles(moduleAccesser).isExist(1) == true) {
            switch (nextStatus) {
            case 0x43:
            case 0x44:
            case 0x45:
            case 0x46:
            case 0x56:
            case 0x5c:
            case 0x6e:
            case 0x70:
            case 0x89:
            case 0x8a:
                keepRock = true;
                break;
            default:
                break;
            }
        }
    }
    if (keepRock == true && getArticles(moduleAccesser).getActiveNum(1) < 2) {
        moduleAccesser->getWorkManageModule().onFlag(0x22000013);
        getArticles(moduleAccesser).shoot(1, 0, true);
    } else {
        getArticles(moduleAccesser).removeExist(1, 0);
    }
}

// BYTES: the base-class defaults sit in this unit in this order, and the destructor between execStop and exitStatus is
// defined out of line (an in-class destructor would put exitStatus after the constructor, past the static initializer).
bool soStatusUniqProcess::checkTransitionPrecede(soModuleAccesser*, void*, int) { return true; }
void soStatusUniqProcess::leaveStop(soModuleAccesser*, int, bool) {}
bool soStatusUniqProcess::onChangeLr(soModuleAccesser*, float, float) { return false; }
void soStatusUniqProcess::checkAttack(soModuleAccesser*, void*, float) {}
bool soStatusUniqProcess::checkDamage(soModuleAccesser*, void*) { return false; }
void soStatusUniqProcess::execFixCamera(soModuleAccesser*) {}
void soStatusUniqProcess::execFixPos(soModuleAccesser*) {}
void soStatusUniqProcess::execFixPosCounter(soModuleAccesser*) {}
void soStatusUniqProcess::execMapCorrection(soModuleAccesser*) {}
void soStatusUniqProcess::execStop(soModuleAccesser*) {}
ftPokeLizardonStatusUniqProcessCrushRock::~ftPokeLizardonStatusUniqProcessCrushRock() {}
void soStatusUniqProcess::exitStatus(soModuleAccesser*, int) {}

ftPokeLizardonStatusUniqProcessCrushRock g_ftPokeLizardonStatusUniqProcessCrushRock;

