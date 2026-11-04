#pragma once

#include <gf/gf_thread.h>
#include <types.h>

class nteND : public gfThread {
public:
    int unk340;
    u32 unk344;
    u32 unk348;
    bool unk34C;
    bool unk34D;
    bool unk34E;
    // HYPOTHESIS: callback interface pointers; their concrete types are unknown.
    void* unk350;
    void* unk354;
    bool unk358;

    explicit nteND(bool);
    ~nteND();
    virtual void run();
    // HYPOTHESIS: callback parameter types are unknown; strings pass to DWC.
    bool startupND(void*, const char*, const char*);
    bool finishND(void*);
    bool download(void*, const char*, const char*, const char*);
    bool _processAsync();
    bool _errorMonitor();

    // HYPOTHESIS: no request is pending and the worker thread is parked.
    bool isIdle() {
        bool suspended;
        return unk340 == 0 && (suspended = OSIsThreadSuspended(&m_thread) == true, suspended);
    }
};
