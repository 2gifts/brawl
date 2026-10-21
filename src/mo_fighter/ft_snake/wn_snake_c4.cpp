#define FT_SNAKE_ARTICLE_POOLS
#include <types.h>
#include <ft/builder/ft_dol_array_list.h>
#include <ft/snake/ft_snake_builder.h>
#include <so/so_module_accesser.h>
#include <so/link/so_link_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {
void fn_122_1AF04(soModuleAccesser*);
} // extern "C"

void wnSnakeC4::shoot() {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 1:
        if (m_moduleAccesser->getLinkModule().isLink(3) == 1) {
            if (static_cast<u32>(m_moduleAccesser->getLinkModule().getParentTaskId(3)) == m_moduleAccesser->getWorkManageModule().getInt(0x10000000)) {
                m_moduleAccesser->getWorkManageModule().onFlag(0x22000002);
            }
        }
        break;
    case 2:
        if (m_moduleAccesser->getLinkModule().isLink(3) == 1) {
            if (static_cast<u32>(m_moduleAccesser->getLinkModule().getParentTaskId(3)) == m_moduleAccesser->getWorkManageModule().getInt(0x10000000)) {
                m_moduleAccesser->getWorkManageModule().onFlag(0x22000002);
            }
        }
        break;
    case 4:
    case 5:
        break;
    default:
        if (m_moduleAccesser->getLinkModule().isModelConstraint() == 1) {
            fn_122_1AF04(m_moduleAccesser);
        }
        m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
        break;
    }
}
