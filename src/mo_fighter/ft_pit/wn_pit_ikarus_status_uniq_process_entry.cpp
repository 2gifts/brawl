#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnPitIkarusStatusUniqProcessEntry : public soStatusUniqProcess {
public:
    virtual ~wnPitIkarusStatusUniqProcessEntry() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

class wnPitIkarusStatusUniqProcessTurn : public soStatusUniqProcess {
public:
    virtual ~wnPitIkarusStatusUniqProcessTurn() { }
};

class wnPitIkarusStatusUniqProcessAttack : public soStatusUniqProcess {
public:
    virtual ~wnPitIkarusStatusUniqProcessAttack() { }
};

wnPitIkarusStatusUniqProcessEntry g_wnPitIkarusStatusUniqProcessEntry;
wnPitIkarusStatusUniqProcessTurn g_wnPitIkarusStatusUniqProcessTurn;
wnPitIkarusStatusUniqProcessAttack g_wnPitIkarusStatusUniqProcessAttack;

void wnPitIkarusStatusUniqProcessEntry::initStatus(soModuleAccesser* moduleAccesser) { }
