#pragma once

#include <em/em_switch_decide_module_impl.h>

class ftSwitchDecideModuleImpl : public soSwitchDecideModuleImpl {
public:
    virtual int decideSwitch(int kind, soArrayContractibleTable<const acCmdArgConv>* args, soModuleAccesser* acc);
    virtual ~ftSwitchDecideModuleImpl();
};
