#include <types.h>
#include <it/it_switch_decide_module_impl.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

// soSwitchDecideModuleImpl::decideSwitch (the map name); src/mo_enemy/sora_enemy/switch/em_switch_decide_module_impl.cpp calls it by this stand-in name.
extern "C" int fn_27_7B610(soSwitchDecideModule* module, int kind, soArrayContractibleTable<const acCmdArgConv>* args,
                           soModuleAccesser* acc);

int itSwitchDecideModuleImpl::decideSwitch(int kind, soArrayContractibleTable<const acCmdArgConv>* args,
                                         soModuleAccesser* acc) {
    switch (kind) {
    case 1000:
        return 1;
    default:
        return fn_27_7B610(this, kind, args, acc);
    }
}
