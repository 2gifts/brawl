#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

// Skeleton: methods without bodies are stubs (not yet reconstructed).

class ftCaptainStatusUniqProcessFalconKick : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFalconKick() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftCaptainStatusUniqProcessFalconKickEnd : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFalconKickEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftCaptainStatusUniqProcessFalconKick::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftCaptainStatusUniqProcessFalconKick::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftCaptainStatusUniqProcessFalconKickEnd::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftCaptainStatusUniqProcessFalconKickEnd::execStatus(soModuleAccesser* moduleAccesser) {
}

ftCaptainStatusUniqProcessFalconKick g_ftCaptainStatusUniqProcessFalconKick;
ftCaptainStatusUniqProcessFalconKickEnd g_ftCaptainStatusUniqProcessFalconKickEnd;
