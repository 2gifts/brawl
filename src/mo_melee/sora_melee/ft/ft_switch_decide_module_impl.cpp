#include <types.h>
#include <ft/ft_switch_decide_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_param_utility.h>
#include <so/so_value_accesser.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

// soSwitchDecideModuleImpl::decideSwitch (the map name); src/mo_enemy/sora_enemy/switch/em_switch_decide_module_impl.cpp calls it by this stand-in name.
extern "C" int fn_27_7B610(soSwitchDecideModule* module, int kind, soArrayContractibleTable<const acCmdArgConv>* args,
                           soModuleAccesser* acc);

int ftSwitchDecideModuleImpl::decideSwitch(int kind, soArrayContractibleTable<const acCmdArgConv>* args,
                                         soModuleAccesser* acc) {
    switch (kind) {
    case 1000:
    case 1001: {
        const acCmdArg& motionArg = acGetCmdArg(*args, 0);
        if (motionArg.getArgType() != 0) {
            return 0;
        }
        int motionKind = motionArg.getIntData();
        if (motionKind != acc->m_enumerationStart->m_motionModule->getKind()) {
            return 0;
        }
        const void* motionTable;
        if (kind == 1000) {
            motionTable = soValueAccesser::getConstantIndefinite(acc, 0xA801, 0);
        } else if (kind == 1001) {
            motionTable = soValueAccesser::getConstantIndefinite(acc, 0xA809, 0);
        } else {
            return 0;
        }
        if (motionTable) {
            int selectedMotion = soParamUtility::getRandomMotionKind(motionTable, motionKind);
            return selectedMotion == motionKind ? -1 : selectedMotion - motionKind;
        }
        break;
    }
    }
    return fn_27_7B610(this, kind, args, acc);
}

