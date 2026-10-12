#include <ip/ip_padconfig.h>
#include <sr/sr_common.h>
#include <types.h>

extern "C" void fn_800485A4();

static ipPadConfig* g_ipPadConfig;

ipPadConfig* ipPadConfig::getInstance() {
    if (!g_ipPadConfig) {
        ipPadConfig* temp = (ipPadConfig*) operator new(0x1AC, Heaps::SystemFW);
        if (temp != NULL) {
            // HYPOTHESIS: byte at 0xB4 is an unnamed flag; ipPadConfig in the header has no field there
            ((u8*) temp)[0xB4] = 0;
            fn_800485A4();
        }
        g_ipPadConfig = temp;
    }
    return g_ipPadConfig;
}
