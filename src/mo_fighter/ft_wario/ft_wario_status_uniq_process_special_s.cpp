#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/wario/ft_wario_status_uniq_process_special_s.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>

#include <so/article/so_generate_article_manage_module.h>
#include <so/so_external_value_accesser.h>
#include <it/item.h>
// TODO(W1): all functions are exact, the weak base-process defaults are in the original order, and the class tables match,
// but the unit cannot be linked yet: the original .data starts at 0x640C (4 mod 8) while mwcc gives this object an
// 8-aligned .data, so the link inserts 4 bytes and shifts everything after it (dtk warns "expected 8, but starts at 4"
// for this split). The base initStatus default also lands after the constructor instead of right after the destructor.
void ftWarioStatusUniqProcessSpecialS::initStatus(soModuleAccesser* a) {
    // BYTES: keep the native declaration order for module registers.
    const soModuleEnumeration* modules = a->m_enumerationStart;
    soGenerateArticleManageModule* articles = static_cast<soGenerateArticleManageModule*>(modules->m_generateArticleManageModule);
    soItemManageModule* items = modules->m_itemManageModule;
    soWorkManageModule* work = modules->m_workManageModule;
    soStatusModule* status = modules->m_statusModule;
    soLinkModule* link = modules->m_linkModule;
    StageObject* const fighter = &a->getStageObject();
    ftWarioBikeRiderParam* param;
    if (fighter->soGetSubKind() == 0x15) {
        ftWario& wario = dynamic_cast<ftWario&>(*fighter);
        param = wario.getExtendParam()->bikeRider;
    } else {
        ftWarioMan& wario = dynamic_cast<ftWarioMan&>(*fighter);
        param = wario.getExtendParam()->bikeRider;
    }
    if (items->getPickableItemKind() == 0x5c) {
        // BYTES: declare the link before the temporary item payload.
        soLinkModule* itemLink;
        soItemInfo item;
        items->getPickableItemInfo(&item);
        BaseItem* bikeItem = item.m_item;
        itemLink = soExternalValueAccesser::getLinkModule(static_cast<StageObject*>(bikeItem));
        if (itemLink->isLinked(7)) {
            work->setInt(itemLink->getNodeTaskId(7), 0x10000044);
            ftWarioBikeTaskEvent event(0x45c, fighter->m_taskId);
            itemLink->sendEventNodes(7, event, 0);
        }
        items->removeItem(bikeItem);
        status->changeStatusRequest(0x123, a);
    } else if (link->isLink(7)) {
        status->changeStatusRequest(0x122, a);
    } else if (work->getInt(0x10000043) < param->remountCooldownFrames) {
        status->changeStatusRequest(0x122, a);
    } else if (articles->isExist(0) == true) {
        status->changeStatusRequest(0x122, a);
    } else {
        work->setInt(0, 0x10000043);
        work->setInt(fighter->m_taskId, 0x10000044);
        status->changeStatusRequest(0x121, a);
    }
    work->offFlag(0x12000041);
}
// Shared virtual defaults are emitted in their original function order; initStatus follows the destructor.
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
void soStatusUniqProcess::execStatus(soModuleAccesser*) {}
void soStatusUniqProcess::exitStatus(soModuleAccesser*, int) {}

ftWarioStatusUniqProcessSpecialS g_ftWarioStatusUniqProcessSpecialS;

inline void soStatusUniqProcess::initStatus(soModuleAccesser*) {}
