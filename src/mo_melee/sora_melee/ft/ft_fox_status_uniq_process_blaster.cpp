#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <ft/fox/ft_fox_status_uniq_process.h>
#include <types.h>

ftFoxStatusUniqProcessBlaster g_ftFoxStatusUniqProcessBlaster;

void ftFoxStatusUniqProcessBlaster::initStatus(soModuleAccesser* moduleAccesser) { }
void ftFoxStatusUniqProcessBlaster::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftFoxStatusUniqProcessBlaster::execStatus(soModuleAccesser* moduleAccesser) { }
