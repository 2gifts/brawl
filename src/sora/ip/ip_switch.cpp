#include <ip/ip_switch.h>
#include <sr/sr_common.h>
#include <types.h>

extern "C" ipSwitch* fn_8004A670(ipSwitch* p1);

ipSwitch* ipSwitch::getInstance() {
    if (!g_ipSwitch) {
        ipSwitch* p = new (Heaps::SystemFW) ipSwitch;
        if (p) {
            p = fn_8004A670(p);
        }
        g_ipSwitch = p;
    }
    return g_ipSwitch;
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_804977D0[0x10] __attribute__((aligned(8))) = { 0 };
