#pragma once

#include <ft/ft_status_uniq_process_capture_pulled.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Status process of the Yoshi capture (capture by Yoshi's tongue / egg-capture) unit (names from the module map).
class ftStatusUniqProcessCaptureYoshi : public ftStatusUniqProcessCapturePulled {
public:
    virtual ~ftStatusUniqProcessCaptureYoshi() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

extern ftStatusUniqProcessCaptureYoshi g_ftStatusUniqProcessCaptureYoshi;
