#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: four u32 values (0xff, 0, 0xff, 1) initialized in the static initializer before the status instance
// (same static block layout as the heart swap unit).
struct ftStatusUniqProcessKinokoInit {
    u32 m_0;
    u32 m_4;
    u32 m_8;
    u32 m_c;
    ftStatusUniqProcessKinokoInit() : m_0(0xff), m_4(0), m_8(0xff), m_c(1) { }
};
static ftStatusUniqProcessKinokoInit g_ftStatusUniqProcessKinokoInit;

class ftStatusUniqProcessKinoko : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessKinoko() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);

    float getFixScale(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessKinoko g_ftStatusUniqProcessKinoko;

float ftStatusUniqProcessKinoko::getFixScale(soModuleAccesser* moduleAccesser) { return 1.0f; }

void ftStatusUniqProcessKinoko::initStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessKinoko::execStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessKinoko::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) { }
