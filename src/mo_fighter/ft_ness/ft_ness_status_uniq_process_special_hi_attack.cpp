#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

// Ness's PSI Magnet attack (special hi attack). Only the class shape is in place; the status bodies are not
// reconstructed yet.
class ftNessStatusUniqProcessSpecialHiAttack : public soStatusUniqProcess {
public:
    virtual ~ftNessStatusUniqProcessSpecialHiAttack() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

ftNessStatusUniqProcessSpecialHiAttack g_ftNessStatusUniqProcessSpecialHiAttack;

void ftNessStatusUniqProcessSpecialHiAttack::initStatus(soModuleAccesser*) {}

void ftNessStatusUniqProcessSpecialHiAttack::execStatus(soModuleAccesser*) {}

void ftNessStatusUniqProcessSpecialHiAttack::execStop(soModuleAccesser* moduleAccesser) {
    execStatus(moduleAccesser);
}

void ftNessStatusUniqProcessSpecialHiAttack::execFixPosCounter(soModuleAccesser*) {}

void ftNessStatusUniqProcessSpecialHiAttack::exitStatus(soModuleAccesser*, int) {}
