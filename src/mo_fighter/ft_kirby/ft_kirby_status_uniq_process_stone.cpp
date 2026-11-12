#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Kirby's down special (Stone): the transformation, the stone itself and the return.
// TODO: the status bodies and the file-local helpers (set_visibility, free_log_group, set_kinetic_energy and the
// ftKirbySpecialLw normal buffer); only the class frame, the empty processes and the static instances are reconstructed.

class ftKirbyStatusUniqProcessStoneStart : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessStoneStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execMapCorrection(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

class ftKirbyStatusUniqProcessStoneStone : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessStoneStone() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execMapCorrection(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* damage);
};

class ftKirbyStatusUniqProcessStoneEnd : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessStoneEnd() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftKirbyStatusUniqProcessStoneStone::execStatus(soModuleAccesser* moduleAccesser) { }

void ftKirbyStatusUniqProcessStoneEnd::execStatus(soModuleAccesser* moduleAccesser) { }

ftKirbyStatusUniqProcessStoneStart g_ftKirbyStatusUniqProcessStoneStart;
ftKirbyStatusUniqProcessStoneStone g_ftKirbyStatusUniqProcessStoneStone;
ftKirbyStatusUniqProcessStoneEnd g_ftKirbyStatusUniqProcessStoneEnd;
