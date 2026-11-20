#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class wnDiddyPeanutsStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnDiddyPeanutsStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

// TODO: clean up - placeholder pair type and names (the same pair of statics as so_sinit_pair_*.cpp, from a header this
// unit includes); replace them with the real static object type and names once that header is identified.
struct SinitPair {
    s32 m_a;
    s32 m_b;
    SinitPair(s32 a, s32 b) : m_a(a), m_b(b) {}
};

SinitPair lbl_27_bss_82A8(0xFF, 0);
SinitPair lbl_27_bss_82B0(0xFF, 1);

wnDiddyPeanutsStatusUniqProcess g_wnDiddyPeanutsStatusUniqProcess;

void wnDiddyPeanutsStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) {
}

void wnDiddyPeanutsStatusUniqProcess::execStatus(soModuleAccesser* moduleAccesser) {
}

void wnDiddyPeanutsStatusUniqProcess::execStop(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0: {
        Vec3f rotation = moduleAccesser->getModelModule().getNodeRotate(1);
        rotation.m_x = moduleAccesser->getWorkManageModule().getFloat(0x21000000);
        moduleAccesser->getModelModule().setNodeRotate(1, &rotation);
        break;
    }
    }
}

void wnDiddyPeanutsStatusUniqProcess::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
}
