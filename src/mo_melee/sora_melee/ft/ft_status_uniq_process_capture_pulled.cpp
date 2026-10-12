#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// MATCH-ONLY: the -O2,s fighter flags stop the implicit constructor inlining into the static initializer;
// the Matching neighbours build at the default -O4,p.
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

#include <ft/ft_status_uniq_process_capture_pulled.h>

ftStatusUniqProcessCapturePulled g_ftStatusUniqProcessCapturePulled;

// TODO: bodies not decoded yet.
void ftStatusUniqProcessCapturePulled::initStatus(soModuleAccesser*) {}
void ftStatusUniqProcessCapturePulled::exitStatus(soModuleAccesser*, int) {}
void ftStatusUniqProcessCapturePulled::execStatus(soModuleAccesser*) {}
bool ftStatusUniqProcessCapturePulled::checkDamage(soModuleAccesser*, void*) { return false; }
void ftStatusUniqProcessCapturePulled::execStatusCommon(soModuleAccesser*, int, int, u32) {}
