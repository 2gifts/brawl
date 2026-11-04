#include <so/link/so_link_event_throw.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftDiddyStatusUniqProcessSpecialSThrow : public soStatusUniqProcess {
public:
    virtual ~ftDiddyStatusUniqProcessSpecialSThrow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStop(soModuleAccesser* moduleAccesser);
};

void ftDiddyStatusUniqProcessSpecialSThrow::initStatus(soModuleAccesser* moduleAccesser) {}
void ftDiddyStatusUniqProcessSpecialSThrow::execStatus(soModuleAccesser* moduleAccesser) {}
void ftDiddyStatusUniqProcessSpecialSThrow::execFixPos(soModuleAccesser* moduleAccesser) {}
void ftDiddyStatusUniqProcessSpecialSThrow::exitStatus(soModuleAccesser* moduleAccesser, int) {}
void ftDiddyStatusUniqProcessSpecialSThrow::execStop(soModuleAccesser* moduleAccesser) {}

ftDiddyStatusUniqProcessSpecialSThrow g_ftDiddyStatusUniqProcessSpecialSThrow;

soLinkEventThrow::soLinkEventThrow(int eventKind, int value, bool enabled)
    : soLinkEventArgs(eventKind), unk8(value), unkC(-1), unk10(-1), unk14(-1),
      unk18(1.0f), unk1C(enabled) {}
