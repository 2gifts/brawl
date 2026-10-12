#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnFoxLandMasterStatusUniqProcessCommon : public soStatusUniqProcess {
public:
    virtual ~wnFoxLandMasterStatusUniqProcessCommon() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execMapCorrection(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnFoxLandMasterStatusUniqProcessCommon g_wnFoxLandMasterStatusUniqProcessCommon;

void wnFoxLandMasterStatusUniqProcessCommon::initStatus(soModuleAccesser* moduleAccesser) { }
void wnFoxLandMasterStatusUniqProcessCommon::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void wnFoxLandMasterStatusUniqProcessCommon::execStatus(soModuleAccesser* moduleAccesser) { }
void wnFoxLandMasterStatusUniqProcessCommon::execMapCorrection(soModuleAccesser* moduleAccesser) { }
void wnFoxLandMasterStatusUniqProcessCommon::execFixPos(soModuleAccesser* moduleAccesser) { }
