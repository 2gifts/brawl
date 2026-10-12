#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <wn/wn_lasso_status_view.h>
#include <types.h>

// HYPOTHESIS: the iron wire article shares the lasso status view at module enumeration +0x9C (same slots).
class wnSheikIronWireStatusUniqProcessTakeup : public soStatusUniqProcess {
public:
    virtual ~wnSheikIronWireStatusUniqProcessTakeup() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnSheikIronWireStatusUniqProcessTakeup g_wnSheikIronWireStatusUniqProcessTakeup;

void wnSheikIronWireStatusUniqProcessTakeup::initStatus(soModuleAccesser* moduleAccesser) {
    wnLassoGetAltStatus(moduleAccesser)->unkA4Int(5);
}

void wnSheikIronWireStatusUniqProcessTakeup::execStatus(soModuleAccesser* moduleAccesser) {
    if (wnLassoGetAltStatus(moduleAccesser)->unk88Int() > 0) {
        wnLassoGetAltStatus(moduleAccesser)->unkB4Int(1);
        if (wnLassoGetAltStatus(moduleAccesser)->unk88Int() == 0) {
            wnLassoGetAltStatus(moduleAccesser)->unkA4Int(6);
        }
    }
}
