#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: two (value, flag) pairs initialized in the static initializer before the status instance
// (same static block layout as the heart swap unit).
struct ftStatusUniqProcessKinokoPair {
    u32 m_0;
    u32 m_4;
    ftStatusUniqProcessKinokoPair(u32 first, u32 second) : m_0(first), m_4(second) { }
};
static ftStatusUniqProcessKinokoPair g_ftStatusUniqProcessKinokoPairA(0xff, 0);
static ftStatusUniqProcessKinokoPair g_ftStatusUniqProcessKinokoPairB(0xff, 1);

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
