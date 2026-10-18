#pragma once

#include <ft/ft_status_uniq_process_capture_pulled.h>
#include <types.h>

class soModuleAccesser;

// Capture wait process (the fighter being held). Later capture statuses (CaptureDamage) derive from it.
class ftStatusUniqProcessCaptureWait : public ftStatusUniqProcessCapturePulled {
public:
    virtual ~ftStatusUniqProcessCaptureWait() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};
