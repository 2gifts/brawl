#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnKirbyFinalCutterStatusUniqProcessRegular : public soStatusUniqProcess {
public:
    virtual ~wnKirbyFinalCutterStatusUniqProcessRegular() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execMapCorrection(soModuleAccesser* moduleAccesser);
};

wnKirbyFinalCutterStatusUniqProcessRegular g_wnKirbyFinalCutterStatusUniqProcessRegular;

void wnKirbyFinalCutterStatusUniqProcessRegular::initStatus(soModuleAccesser* moduleAccesser) { }
void wnKirbyFinalCutterStatusUniqProcessRegular::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void wnKirbyFinalCutterStatusUniqProcessRegular::execStatus(soModuleAccesser* moduleAccesser) { }
void wnKirbyFinalCutterStatusUniqProcessRegular::execMapCorrection(soModuleAccesser* moduleAccesser) { }
