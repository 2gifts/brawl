#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/pikmin/wn_pikmin_chain.h>
#include <wn/wn_lasso_status_view.h>

class wnPikminChainVisibleEvent : public soLinkEventArgs {
public:
    int m_count;
    wnPikminChainVisibleEvent(int eventKind, int count) : soLinkEventArgs(eventKind), m_count(count) { }
};

void wnPikminChainUtility::updatePikminVisibility(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: the lasso view reports how many chain links are out; work int 0x10000008 remembers how many Pikmin are shown
    // and 0x10000007 is the number of Pikmin the chain carries.
    int visible = wnLassoGetAltStatus(moduleAccesser)->unk88Int();
    if (visible != moduleAccesser->getWorkManageModule().getInt(0x10000008)) {
        int pikminNum = moduleAccesser->getWorkManageModule().getInt(0x10000007);
        if (visible > pikminNum) {
            visible = pikminNum;
        }
        wnPikminChainVisibleEvent event(0x83f, visible);
        moduleAccesser->getLinkModule().sendEventParents(3, event);
        moduleAccesser->getWorkManageModule().setInt(visible, 0x10000008);
    }
}
