#include <so/motion/so_motion_module_impl.h>
#include <so/so_param_utility.h>
#include <types.h>
#include <wn/pikmin/wn_pikmin_pikmin.h>
#include <wn/pikmin/wn_pikmin_pikmin_switch_decide_module_impl.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

// soSwitchDecideModuleImpl::decideSwitch (the map name); src/mo_enemy/sora_enemy/switch/em_switch_decide_module_impl.cpp calls it by this stand-in name.
extern "C" int fn_27_7B610(soSwitchDecideModule* module, int kind, soArrayContractibleTable<const acCmdArgConv>* args,
                           soModuleAccesser* acc);

int wnPikminPikminSwitchDecideModuleImpl::decideSwitch(int kind, soArrayContractibleTable<const acCmdArgConv>* args,
                                                    soModuleAccesser* acc) {
    switch (kind) {
    case 1000: {
        const acCmdArg& motionArg = acGetCmdArg(*args, 0);
        int motionKind = motionArg.getIntData();
        int selectedMotion;
        if (motionKind != acc->m_enumerationStart->m_motionModule->getKind()) {
            selectedMotion = motionKind;
        } else {
            wnPikminPikmin* pikmin = dynamic_cast<wnPikminPikmin*>(acc->m_stageObject);
            selectedMotion = !pikmin ? motionKind :
                soParamUtility::getRandomMotionKind(pikmin->getWaitAnimTable(), 0);
        }
        return selectedMotion - motionKind;
    }
    default:
        return fn_27_7B610(this, kind, args, acc);
    }
}

wnPikminPikminSwitchDecideModuleImpl::~wnPikminPikminSwitchDecideModuleImpl() { }

wnPikminPikminSwitchDecideModuleImpl::wnPikminPikminSwitchDecideModuleImpl() { }
