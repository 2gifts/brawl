#pragma once

#include <so/status/so_status_module_impl.h>

class soModuleAccesser;

// The status unique process of the cutter (wn_kirby_final_cutter_status_uniq_process_regular.cpp).
class wnKirbyFinalCutterStatusUniqProcessRegular : public soStatusUniqProcess {
public:
    virtual ~wnKirbyFinalCutterStatusUniqProcessRegular() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execMapCorrection(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};
extern wnKirbyFinalCutterStatusUniqProcessRegular g_wnKirbyFinalCutterStatusUniqProcessRegular;
