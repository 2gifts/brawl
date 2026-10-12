#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Aether (Ike Special N) end at full charge.
class ftIkeStatusUniqProcessSpecialNEndMax : public soStatusUniqProcess {
public:
    virtual ~ftIkeStatusUniqProcessSpecialNEndMax() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftIkeStatusUniqProcessSpecialNEndMax g_ftIkeStatusUniqProcessSpecialNEndMax;

// MATCH-ONLY: the damage module's entry at vtable offset 0x4c is reached through the vptr stored at +8 of the
// module object and is not declared in the shared headers. Called with (index 0, value) like the target.
typedef void (*ftIkeDamageSlotFn)(void* self, u32 index, float value);

static void callDamageSlot(void* damage, float value) {
    void** vtable = *reinterpret_cast<void***>(reinterpret_cast<char*>(damage) + 8);
    ftIkeDamageSlotFn fn = reinterpret_cast<ftIkeDamageSlotFn>(vtable[0x4c / 4]);
    fn(damage, 0, value);
}

void ftIkeStatusUniqProcessSpecialNEndMax::initStatus(soModuleAccesser* acc) {
    acc->getKineticModule().getEnergy(0)->enable();
}

void ftIkeStatusUniqProcessSpecialNEndMax::execFixPos(soModuleAccesser* acc) {
    if (acc->getCollisionAttackModule().isAttack(0, false)) {
        if (!acc->getWorkManageModule().isFlag(0x22000012)) {
            // HYPOTHESIS: the constant id is the Ike damage-scale constant used when the full charge hits.
            float value = acc->getConstantFloatKirby(0xfa2);
            callDamageSlot(&acc->getDamageModule(), value);
            acc->getWorkManageModule().onFlag(0x22000012);
        }
    }
}

void ftIkeStatusUniqProcessSpecialNEndMax::exitStatus(soModuleAccesser*, int) { }
