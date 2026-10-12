#include <ft/ft_status_uniq_process_capture_yoshi.h>
#include <ft/ft_status_uniq_process_views.h>
#include <so/so_module_accesser.h>
#include <types.h>

ftStatusUniqProcessCaptureYoshi g_ftStatusUniqProcessCaptureYoshi;

// sora_melee helper called when the capture work flag is not set (name unknown).
extern "C" void fn_27_179BF8(void* process, soModuleAccesser* moduleAccesser);

void ftStatusUniqProcessCaptureYoshi::execStatus(soModuleAccesser* moduleAccesser) {
    if (!moduleAccesser->getWorkManageModule().isFlag(0x1200003B)) {
        fn_27_179BF8(this, moduleAccesser);
    }
}

void ftStatusUniqProcessCaptureYoshi::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    // HYPOTHESIS: exit body not yet matched (large paired-single vector update and link/kinetic calls).
}

bool ftStatusUniqProcessCaptureYoshi::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    return reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot34(-1);
}
