#pragma once

#include <em/em_switch_decide_module_impl.h>

class wnPikminPikminSwitchDecideModuleImpl : public soSwitchDecideModuleImpl {
public:
    wnPikminPikminSwitchDecideModuleImpl();
    virtual int decideSwitch(int kind, soArrayContractibleTable<const acCmdArgConv>* args, soModuleAccesser* acc);
    virtual ~wnPikminPikminSwitchDecideModuleImpl();
};
