#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define WN_LASSO_RUBBER_SLOTS
#include <so/status/so_status_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>
#include <wn/weapon.h>
#include <wn/wn_lasso_status_view.h>

class wnPopoRubberStatusUniqProcessCommon : public soStatusUniqProcess {
public:
    virtual ~wnPopoRubberStatusUniqProcessCommon() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnPopoRubberStatusUniqProcessCommon g_wnPopoRubberStatusUniqProcessCommon;

void wnPopoRubberStatusUniqProcessCommon::initStatus(soModuleAccesser* moduleAccesser) {
    wnLassoGetAltStatus(moduleAccesser)->unk4CInt(1);
    if (moduleAccesser->getStatusModule().getStatusKind() == 1) {
        wnLassoGetAltStatus(moduleAccesser)->unk64Int(0);
        wnLassoGetAltStatus(moduleAccesser)->unk68VecPair(&Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(5.0f, 0.0f, 0.0f));
    } else if (moduleAccesser->getStatusModule().getStatusKind() == 2) {
        wnLassoGetAltStatus(moduleAccesser)->unk64Int(1);
    } else if (moduleAccesser->getStatusModule().getStatusKind() == 3) {
        wnLassoGetAltStatus(moduleAccesser)->unk64Int(2);
    }
}

void wnPopoRubberStatusUniqProcessCommon::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getStatusKind() == 4) {
        if (wnLassoGetAltStatus(moduleAccesser)->unk88Int() <= 0) {
            dynamic_cast<Weapon*>(&moduleAccesser->getStageObject())->deactivateArticle();
            return;
        }
        wnLassoGetAltStatus(moduleAccesser)->unk90Int(1);
    }
    Vec3f pos = moduleAccesser->getLinkModule().getModelConstraintTargetNodePosition();
    if (moduleAccesser->getLinkModule().isLink(5)) {
        Vec3f node = moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(5, 0x27, false);
        wnLassoGetAltStatus(moduleAccesser)->unk68VecPair(&pos, &node);
    }
}
