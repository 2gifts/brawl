#pragma once

#include <StaticAssert.h>
#include <ac/ac_cmd_interpreter.h>
#include <gf/gf_task.h>
#include <so/effect/so_effect_screen.h>
#include <types.h>

class acAnimCmdConv;

// The task that runs the screen-effect command list of the one requester with the highest priority.
class soEffectScreenManager : public gfTask {
public:
    char _40[0xB8]; // HYPOTHESIS: the screen handles and the interpreter stack (built by the constructor)
    soEffectScreen m_screen; // 0xF8
    acCmdInterpreter m_interpreter; // 0xFC
    char _130[4];
    const acAnimCmdConv* m_cmd; // 0x134
    int m_priority; // 0x138
    u32 m_owner; // 0x13C

    soEffectScreenManager();
    virtual void processEffect();
    virtual ~soEffectScreenManager();

    static void initialize();
    void req(const acAnimCmdConv* cmd, int priority, u32 owner);
    void remove(const acAnimCmdConv* cmd, u32 owner);
};
static_assert(sizeof(soEffectScreenManager) == 0x140, "Class is wrong size!");

extern soEffectScreenManager* g_soEffectScreenManager;
