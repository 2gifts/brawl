#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <mu/wifi/mu_wifi_interface.h>
#include <sq/sq_net_friend_melee.h>
#include <sr/sr_common.h>
#include <types.h>

sqNetFriendMelee* sqNetFriendMelee::create() {
    return new (Heaps::GlobalMode) sqNetFriendMelee("sqNetFriendMelee");
}

void sqNetFriendMelee::start() {
    unk8 = 0;
    m_isNetOnline = g_gfApplication->unk110[0] != 0;
    g_GameGlobal->initSelCharData();
    switch (gfSceneManager::getInstance()->unk31C) {
    case 1:
        g_GameGlobal->m_selCharData->unk6 = 1;
        break;
    }
}

void sqNetFriendMelee::exit() { }

// TODO: clean up, give variables real names
namespace jj_383C0 {
extern "C" unsigned char lbl_8059E742;

extern "C" unsigned char fn_1_383C0() {
    return lbl_8059E742;
}
}

// TODO: clean up, give variables real names
namespace jj_389A4 {

extern "C" void* getAssignedMeleeRule__19muWifiInterfaceTaskFv(void* self);

struct TopBits {
    unsigned char hi : 3;
    unsigned char lo : 5;
};

extern "C" void fn_1_389A4(void)
{
    unsigned char* src = (unsigned char*)getAssignedMeleeRule__19muWifiInterfaceTaskFv(g_muWifiInterfaceTask);
    unsigned char* a = (unsigned char*)g_GameGlobal->m_selCharData;
    unsigned char* b = (unsigned char*)g_GameGlobal->m_setRule;
    a[0x33] = src[0];
    ((TopBits*)(a + 0x29))->hi = src[1];
    *(int*)(a + 0x40) = src[2];
    *(int*)(a + 0x44) = src[3];
    b[0x2] = src[1];
    b[0x3] = src[2];
    b[0x4] = src[3];
    b[0x5] = src[4];
    b[0x6] = src[5];
    b[0x8] = src[6];
    b[0x9] = src[7];
    b[0xB] = src[8];
    b[0xC] = src[9];
}
}

