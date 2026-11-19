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

// TODO: clean up, give variables real names
namespace jj_2BDF40 {
extern "C" char lbl_27_bss_71A0[4];

extern "C" char* fn_27_2BDF40(void)
{
    return lbl_27_bss_71A0;
}
}

// TODO: clean up, give variables real names
namespace jj_2BDF4C {
extern "C" {
void __dl__FPv(void* p);

void* fn_27_2BDF4C(void* p, s32 n) {
    if (p != nullptr && n > 0) {
        __dl__FPv(p);
    }
    return p;
}
}
}

