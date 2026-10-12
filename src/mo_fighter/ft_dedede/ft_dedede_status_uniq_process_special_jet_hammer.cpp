#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

// Skeleton: methods without bodies are stubs (not yet reconstructed).

class ftDededeStatusUniqProcessSpecialJetHammer : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialJetHammer() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDededeStatusUniqProcessSpecialJetHammer::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialJetHammer::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialJetHammer::exitStatus(soModuleAccesser* moduleAccesser, int) {
}

ftDededeStatusUniqProcessSpecialJetHammer g_ftDededeStatusUniqProcessSpecialJetHammer;
