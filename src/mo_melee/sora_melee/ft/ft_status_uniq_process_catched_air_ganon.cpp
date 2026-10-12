#include <ft/ft_status_uniq_process_catched_air_ganon.h>
#include <ft/ft_status_uniq_process_views.h>
#include <so/so_module_accesser.h>
#include <types.h>

// Globals are registered in this order by the static initializer.
ftStatusUniqProcessCatchedAirGanon g_ftStatusUniqProcessCatchedAirGanon;
ftStatusUniqProcessCatchedAirFallGanon g_ftStatusUniqProcessCatchedAirFallGanon;
ftStatusUniqProcessCatchedAirEndGanon g_ftStatusUniqProcessCatchedAirEndGanon;

void ftStatusUniqProcessCatchedAirGanon::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: not yet matched.
}

void ftStatusUniqProcessCatchedAirGanon::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot44();
}

void ftStatusUniqProcessCatchedAirGanon::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    // HYPOTHESIS: the capture is released for every status outside 0xE9..0xEA (exact order still unverified).
    if (status < 0xEB) {
        if (status >= 0xE9) {
            return;
        }
    }
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot30(0, 0);
}

bool ftStatusUniqProcessCatchedAirGanon::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    return true;
}

void ftStatusUniqProcessCatchedAirFallGanon::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: not yet matched.
}

void ftStatusUniqProcessCatchedAirFallGanon::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot44();
}

void ftStatusUniqProcessCatchedAirFallGanon::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status == 0xEA) {
        return;
    }
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot30(0, 0);
}

bool ftStatusUniqProcessCatchedAirFallGanon::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    return true;
}

void ftStatusUniqProcessCatchedAirEndGanon::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: not yet matched.
}

void ftStatusUniqProcessCatchedAirEndGanon::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot44();
}

void ftStatusUniqProcessCatchedAirEndGanon::exitStatus(soModuleAccesser* moduleAccesser, int) {
    // HYPOTHESIS: not yet matched.
}

bool ftStatusUniqProcessCatchedAirEndGanon::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot54(0);
    return true;
}
