#include <if/if_wifipr_mngr.h>
#include <st/loader/st_loader_info_wifi.h>
#include <types.h>

// HYPOTHESIS: the open request, the ready poll and the close of the Wi-Fi HUD (unnamed IfWifiprMngr methods in main.dol).
extern "C" {
void fn_800FC43C(IfWifiprMngr* mngr, int unk);
bool fn_800FC660(IfWifiprMngr* mngr);
void fn_800FC840(IfWifiprMngr* mngr);
}

stLoaderInfoWifi::stLoaderInfoWifi() { }

stLoaderInfoWifi::~stLoaderInfoWifi() { }

void stLoaderInfoWifi::processBegin() {
    if ((u8)_0x45[1] != 0) {
        switch (m_state) {
            case 0:
                m_state = 1;
                break;
            case 1:
                fn_800FC43C(g_IfWifiprMngr, 1);
                m_state = 2;
                break;
            case 2:
                if (fn_800FC660(g_IfWifiprMngr)) {
                    m_state = 3;
                }
                break;
            case 3:
                if ((u8)_0x45[2] == 1) {
                    m_state = 5;
                }
                break;
            case 5:
                if ((u8)_0x45[2] == 1) {
                    m_state = 6;
                } else {
                    m_state = 0;
                }
                fn_800FC840(g_IfWifiprMngr);
                break;
        }
    }
}

void stLoaderInfoWifi::entryEntity() {
    g_IfWifiprMngr->createModel();
    g_IfWifiprMngr->startMelee();
}

void stLoaderInfoWifi::removeEntity() {
    g_IfWifiprMngr->destroyModel();
}
